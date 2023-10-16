#pragma once

#include "anker_file_utils.hpp"
#include "anker_math.hpp"
#include "anker_serialize.hpp"
#include "anker_status.hpp"
#include "anker_string_utils.hpp"
#include "anker_type_utils.hpp"

namespace Anker {

////////////////////////////////////////////////////////////
// JSON Reader

// The JSON reader serializes data from the given JSON input into the given
// object. The JSON DOM is traversed via the help of a stack. This stack is
// internal and current() refers to the JSON value at the top of the stack.
//
// When traversing manually, e.g. by customizing the serialization function for
// a specific type, use the push and pop functions to navigate between fields.
//
// Arrays elements are accessed via arrayElement.
class JsonReader {
  public:
	template <typename T>
	static Status fromFile(T& outValue, const fs::path& filepath) requires Serializable<JsonReader, T>
	{
		std::string buffer;
		ANKER_TRY(readFile(buffer, filepath));

		JsonReader read(buffer);
		if (!read(outValue)) {
			return FormatError;
		}
		return Ok;
	}

	Status parse(std::string_view input, std::string_view identifier = {})
	{
		m_values.clear();

		const auto parseFlags = rapidjson::kParseCommentsFlag //
		                      | rapidjson::kParseTrailingCommasFlag;

		rapidjson::ParseResult ok = m_doc.Parse<parseFlags>(input.data(), input.size());
		if (!ok) {
			ANKER_ERROR("{}: {} offset={}", identifier, rapidjson::GetParseError_En(ok.Code()), ok.Offset());
			return FormatError;
		}

		m_values.push_back(&m_doc);
		return Ok;
	}

	Status parse(std::span<const u8> input, std::string_view identifier = {})
	{
		return parse(asStringView(input), identifier);
	}

	bool operator()(bool& outValue) { return readPrimitive(outValue); }
	bool operator()(int& outValue) { return readPrimitive(outValue); }
	bool operator()(u32& outValue) { return readPrimitive(outValue); }
	bool operator()(float& outValue) { return readPrimitive(outValue); }
	bool operator()(double& outValue) { return readPrimitive(outValue); }
	bool operator()(std::string& outValue) { return readPrimitive(outValue); }

	bool operator()(EntityID& outValue)
	{
		entt::id_type id = 0;
		ANKER_TRY(readPrimitive(id));
		outValue = EntityID(id);
		return true;
	}

	template <typename EnumType>
	bool operator()(EnumType& outValue) requires std::is_enum_v<EnumType>
	{
		if constexpr (FromStringable<EnumType>) {
			ANKER_TRY(current().IsString());
			return fromString(outValue, current().GetString());
		} else {
			int i;
			ANKER_TRY(readPrimitive(i));
			outValue = EnumType(i);
			return true;
		}
	}

	template <typename T>
	bool operator()(T& outObject) requires Internal::SerializableClass<JsonReader, T>
	{
		bool ok = true;

		if constexpr (Internal::CustomSerialization<JsonReader, T>) {
			ok = serialize(*this, outObject);
		}

		else if constexpr (refl::is_reflectable<T>()) {
			// For fields accessible via getter/setter (i.e. properties)
			auto properties = filter(refl::reflect<T>().members, [](auto member) { //
				return is_property(member) && is_writable(member) && has_reader(member);
			});
			for_each(properties, [&](auto member) {
				auto valueCopy = std::invoke(get_reader(member).pointer, outObject);
				ok = fieldByReflection(get_display_name(member), valueCopy) && ok;
				std::invoke(get_writer(member), outObject, valueCopy);
			});

			// Fields can be accessed directly via reference.
			auto fields = filter(refl::reflect<T>().members, [](auto member) { return is_field(member); });
			for_each(fields, [&](auto member) { //
				ok = fieldByReflection(member.name.c_str(), member(outObject)) && ok;
			});
		}

		return ok;
	}

	bool isArray() const { return current().IsArray(); }
	bool isObject() const { return current().IsObject(); }
	bool hasKey(const char* key) const { return current().HasMember(key); }

	u32 arrayLength() const
	{
		if (current().IsArray()) {
			return current().GetArray().Size();
		} else {
			return 0;
		}
	}

	// Navigate to the value of the given key in the current object. Returns
	// true if the field is present, in which case you have to call pop when you
	// are done with this field.
	bool push(const char* key)
	{
		if (current().IsObject()) {
			if (auto it = current().FindMember(key); it != current().MemberEnd()) {
				m_values.push_back(&it->value);
				return true;
			}
		}
		return false;
	}

	// Same as above, but for accessing an element of the current array.
	bool push(u32 index)
	{
		if (current().IsArray()) {
			if (index < current().GetArray().Size()) {
				m_values.push_back(&current().GetArray()[index]);
				return true;
			}
		}
		return false;
	}

	void pop() { m_values.pop_back(); }

	// Convenience function to directly read a field from the current object.
	template <typename T>
	bool field(const char* key, T& outValue) requires Serializable<JsonReader, T>
	{
		ANKER_TRY(push(key));
		ANKER_DEFER([&] { pop(); });
		return (*this)(outValue);
	}

	// Reads the element at the given index from the current array.
	template <typename T>
	bool arrayElement(u32 index, T& outValue) requires Serializable<JsonReader, T>
	{
		ANKER_TRY(push(index));
		ANKER_DEFER([&] { pop(); });
		return (*this)(outValue);
	}

