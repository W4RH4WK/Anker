#include <anker/game/anker_map.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/graphics/anker_map_renderer.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

using TileId = uint32_t;
constexpr TileId EmptyTile = 0;

enum TileFlipFlag : TileId {
	FlipHorizontal = 1u << 31,
	FlipVertical = 1u << 30,
	FlipDiagonal = 1u << 29,
};
constexpr TileId FlipMask = FlipHorizontal | FlipVertical | FlipDiagonal;

// The loader for .tmj files. This loader should not be exposed, instead a
// loadMap function is the primary interface for map loading. This is only
// implemented as a class to make the implementation more readable.
//
// An instance may be used only once; create a new instance if you need to load
// another map.
class TmjLoader {
  public:
	TmjLoader(Scene& scene, AssetCache& assetCache)
	    : m_scene(scene), m_assetCache(assetCache), m_layerSceneNode(&scene.createEntity("Map").emplace<SceneNode>())
	{}

	TmjLoader(const TmjLoader&) = delete;
	TmjLoader& operator=(const TmjLoader&) = delete;
	TmjLoader(TmjLoader&&) noexcept = delete;
	TmjLoader& operator=(TmjLoader&&) noexcept = delete;

	// Loader entry point. This will load the given map and populate the scene
	// given on initialization.
	Status load(std::string_view identifier)
	{
		m_tmjIdentifier = identifier;

		auto filepath = std::string{identifier} + ".tmj";

		ByteBuffer tmjData;
		ANKER_TRY(g_assetDataLoader.load(tmjData, filepath));
		ANKER_TRY(m_tmjReader.parse(tmjData, filepath));

		ANKER_TRY(loadTilesets());
		ANKER_TRY(loadLayers());

		return Ok;
	}

  private:
	////////////////////////////////////////////////////////////
	// Tileset
	//
	// A Tileset is used to populate a MapLayer's vertex buffer. Specifically,
	// we need to get the texture coordinates for each tile.

	struct Tileset {
		Rect2 textureCoordinates(uint32_t gid) const
		{
			ANKER_ASSERT(gid >= firstTileId);
			uint32_t index = gid - firstTileId;

			ANKER_ASSERT(index < tileCount.x * tileCount.y);
			float x = float(index % tileCount.x);
			float y = float(index / tileCount.x);

			Vec2 texSize = Vec2(texture->info.size);

			return Rect2(Vec2(tileSize) / texSize, //
			             Vec2{x, y} * Vec2(tileSize) / texSize);
		}

		TileId firstTileId = 1;
		Vec2u tileCount;
		Vec2u tileSize;
		AssetPtr<Texture> texture;
	};

	Status loadTilesets()
	{
		Status status;

		m_tmjReader.forEach("tilesets", [&](uint32_t tilesetIndex) {
			if (not status) {
				return;
			}

			Tileset tileset;

			std::string source;
			bool formatOk = m_tmjReader.field("source", source) //
			             && m_tmjReader.field("firstgid", tileset.firstTileId);
			if (!formatOk) {
				ANKER_ERROR("{}: Tileset {}: Invalid format", m_tmjIdentifier, tilesetIndex);
				status = FormatError;
				return;
			}

			fs::path tilesetFilepath = fs::path(m_tmjIdentifier).replace_filename(source);
			if (status = loadTileset(tileset, tilesetFilepath); !status) {
				return;
			}

			m_tilesets.emplace_back(std::move(tileset));
		});

		// Sorting the Tilesets in reverse order for the linear lookup we need
		// later on when using global ids.
		std::ranges::sort(m_tilesets, [](auto& a, auto& b) { return a.firstTileId >= b.firstTileId; });

		return Ok;
	}

	Status loadTileset(Tileset& tileset, const fs::path& filepath)
	{
		ByteBuffer tsjData;
		ANKER_TRY(g_assetDataLoader.load(tsjData, filepath));

		JsonReader tsjReader;
		ANKER_TRY(tsjReader.parse(tsjData, filepath.string()));

		std::string image;
		uint32_t tileCountTotal = 0;
		bool formatOk = tsjReader.field("image", image)                  //
		             && tsjReader.field("tilecount", tileCountTotal)     //
		             && tsjReader.field("columns", tileset.tileCount.x)  //
		             && tsjReader.field("tilewidth", tileset.tileSize.x) //
		             && tsjReader.field("tileheight", tileset.tileSize.y);
		if (!formatOk) {
			ANKER_ERROR("{}: Invalid format", filepath);
			return FormatError;
		}

		tileset.tileCount.y = tileCountTotal / tileset.tileCount.x;

		auto imageIdentifier = toIdentifier(fs::path(filepath).replace_filename(image));
		tileset.texture = m_assetCache.loadTexture(imageIdentifier);

		return Ok;
	}

