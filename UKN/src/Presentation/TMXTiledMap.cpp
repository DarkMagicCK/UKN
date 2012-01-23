//
//  TMXTiledMap.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/TMXTiledMap.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Asset.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/OperationForEach.h"
#include "UKN/Texture.h"
#include "UKN/Color.h"
#include "UKN/ConfigParser.h"
#include "UKN/Logger.h"
#include "UKN/Base64.h"
#include "UKN/ZipUtil.h"
#include "UKN/Context.h"
#include "UKN/RenderBuffer.h"

namespace ukn {
    
    namespace tmx {
        
        Map::Map():
        mMapWidth(0),
        mMapHeight(0),
        mTileWidth(0),
        mTileHeight(0) {
            mMapRenderer = Context::Instance().getGraphicFactory().createSpriteBatch();
        }
        
        Map::Map(const String& map_file):
        mMapWidth(0),
        mMapHeight(0),
        mTileWidth(0),
        mTileHeight(0) {
            mMapRenderer = Context::Instance().getGraphicFactory().createSpriteBatch();
            
            deserialize(AssetManager::Instance().load<ConfigParser>(map_file));
        }
        
        Map::~Map() {
            
        }
        
        const Map::TileSetList& Map::getTileSets() const {
            return mTileSets;
        }
        
        const Map::LayerList& Map::getLayers() const {
            return mLayers;
        }
        
        SpriteBatchPtr Map::getMapRenderer() const {
            return mMapRenderer;
        }

        void Map::setMapRenderer(SpriteBatchPtr renderer) {
            mMapRenderer = renderer;
        }
        
        MapOrientation Map::getOrientation() const {
            return mOrientation;
        }
        
        uint32 Map::getMapWidth() const {
            return mMapWidth;
        }
        
        uint32 Map::getMapHeight() const {
            return mMapHeight;
        }
        
        int32 Map::getTileWidth() const {
            return mTileWidth;
        }
        
        int32 Map::getTileHeight() const {
            return mTileHeight;
        }
        
        void Map::setMapPosition(const Vector2& pos) {
            mMapPosition.x = (int32)(pos.x / mTileWidth);
            mMapPosition.y = (int32)(pos.y / mTileHeight);
            mPosition = pos;
        }
        
        Vector2 Map::getMapPosition() const {
            return mMapPosition;
        }
        
        void Map::setMapViewSize(const Vector2& size) {
            mMapViewSize = size;
        }
        
        Vector2 Map::getMapViewSize() const {
            return mMapViewSize;
        }
        
        bool Map::serialize(const ConfigParserPtr& config) {
            return false;
        }
        
        static void build_tileset_tiles(TileSet& tileset, uint32 tileset_id) {
            if(tileset.image) {
                int32 wcount = tileset.image->getWidth() / tileset.tile_width;
                int32 hcount = tileset.image->getHeight() / tileset.tile_height;
                
                tileset.tiles.resize(wcount * hcount);
                //ukn_logged_assert(tileset.tiles, "error allocing tileset tiles");
                
                for(int32 i = 0; i < wcount * hcount; ++i) {
                    Tile& tile = tileset.tiles[i];
                    
                    tile.tile_id = i;
                    
                    int32 x = tileset.spacing + (i % wcount) * (tileset.tile_width + tileset.spacing);
                    int32 y = tileset.spacing + (i / wcount) * (tileset.tile_height + tileset.spacing);
                    tile.tile_texture_rect = Rectangle(x + tileset.margin,
                                                       y + tileset.margin,
                                                       x + tileset.tile_width - tileset.margin,
                                                       y + tileset.tile_height - tileset.margin);
                    
                    tile.tileset_id = tileset_id;
                }
            }
        }
        
        static void deserialize_tile_set(TileSet& ts, uint32 ts_id, ConfigParserPtr config) {
            ts.name = config->getString("name");
            ts.tile_width = config->getInt("tilewidth");
            ts.tile_height = config->getInt("tileheight");
            ts.margin = config->getInt("margin");
            ts.spacing = config->getInt("spacing");
            
            if(config->toNode("tileoffset")) {
                ts.tile_offset_x = config->getInt("x");
                ts.tile_offset_y = config->getInt("y");
                config->toParent();
            }
            if(config->toNode("image")) {
                ts.image = AssetManager::Instance().load<Texture>(get_file_path(config->getName()) + string_to_wstring(config->getString("source")));
                config->toParent();
            }
            
            build_tileset_tiles(ts, ts_id);
            
            // parser tile properties
            if(config->toFirstChild()) {
                do {
                    if(config->getCurrentNodeName() == "tile") {
                        int32 tileid = config->getInt("id");
                        ukn_assert(tileid < ts.tiles.size());
                        
                        Tile* tile = ts.tileAt(tileid);
                        if(tile) {
                            if(config->toNode("properties")) {
                                if(config->toFirstChild()) {
                                    do {
                                        if(config->hasAttribute("name")) 
                                            tile->property.properties.push_back(Property(config->getString("name"),
                                                                                         config->getString("value")));
                                        
                                    } while(config->toNextChild());
                                
                                    // <properties>
                                    config->toParent();
                                }
                                
                                // <tile>
                                config->toParent();
                            }
                        }
                    }
                } while(config->toNextChild());
                
                // <tileset>
                config->toParent();
            }
        }
        