	// Invokes function for each field in the current object.
	template <typename F>
	void forEach(F function) requires std::invocable<F, const char*>
	{
		if (current().IsObject()) {
			for (auto& [k, v] : current().GetObject()) {
				m_values.push_back(&v);
				function(k.GetString());
				m_values.pop_back();
			}
		}
	}

	// Invokes function for each element in the current array.
	template <typename F>
	void forEach(F function) requires std::invocable<F, u32>
	{
		if (current().IsArray()) {
			for (u32 i = 0; i < current().GetArray().Size(); ++i) {
				m_values.push_back(&current().GetArray()[i]);
				function(i);
				m_values.pop_back();
			}
		}
	}

	// Invokes function for each field in the object or element in the array
	// (depending on F's signature), located at the given field key.
	template <typename F>
	bool forEach(const char* key, F function)
	{
		ANKER_TRY(push(key));
		forEach(function);
		pop();
		return true;
	}

	// Invokes function for each field in the object or element in the array
	// (depending on F's signature), located at the given array index.
	template <typename F>
	bool forEach(u32 index, F function)
	{
		ANKER_TRY(push(index));
		forEach(function);
		pop();
		return true;
	}

  private:
	template <typename T>
	bool readPrimitive(T& outValue)
	{
		if constexpr (std::is_same_v<T, bool>) {
			ANKER_TRY(current().IsBool());
		} else if constexpr (std::is_arithmetic_v<T>) {
			ANKER_TRY(current().IsNumber());
		} else if constexpr (std::is_same_v<T, std::string>) {
			ANKER_TRY(current().IsString());
		} else {
			static_assert(AlwaysFalse<T>, "Not a primitive");
		}

		outValue = current().Get<T>();
		return true;
	}

	template <typename T>
	bool fieldByReflection(const char* key, T& outValue)
	{
		if constexpr (Serializable<JsonReader, T>) {
			return field(key, outValue);
		} else {
			ANKER_WARN("JsonReader: field not serialized {}", key);
			return true;
		}
	}

	rapidjson::Document::GenericValue& current() const { return *m_values.back(); }

	rapidjson::Document m_doc;
	std::vector<rapidjson::Document::GenericValue*> m_values;
};

////////////////////////////////////////////////////////////
// JSON Writer

class JsonWriter {
  public:
	template <typename T>
	static Status toFile(const fs::path& filepath, const T& value) requires Serializable<JsonWriter, T>
	{
		JsonWriter write;
		write(value);
		return writeFile(write.output(), filepath);
	}

	JsonWriter()
	{
		m_writer.SetIndent(' ', 2);
		m_writer.SetMaxDecimalPlaces(6);
		m_writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	}

	std::string_view output() const { return {m_buffer.GetString(), m_buffer.GetSize()}; }

	void operator()(bool value) { m_writer.Bool(value); }
	void operator()(int value) { m_writer.Int(value); }
	void operator()(u32 value) { m_writer.Uint(value); }
	void operator()(float value) { m_writer.Double(value); }
	void operator()(const char* value) { m_writer.String(value); }
	void operator()(std::string_view value) { m_writer.String(value.data(), u32(value.size())); }

	void operator()(EntityID value) { (*this)(static_cast<entt::id_type>(value)); }

	template <typename EnumType>
	void operator()(EnumType value) requires std::is_enum_v<EnumType>
	{
		if constexpr (ToStringable<EnumType>) {
			(*this)(to_string(value));
		} else {
			(*this)(std::underlying_type_t<EnumType>(value));
		}
	}

	template <typename T>
	void operator()(const T& object) requires Internal::SerializableClass<JsonWriter, T>
	{
		m_writer.StartObject();

		if constexpr (Internal::CustomSerialization<JsonWriter, T>) {
			serialize(*this, object);
		}

		else if constexpr (refl::is_reflectable<T>()) {
			// For fields accessible via getter/setter (i.e. properties), we can
			// just use the getter.
			auto properties = filter(refl::reflect<T>().members, [](auto member) { //
				return is_property(member) && is_readable(member) && has_writer(member);
			});
			for_each(properties, [&](auto member) {
				fieldByReflection(get_display_name(member), std::invoke(get_reader(member).pointer, object));
			});

			// Fields can be accessed directly via reference.
			auto fields = filter(refl::reflect<T>().members, [](auto member) { return is_field(member); });
			for_each(fields, [&](auto member) { //
				fieldByReflection(member.name.c_str(), member(object));
			});
		}

		m_writer.EndObject();
	}

	template <typename T>
	void field(const char* key, const T& value) requires Serializable<JsonWriter, T>
	{
		m_writer.Key(key);
		(*this)(value);
	}

	void key(const char* key) { m_writer.Key(key); }
	void key(std::string_view key) { m_writer.Key(key.data(), rapidjson::SizeType(key.size())); }
	void beginArray() { m_writer.StartArray(); }
	void endArray() { m_writer.EndArray(); }
	void beginObject() { m_writer.StartObject(); }
	void endObject() { m_writer.EndObject(); }

  private:
	template <typename T>
	void fieldByReflection(const char* key, const T& value)
	{
		if constexpr (Serializable<JsonWriter, T>) {
			field(key, value);
		} else {
			ANKER_WARN("JsonWriter: field not serialized {}", key);
		}
	}

	rapidjson::StringBuffer m_buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> m_writer{m_buffer};
};

} // namespace Anker