	////////////////////////////////////////////////////////////

	Status loadLayers()
	{
		Status status;

		m_tmjReader.forEach("layers", [&](uint32_t) {
			if (not status) {
				return;
			}

			// Load common layer parameters

			std::string type;
			if (!m_tmjReader.field("type", type)) {
				ANKER_ERROR("{}: Missing type field", m_tmjIdentifier);
				status = FormatError;
				return;
			}

			std::string layerName;
			m_tmjReader.field("name", layerName);
			if (layerName.empty()) {
				layerName = "Map Layer";
			}

			Vec2 layerOffset;
			m_tmjReader.field("x", layerOffset.x);
			m_tmjReader.field("y", layerOffset.y);
			layerOffset = convertCoordinates(layerOffset);

			m_layerSceneNode = &m_scene //
			                        .createEntity(layerName)
			                        .emplace<SceneNode>(Transform2D(layerOffset), m_layerSceneNode);
			ANKER_DEFER([&] { m_layerSceneNode = m_layerSceneNode->parent(); });

			Vec4 color = Vec4(1);
			if (std::string colorHtml; m_tmjReader.field("tintcolor", colorHtml)) {
				colorFromHtml(color, colorHtml);
			}
			m_tmjReader.field("opacity", color.w);
			m_colorStack.push_back(color);
			ANKER_DEFER([&] { m_colorStack.pop_back(); });

			Vec2 parallax = Vec2(1);
			m_tmjReader.field("parallaxx", parallax.x);
			m_tmjReader.field("parallaxy", parallax.y);
			m_parallaxStack.push_back(parallax);
			ANKER_DEFER([&] { m_parallaxStack.pop_back(); });

			// Dispatch

			if (type == "tilelayer") {
				status = loadTileLayer();
			} else if (type == "objectgroup") {
				if (layerName.starts_with("Collision")) {
					status = loadCollisionLayer();
				} else {
					status = loadObjectLayer();
				}
			} else if (type == "group") {
				status = loadLayers();
			} else {
				ANKER_ERROR("{}: Unknown layer type: {}", m_tmjIdentifier, type);
				status = FormatError;
			}
		});

		return status;
	}