        const int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        const int FLIPPED_VERTICALLY_FLAG   = 0x40000000;
        const int FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
        
        bool Map::deserialize(const ConfigParserPtr& config) {
            if(config->toNode("map")) {
                ukn_string orientation = config->getString("orientation", "orthogonall");
                if(orientation == "orthogonal")
                    mOrientation = MO_Orthogonal;
                else
                    mOrientation = MO_Isometric;
                
                mMapWidth = config->getInt("width");
                mMapHeight = config->getInt("height");
                mTileWidth = config->getInt("tilewidth");
                mTileHeight = config->getInt("tileheight");
                
                if(config->toFirstChild()) {
                    do {
                        ukn_string node_type = config->getCurrentNodeName();
                        if(node_type == "tileset") {                            
                            ukn_string source = config->getString("source");
                            
                            mTileSets.push_back(TileSet());
                            
                            TileSet& ts = mTileSets.back();
                            ts.first_grid = config->getInt("firstgid");

                            if(source.empty()) {
                                deserialize_tile_set(ts, 
                                                     (uint32)mTileSets.size() - 1, 
                                                     config);
                            } else {
                                // external tile set file
                                ConfigParserPtr extern_config = AssetManager::Instance().load<ConfigParser>(get_file_path(config->getName()) + string_to_wstring(source));
                                if(extern_config->toNode("tileset")) {
                                    deserialize_tile_set(ts, 
                                                         (uint32)mTileSets.size() - 1, 
                                                         extern_config);
                                } else {
                                    log_error("ukn::tmx::Map::deserialize: invalid external config file");
                                    return false;
                                }
                            }

                                                        
                        } else if(node_type == "layer") {
                            ukn_assert(!mTileSets.empty());
                            
                            mLayers.push_back(new Layer());
                            
                            Layer& layer = *mLayers.back();
                            layer.name = config->getString("name");
                            layer.width = config->getInt("width");
                            layer.height = config->getInt("height");
                            layer.x = config->getInt("x");
                            layer.y = config->getInt("y");
                            layer.visible = config->getBool("visible", true);
                            layer.opacity = config->getFloat("opacity", 1.0f);
                            
                            if(config->toNode("data")) {
                                ukn_string encoding = config->getString("encoding");
                                ukn_string compression = config->getString("compression");
                                
                                if(encoding == "base64") {
                                    ukn_string str_data = config->getString(ukn_string());
                                    ukn_assert(!str_data.empty());

                                    ukn_string::iterator begin = str_data.begin();
                                    while(*begin == '\n' || *begin == ' ')
                                        begin++;
                                    ukn_string::iterator end = str_data.end();
                                    str_data.erase(str_data.begin(), begin);
                                    
                                    // base64 decode
                                    Array<uint8> data(base64_decode(/* node value */
                                                                    str_data
                                                                    ));
                                    ukn_assert(data.size() != 0);
                                                                    
                                    // zlib/gzip decompress
                                    Array<uint8> dp_data(zlib_decompress(data.begin(), (uint32)data.size()));
                                    
                                    ukn_assert(dp_data.size() == layer.width * layer.height * 4);

                                    uint32 tile_index = 0;
                                    uint8* raw_data = (uint8*)dp_data.begin();
                                    
                                    layer.tiles.resize(layer.width * layer.height);
                                    
                                    layer.parent = this;
                                    
                                    for(int32 j = 0; j < layer.height; ++j) {
                                        for(int32 i = 0; i < layer.width; ++i) {
                                            // for tmx format wiki
                                            uint32 global_tile_id = raw_data[tile_index] |
                                                                    raw_data[tile_index + 1] << 8 |
                                                                    raw_data[tile_index + 2] << 16 |
                                                                    raw_data[tile_index + 3] << 24;
                                            tile_index += 4;
                                            
                                            // Read out the flags
                                            /* Bit 32 is used for storing whether the tile is horizontally flipped and bit 31 is used for the vertically flipped tiles. And since Tiled Qt 0.8.0, bit 30 means whether the tile is flipped (anti) diagonally, enabling tile rotation
                                             */
                                            bool flipped_horizontally = global_tile_id & FLIPPED_HORIZONTALLY_FLAG;
                                            bool flipped_vertically = global_tile_id & FLIPPED_VERTICALLY_FLAG;                                          
                                            bool flipped_diagonally = global_tile_id & FLIPPED_DIAGONALLY_FLAG;
                                            
                                            global_tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG |
                                                                FLIPPED_VERTICALLY_FLAG |
                                                                FLIPPED_DIAGONALLY_FLAG);
                                            
                                            ukn_assert(global_tile_id < mTileSets.back().first_grid + mTileSets.back().tiles.size());
                                            
                                            // Resolve the tile
                                            if(global_tile_id == 0) {
                                                layer.tiles[i + j * layer.height].tile_id = -1;
                                            } else {
                                                for (int32 ti = (int32)mTileSets.size() - 1; ti >= 0; --ti) {
                                                    TileSet& tileset = mTileSets[ti];
                                                    if (tileset.first_grid <= global_tile_id) {
                                                        Tile& tile = layer.tiles[i + j * layer.height];
                                                        
                                                        tile = tileset.tiles[global_tile_id - tileset.first_grid];

                                                        tile.flipped_diagonally = flipped_diagonally;
                                                        tile.flipped_horizontally = flipped_horizontally;
                                                        tile.flipped_vertically = flipped_vertically;
                                                        
                                                        tile.tile_bounding_rect = Rectangle((i % layer.width) * tileset.tile_width,
                                                                                            (i / layer.width) * tileset.tile_height,
                                                                                            (i % layer.width) * tileset.tile_width + tileset.tile_width,
                                                                                            (i / layer.width) * tileset.tile_height + tileset.tile_height);

                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                                                    
                                } else if(encoding == "csv") {
                                    ukn_logged_assert(false, "ukn::tmx::Map: csv layer data encoding not supported");
                                }

                                // <layer>
                                config->toParent();
                            }
                        }
                        
                    } while(config->toNextChild());
                    
                    mMapViewSize.x = mMapWidth;
                    mMapViewSize.y = mMapHeight;
                    
                    // <map>
                    config->toParent();
                    return true;
                }
            }
            return false;
        }
        
        Tile& Layer::getTileAt(const Vector2& pos) {
            return tiles[pos.x / parent->getTileWidth() + pos.y / parent->getTileHeight() * parent->getMapWidth()];
        }
        
        Tile& Map::getTileAt(uint32 layer_index, const Vector2& pos) {
            return mLayers[layer_index]->getTileAt(pos);
        }
        
        const String& Map::getName() const {
            return mName;
        }
        
        Box Map::getBound() const {
            return Box(Vector3(0.f, 0.f, 0.f),
                       Vector3(mMapWidth * mTileWidth,
                               mMapHeight * mTileHeight,
                               0.f));
        }
        
        RenderBufferPtr Map::getRenderBuffer() const {
            return mMapRenderer->getRenderBuffer();
        }
        
        void Map::onRenderBegin() {
            mMapRenderer->onRenderBegin();
        }
        
        void Map::onRenderEnd() {
            mMapRenderer->onRenderEnd();
        }
        
        void Map::orthogonalRender() {
            UKN_ENUMERABLE_FOREACH(SharedPtr<Layer>, layer_ptr, mLayers) {
                Layer& layer = *layer_ptr;
                if(layer.visible) {
                    mMapRenderer->startBatch();
                    
                    for(int32 j = mMapPosition.y; j <= mMapPosition.y + mMapViewSize.y + 1; ++j) {
                        for(int32 i = mMapPosition.x; i <= mMapPosition.x + mMapViewSize.x + 1; ++i) {
                            Tile& tile = layer.tiles[i + j * layer.height];
                            if(tile.tile_id != -1) {
                                TileSet& ts = mTileSets[tile.tileset_id];
                                
                                float x = i * (mTileWidth - ts.margin * 2) + ts.tile_offset_x + layer.x - mPosition.x;
                                float y = j * (mTileHeight - ts.margin * 2) + ts.tile_offset_y + layer.y - mPosition.y + mTileHeight;
                                
                                mMapRenderer->draw(ts.image,
                                                   Vector2(x,
                                                           y),
                                                   tile.tile_texture_rect,
                                                   Vector2(0,
                                                           ts.tile_height),
                                                   0.f,
                                                   Vector2(1.f, 1.f),
                                                   color::White * layer.opacity);
                            }
                            
                        }
                    }
                    
                    mMapRenderer->endBatch();
                }
            }
        }
        
        void Map::isometricRender() {
            
        }
        
        void Map::render() {
            mMapRenderer->begin();
                              
            if(mOrientation == MO_Orthogonal)
                orthogonalRender();
            else
                isometricRender();
                       
            mMapRenderer->end();
        }
        
    } // namespace tmx
    
} // namespace ukn