#pragma once

#include "anker_file_utils.hpp"
#include "anker_math.hpp"
#include "anker_serialize.hpp"
#include "anker_status.hpp"
#include "anker_string_utils.hpp"

namespace Anker {

////////////////////////////////////////////////////////////
// JSON Reader

// The JSON reader serializes data from the given JSON input into the given
// object. The JSON DOM is traversed via the help of a stack. This stack is
// internal and current() refers to the JSON value at the top of the stack.
//
// When traversing manually, e.g. by customizing the serialization function for
// a specific type, use the pushKey and popKey functions to navigate between
// fields.
//
// Arrays elements are accessed via arrayElement.
class JsonReader {
  public:
	template <typename T>
	static Status fromFile(const fs::path& filepath, T& outValue) requires Serializable<JsonReader, T>
	{
		std::string buffer;
		ANKER_TRY(readFile(filepath, buffer));

		JsonReader read(buffer);
		read(outValue);
		return OK;
	}

	explicit JsonReader(std::string_view input)
	{
		const auto flags = rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag;

		m_doc.Parse<flags>(input.data(), input.size());
		m_values.push(&m_doc);
	}

	explicit JsonReader(std::span<const uint8_t> input)
	    : JsonReader(std::string_view(reinterpret_cast<const char*>(input.data()), input.size()))
	{}

	void operator()(bool& outValue) { outValue = current().GetBool(); }
	void operator()(int& outValue) { outValue = current().GetInt(); }
	void operator()(uint32_t& outValue) { outValue = current().GetUint(); }
	void operator()(float& outValue) { outValue = current().GetFloat(); }
	void operator()(double& outValue) { outValue = current().GetDouble(); }
	void operator()(std::string_view& outValue) { outValue = {current().GetString(), current().GetStringLength()}; }
	void operator()(std::string& outValue) { outValue = current().GetString(); }

	void operator()(EntityID& outValue) { (*this)(reinterpret_cast<entt::id_type&>(outValue)); }

	template <typename EnumType>
	void operator()(EnumType& outValue) requires std::is_enum_v<EnumType>
	{
		if constexpr (FromStringable<EnumType>) {
			std::string_view view;
			(*this)(view);
			if (auto e = fromString<EnumType>(view)) {
				outValue = *e;
			}
		} else {
			outValue = EnumType(current().GetInt());
		}
	}

	template <typename T>
	void operator()(T& outObject) requires internal::SerializableClass<JsonReader, T>
	{
		if constexpr (internal::CustomSerialization<JsonReader, T>) {
			serialize(*this, outObject);
		}

		else if constexpr (refl::is_reflectable<T>()) {
			// For fields accessible via getter/setter (i.e. properties), TODO
			auto properties = filter(refl::reflect<T>().members, [](auto member) { //
				return is_property(member) && is_writable(member) && has_reader(member);
			});
			for_each(properties, [&](auto member) {
				auto valueCopy = std::invoke(get_reader(member).pointer, outObject);
				fieldByReflection(get_display_name(member), valueCopy);
				std::invoke(get_writer(member), outObject, valueCopy);
			});

			// Fields can be accessed directly via reference.
			auto fields = filter(refl::reflect<T>().members, [](auto member) { return is_field(member); });
			for_each(fields, [&](auto member) { //
				fieldByReflection(member.name.c_str(), member(outObject));
			});
		}
	}

	bool isArray() const { return current().IsArray(); }
	bool isObject() const { return current().IsObject(); }

	// Navigate to the value of the given key in the current object. Returns
	// true if the field is present, in which case you have to call popKey when
	// you are done with this field.
	bool pushKey(const char* key)
	{
		if (auto it = current().FindMember(key); it != current().MemberEnd()) {
			m_values.push(&it->value);
			return true;
		} else {
			return false;
		}
	}
	void popKey() { m_values.pop(); }

	// Similar to pushKey, but for accessing an element of the current array.
	bool pushIndex(uint32_t index)
	{
		if (index < current().GetArray().Size()) {
			m_values.push(&current().GetArray()[index]);
			return true;
		} else {
			return false;
		}
	}
	void popIndex() { m_values.pop(); }

	// Convenience function to directly read a field from the current object.
	template <typename T>
	bool field(const char* key, T& outValue) requires Serializable<JsonReader, T>
	{
		if (pushKey(key)) {
			(*this)(outValue);
			popKey();
			return true;
		} else {
			return false;
		}
	}

	// Reads the element at the given index from the current array.
	template <typename T>
	bool arrayElement(uint32_t index, T& outValue) requires Serializable<JsonReader, T>
	{
		if (pushIndex(index)) {
			(*this)(outValue);
			popIndex();
			return true;
		} else {
			return false;
		}
	}

	uint32_t arrayLength() const { return current().GetArray().Size(); }

	// Invokes function for each field in the current object.
	template <typename F>
	void forEach(F function) requires std::invocable<F, const char*>
	{
		for (auto& [k, v] : current().GetObject()) {
			m_values.push(&v);
			function(k.GetString());
			m_values.pop();
		}
	}

	// Invokes function for each element in the current array.
	template <typename F>
	void forEach(F function) requires std::invocable<F, uint32_t>
	{
		for (uint32_t i = 0; i < current().GetArray().Size(); ++i) {
			m_values.push(&current().GetArray()[i]);
			function(i);
			m_values.pop();
		}
	}

	// Invokes function for each field in the object or element in the array
	// (depending on F's signature), located at the given field key.
	template <typename F>
	bool forEach(const char* key, F function)
	{
		if (pushKey(key)) {
			forEach(function);
			popKey();
			return true;
		} else {
			return false;
		}
	}

	// Invokes function for each field in the object or element in the array
	// (depending on F's signature), located at the given array index.
	template <typename F>
	bool forEach(uint32_t index, F function)
	{
		if (pushIndex(index)) {
			forEach(function);
			popIndex();
			return true;
		} else {
			return false;
		}
	}

  private:
	template <typename T>
	void fieldByReflection(const char* key, T& outValue)
	{
		if constexpr (Serializable<JsonReader, T>) {
			field(key, outValue);
		} else {
			ANKER_WARN("JsonReader: field not serialized {}", key);
		}
	}

	rapidjson::Document::GenericValue& current() const { return *m_values.top(); }

	rapidjson::Document m_doc;
	std::stack<rapidjson::Document::GenericValue*> m_values;
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
		return writeFile(filepath, write.output());
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
	void operator()(uint32_t value) { m_writer.Uint(value); }
	void operator()(float value) { m_writer.Double(value); }
	void operator()(const char* value) { m_writer.String(value); }
	void operator()(std::string_view value) { m_writer.String(value.data(), static_cast<uint32_t>(value.size())); }

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
	void operator()(const T& object) requires internal::SerializableClass<JsonWriter, T>
	{
		m_writer.StartObject();

		if constexpr (internal::CustomSerialization<JsonWriter, T>) {
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