	Status loadTileLayer()
	{
		std::string name;
		m_tmjReader.field("name", name);

		std::string encoding, compression;
		m_tmjReader.field("encoding", encoding);
		m_tmjReader.field("compression", compression);

		if (encoding != "base64" || !compression.empty()) {
			ANKER_ERROR("{}: Not using base64 (uncompressed)", m_tmjIdentifier);
			return FormatError;
		}

		std::string data;
		if (!m_tmjReader.field("data", data)) {
			ANKER_ERROR("{}: Missing data field (chunked maps are not supported yet)", m_tmjIdentifier);
			return FormatError;
		}
		data = decodeBase64(data);

		uint32_t width = 0, height = 0;
		m_tmjReader.field("width", width);
		m_tmjReader.field("height", height);

		// We create one MapLayer for every Tileset here, along with a vertex
		// storage for each of them.
		using Vertices = std::vector<MapRenderer::Vertex>;
		std::vector<MapLayer> mapLayers(m_tilesets.size());
		std::vector<Vertices> verticesForMapLayers(m_tilesets.size());

		for (auto [i, layer] : iter::enumerate(mapLayers)) {
			layer = {
			    .name = fmt::format("{} (Tileset {})", name, i),
			    .color = calcColor(),
			    .parallax = calcParallax(),
			    .texture = m_tilesets[i].texture,
			};
		}

		// We re-interpret the data buffer as buffer of TileIds.
		std::span<const TileId> tiles = std::span(reinterpret_cast<TileId*>(data.data()), //
		                                          data.size() / sizeof(TileId));
		if (tiles.size() != width * height) {
			ANKER_ERROR("{}: data length does not match layer dimensions tileCount={} width={} height={}",
			            m_tmjIdentifier, tiles.size(), width, height);
			return FormatError;
		}

		for (auto [tileIndex, tile] : iter::enumerate(tiles)) {
			if (tile == EmptyTile) {
				continue;
			}

			// The tile number consists of a global id and flip bits.
			const TileId gid = tile & ~FlipMask;

			// From the global id, we can determine the Tileset used for this
			// specific tile.
			const uint32_t tilesetIndex = findTilesetIndex(gid);

			const Rect2 pos = Rect2(Vec2(1), {float(tileIndex % width), -float(tileIndex / width) - 1.0f});

			const Rect2 texCoordinates = m_tilesets[tilesetIndex].textureCoordinates(gid);
			Vec2 uvTopLeft = texCoordinates.topLeft();
			Vec2 uvTopRight = texCoordinates.topRight();
			Vec2 uvBottomLeft = texCoordinates.bottomLeft();
			Vec2 uvBottomRight = texCoordinates.bottomRight();

			if (tile & FlipVertical) {
				std::swap(uvTopLeft, uvBottomLeft);
				std::swap(uvTopRight, uvBottomRight);
			}
			if (tile & FlipHorizontal) {
				std::swap(uvTopLeft, uvTopRight);
				std::swap(uvBottomLeft, uvBottomRight);
			}
			if (tile & FlipDiagonal) {
				std::swap(uvTopRight, uvBottomLeft);
			}

			verticesForMapLayers[tilesetIndex].insert(
			    verticesForMapLayers[tilesetIndex].end(),
			    {
			        MapRenderer::Vertex{.position = pos.topLeftWorld(), .uv = uvTopLeft},
			        MapRenderer::Vertex{.position = pos.bottomLeftWorld(), .uv = uvBottomLeft},
			        MapRenderer::Vertex{.position = pos.topRightWorld(), .uv = uvTopRight},
			        MapRenderer::Vertex{.position = pos.topRightWorld(), .uv = uvTopRight},
			        MapRenderer::Vertex{.position = pos.bottomLeftWorld(), .uv = uvBottomLeft},
			        MapRenderer::Vertex{.position = pos.bottomRightWorld(), .uv = uvBottomRight},
			    });
		}

		for (auto [layer, vertices] : iter::zip(mapLayers, verticesForMapLayers)) {
			if (vertices.empty()) {
				continue;
			}

			layer.vertexBuffer.info = {
			    .name = "MapLayer Vertex Buffer " + layer.name,
			    .bindFlags = GpuBindFlag::VertexBuffer,
			};
			ANKER_TRY(m_assetCache.renderDevice().createBuffer(layer.vertexBuffer, vertices));

			auto entity = m_scene.createEntity(layer.name);
			entity.emplace<SceneNode>(Transform2D{}, m_layerSceneNode);
			entity.emplace<MapLayer>(std::move(layer));
		}

		return Ok;
	}

	Status loadObjectLayer()
	{
		m_tmjReader.forEach("objects", [&](uint32_t) {
			std::string objectName;
			m_tmjReader.field("name", objectName);

			Transform2D transform;
			m_tmjReader.field("rotation", transform.rotation);
			transform.rotation = -transform.rotation * Deg2Rad;
			m_tmjReader.field("width", transform.scale.x);
			m_tmjReader.field("height", transform.scale.y);
			transform.scale /= 256.0f; // pixel -> meter

			// Rotation pivot in Tiled is the bottom left corner of an object.
			// However, our rotation pivot is the object's center.
			transform.position = transform.scale / 2.0f;
			transform.position.rotate(transform.rotation);

			Vec2 offset;
			m_tmjReader.field("x", offset.x);
			m_tmjReader.field("y", offset.y);
			transform.position += convertCoordinates(offset);

			TileId tile = 0;
			m_tmjReader.field("gid", tile);
			ANKER_ASSERT(tile != 0); // TODO

			// The tile number consists of a global id and flip bits.
			const TileId gid = tile & ~FlipMask;

			if (tile & FlipHorizontal) {
				transform.scale.x *= -1.0f;
			}
			if (tile & FlipVertical) {
				transform.scale.y *= -1.0f;
			}

			const auto& tileset = m_tilesets[findTilesetIndex(gid)];

			auto entity = m_scene.createEntity(objectName);
			entity.emplace<SceneNode>(transform, m_layerSceneNode);
			entity.emplace<Sprite>(Sprite{
			    .color = calcColor(),
			    .parallax = calcParallax(),
			    .offset = {-0.5f, -0.5f},
			    .pixelToMeter = 256.0f, // TODO
			    .texture = tileset.texture,
			    .textureRect = tileset.textureCoordinates(gid),
			});
		});

		return Ok;
	}

	Status loadCollisionLayer()
	{
		m_tmjReader.forEach("objects", [&](uint32_t) {
			if (m_tmjReader.hasKey("ellipse")) {
				ANKER_WARN("{}: Ellipse collider not supported", m_tmjIdentifier);
				return;
			}
			if (m_tmjReader.hasKey("point")) {
				ANKER_WARN("{}: Point collider not supported", m_tmjIdentifier);
				return;
			}
			if (float rotation; m_tmjReader.field("rotation", rotation) && rotation != 0) {
				ANKER_WARN("{}: Collider rotation not supported", m_tmjIdentifier);
				return;
			}

			std::vector<b2Vec2> vertices;
			if (m_tmjReader.hasKey("polygon")) {
				m_tmjReader.forEach("polygon", [&](uint32_t) {
					Vec2 vertex;
					m_tmjReader.field("x", vertex.x);
					m_tmjReader.field("y", vertex.y);
					vertices.push_back(convertCoordinates(vertex));
				});
			} else {
				Vec2 boxSize;
				m_tmjReader.field("width", boxSize.x);
				m_tmjReader.field("height", boxSize.y);
				vertices.push_back(convertCoordinates({0, 0}));
				vertices.push_back(convertCoordinates({0, boxSize.y}));
				vertices.push_back(convertCoordinates({boxSize.x, boxSize.y}));
				vertices.push_back(convertCoordinates({boxSize.x, 0}));
			}

			auto entity = m_scene.createEntity("Collider");

			Transform2D transform;
			m_tmjReader.field("x", transform.position.x);
			m_tmjReader.field("y", transform.position.y);
			transform.position = convertCoordinates(transform.position);

			entity.emplace<SceneNode>(transform, m_layerSceneNode);

			auto* physicsBody = entity.emplace<PhysicsBody>().body;
			physicsBody->SetType(b2_staticBody);

			b2ChainShape chain;
			chain.CreateLoop(vertices.data(), int32(vertices.size()));
			physicsBody->CreateFixture(&chain, 0);
		});

		return Ok;
	}

	// Given a global id (without flip bits), this function returns the index of
	// the corresponding Tileset.
	uint32_t findTilesetIndex(uint32_t gid) const
	{
		auto iter = std::ranges::find_if(m_tilesets, [=](auto& tileset) { return gid >= tileset.firstTileId; });
		return uint32_t(std::distance(m_tilesets.begin(), iter));
	}

	Vec2 convertCoordinates(Vec2 v) const
	{
		v /= 256.0f;  // pixel -> meter
		v.y *= -1.0f; // flip Y axis
		return v;
	}

	Vec4 calcColor() const
	{
		return std::accumulate(m_colorStack.begin(), m_colorStack.end(), Vec4(1), std::multiplies());
	}

	Vec2 calcParallax() const
	{
		return std::accumulate(m_parallaxStack.begin(), m_parallaxStack.end(), Vec2(1), std::multiplies());
	}

	Scene& m_scene;
	AssetCache& m_assetCache;
	std::string_view m_tmjIdentifier;

	JsonReader m_tmjReader;

	std::vector<Tileset> m_tilesets;

	// While traversing the tree of layers we build a corresponding scene graph.
	SceneNode* m_layerSceneNode = nullptr;

	// While traversing the tree of layers, certain properties are passed on
	// from parent to child. These are tracked here as explicit stacks.
	std::vector<Vec2> m_parallaxStack;
	std::vector<Vec4> m_colorStack;
};

Status loadMap(Scene& scene, std::string_view identifier, AssetCache& assetCache)
{
	ANKER_PROFILE_ZONE_T(identifier);

	TmjLoader loader(scene, assetCache);
	return loader.load(identifier);
}

} // namespace Anker
