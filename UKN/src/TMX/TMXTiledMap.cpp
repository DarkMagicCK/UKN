//
//  TMXTiledMap.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "mist/Operations.h"
#include "mist/Color.h"
#include "mist/ConfigParser.h"
#include "mist/Logger.h"
#include "mist/Base64.h"
#include "mist/ZipUtil.h"
#include "mist/Convert.h"

#include "UKN/tmx/TMXTiledMap.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Asset.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Texture.h"
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
        
        Map::Map(const UknString& map_file):
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
        
        void Map::setPosition(const Vector2& pos) {
            mPosition = pos;
        }
        
        const Vector2& Map::getPosition() const {
            return mPosition;
        }
        
        bool Map::serialize(const ConfigParserPtr& config) {
            return false;
        }
        
        void Map::setMapViewRect(const mist::Rectangle& vr) {
            mViewRect = vr;
        }
        
        const mist::Rectangle& Map::getMapViewRect() const {
            return mViewRect;
        }
        
        void Map::parseProperties(PropertyContainer& cont, const ConfigParserPtr& config) {
            if(config->toNode(L"properties")) {
                if(config->toFirstChild()) {
                    do {
                        if(config->hasAttribute(L"name"))
                            cont.properties.push_back(Property(config->getString(L"name"),
                                          config->getString(L"value")));
                        
                    } while(config->toNextChild());
                    
                    // <properties>
                    config->toParent();
                }
                
                // 
                config->toParent();
            }
        }
        
        static void build_tileset_tiles(TileSet& tileset, uint32 tileset_id) {
            if(tileset.image) {
                int32 wcount = tileset.image->width() / tileset.tile_width;
                int32 hcount = tileset.image->height() / tileset.tile_height;
                
                tileset.tiles.resize(wcount * hcount);
                //ukn_logged_assert(tileset.tiles, "error allocing tileset tiles");
                
                for(int32 i = 0; i < wcount * hcount; ++i) {
                    Tile& tile = tileset.tiles[i];
                    
                    tile.tile_id = i;
                    
                    int32 x = (i % wcount) * (tileset.tile_width + tileset.spacing);
                    int32 y = (i / wcount) * (tileset.tile_height + tileset.spacing);
                    tile.tile_texture_rect = mist::Rectangle(real(x + tileset.margin),
															 real(y + tileset.margin),
															 real(x + tileset.tile_width + tileset.margin),
															 real(y + tileset.tile_height + tileset.margin));
                    
                    tile.tileset_id = tileset_id;
                }
            }
        }
        
        void Map::deserialize_tile_set(TileSet& ts, uint32 ts_id, const ConfigParserPtr& config) {
            ts.name =config->getString(L"name");
            ts.tile_width = config->getInt(L"tilewidth");
            ts.tile_height = config->getInt(L"tileheight");
            ts.margin = config->getInt(L"margin");
            ts.spacing = config->getInt(L"spacing");
            
            if(config->toNode(L"tileoffset")) {
                ts.tile_offset_x = config->getInt(L"x");
                ts.tile_offset_y = config->getInt(L"y");
                config->toParent();
            }
            if(config->toNode(L"image")) {
                ts.image = AssetManager::Instance().load<Texture>(string::GetFilePath(config->getName()) + config->getString(L"source"));
                config->toParent();
            }
            parseProperties(ts.property, config);
            
            build_tileset_tiles(ts, ts_id);
            
            // parser tile properties
            if(config->toFirstChild()) {
                do {
                    if(config->getCurrentNodeName() == L"tile") {
                        int32 tileid = config->getInt(L"id");
                        mist_assert(tileid < ts.tiles.size());
                        
                        Tile* tile = ts.tileAt(tileid);
                        if(tile) {
                            parseProperties(tile->property, config);
                        }
                    }
                } while(config->toNextChild());
                
                // <tileset>
                config->toParent();
            }
        }
        
        
        void Map::parseLayer(const ConfigParserPtr& config) {
            
            static const int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
            static const int FLIPPED_VERTICALLY_FLAG   = 0x40000000;
            static const int FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
            
            mist_assert(!mTileSets.empty());
            
            mLayers.push_back(MakeSharedPtr<Layer>());
            
            Layer& layer = *mLayers.back();
            layer.name = config->getString(L"name");
            layer.width = config->getInt(L"width");
            layer.height = config->getInt(L"height");
            layer.x = config->getInt(L"x");
            layer.y = config->getInt(L"y");
            layer.visible = config->getBool(L"visible", true);
            layer.opacity = config->getFloat(L"opacity", 1.0f);
            
            if(config->toNode(L"data")) {
                UknString encoding = config->getString(L"encoding");
                UknString compression = config->getString(L"compression");
                
                if(encoding == L"base64") {
                    UknString str_data = config->getString(UknString());
                    mist_assert(!str_data.empty());
                    
                    UknString::iterator begin = str_data.begin();
                    while(*begin == '\n' || *begin == ' ')
                        begin++;
                    str_data.erase(str_data.begin(), begin);
                    
                    // base64 decode
                    Array<uint8> data(base64_decode(/* node value */
                                                    string::WStringToStringFast(str_data)
                                                    ));
                    mist_assert(data.size() != 0);
                    
                    // zlib/gzip decompress
                    std::vector<uint8> dp_data = zlib_decompress(&data.front(), (uint32)data.size());
                    
                    mist_assert(dp_data.size() == layer.width * layer.height * 4);
                    
                    uint32 tile_index = 0;
                    uint8* raw_data = (uint8*)&dp_data.front();
                    
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
                            bool flipped_horizontally = (bool)(global_tile_id & FLIPPED_HORIZONTALLY_FLAG);
                            bool flipped_vertically = (bool)(global_tile_id & FLIPPED_VERTICALLY_FLAG);                                          
                            bool flipped_diagonally = (bool)(global_tile_id & FLIPPED_DIAGONALLY_FLAG);
                            
                            global_tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG |
                                                FLIPPED_VERTICALLY_FLAG |
                                                FLIPPED_DIAGONALLY_FLAG);
                            
                            mist_assert(global_tile_id < mTileSets.back().first_grid + mTileSets.back().tiles.size());
                            
                            // Resolve the tile
                            if(global_tile_id == 0) {
                                layer.tiles[i + j * layer.height].tile_id = -1;
                            } else {
                                Tile* g_tile = getTileWithGid(global_tile_id);
                                if(g_tile) {
                                    Tile& tile = layer.tiles[i + j * layer.width];
                                    
                                    tile = *g_tile;
                                    
                                    tile.flipped_diagonally = flipped_diagonally;
                                    tile.flipped_horizontally = flipped_horizontally;
                                    tile.flipped_vertically = flipped_vertically;
                                    
                                    TileSet& tileset = mTileSets[tile.tileset_id];
                                    tile.tile_bounding_rect = mist::Rectangle(real((i % layer.width) * tileset.tile_width),
																			  real((i / layer.width) * tileset.tile_height),
																			  real((i % layer.width) * tileset.tile_width + tileset.tile_width),
																			  real((i / layer.width) * tileset.tile_height + tileset.tile_height));
                                } else
                                    log_error(L"ukn::tmx::Map::parseLayer: invalid tile with gid " + Convert::ToString(global_tile_id));
                            }
                        }
                    }
                    
                } else if(encoding == L"csv") {
                    StringTokenlizer tiles(config->getString(UknString()),
                                           L",");
                    mist_assert(tiles.size() == layer.width * layer.height);
                    for(int32 j = 0; j < layer.height; ++j) {
                        for(int32 i = 0; i < layer.width; ++i) {
                            Tile* g_tile = getTileWithGid(Convert::ToInt32(tiles[j * layer.width + i]));
                            if(g_tile) {
                                Tile& tile = layer.tiles[i + j * layer.width];
                                
                                tile = *g_tile;
                                
                                TileSet& tileset = mTileSets[tile.tileset_id];
                                tile.tile_bounding_rect = mist::Rectangle((float)(i % layer.width) * tileset.tile_width,
																			(float)(i / layer.width) * tileset.tile_height,
																			(float)(i % layer.width) * tileset.tile_width + tileset.tile_width,
																			(float)(i / layer.width) * tileset.tile_height + tileset.tile_height);
                            } else
                                log_error(L"ukn::tmx::Map::parseLayer: invalid tile with gid " + tiles[j * layer.width + i]);
                        }
                    }
                }
                
                // <layer>
                config->toParent();
            }
            
            parseProperties(layer.property, config);
        }
        
        Tile* Map::getTileWithGid(int32 gid) {
            for (int32 ti = (int32)mTileSets.size() - 1; ti >= 0; --ti) {
                TileSet& tileset = mTileSets[ti];
                if (tileset.first_grid <= gid) {
                    return &tileset.tiles[gid - tileset.first_grid];
                }
            }
            return 0;
        }
        
        void Map::parseTileset(const ConfigParserPtr& config) {
            UknString source = config->getString(L"source");
            
            mTileSets.push_back(TileSet());
            
            TileSet& ts = mTileSets.back();
            ts.first_grid = config->getInt(L"firstgid");
            
            if(source.empty()) {
                deserialize_tile_set(ts, 
                                     (uint32)mTileSets.size() - 1, 
                                     config);
            } else {
                // external tile set file
                ConfigParserPtr extern_config = AssetManager::Instance().load<ConfigParser>(string::GetFilePath(config->getName()) + source);
                if(extern_config->toNode(L"tileset")) {
                    deserialize_tile_set(ts, 
                                         (uint32)mTileSets.size() - 1, 
                                         extern_config);
                } else {
                    log_error("ukn::tmx::Map::deserialize: invalid external config file");
                }
            }
        }
        
        void Map::parseObjectGroup(const ConfigParserPtr& config) {
            mLayers.push_back(MakeSharedPtr<ObjectGroup>());
            
            ObjectGroup& obj_group = *static_cast<ObjectGroup*>(mLayers.back().get());
            
            // layer properties
            obj_group.name = config->getString(L"name");
            obj_group.width = config->getInt(L"width");
            obj_group.height = config->getInt(L"height");
            obj_group.x = config->getInt(L"x");
            obj_group.y = config->getInt(L"y");
            obj_group.visible = config->getBool(L"visible", true);
            obj_group.opacity = config->getFloat(L"opacity", 1.0f);
            
            parseProperties(obj_group.property, config);
            
            if(config->toFirstChild()) {
                do {
                    UknString node_type = config->getCurrentNodeName();
                    if(node_type == L"object") {
                        Object obj;
                        
                        obj.name = config->getString(L"name");
                        obj.type = config->getString(L"type");
                        
                        obj.x = config->getInt(L"x");
                        obj.y = config->getInt(L"y");
                        obj.width = config->getInt(L"width");
                        obj.height = config->getInt(L"height");
                        
                        obj.gid = config->getInt(L"gid", -1);
                        if(obj.gid != -1) {
                            obj.tile = *getTileWithGid(obj.gid);
                        } else {
                            obj.image = AssetManager::Instance().load<Texture>(string::GetFilePath(config->getName()) + config->getString(L"image"));
                        }
                        
                        parseProperties(obj.property, config);
                        
                        if(config->toNode(L"polygon")) {
                            mist_assert_l(false, "ukn::tmx::Map: polygon object not supported");
                            config->toParent();
                        } else if(config->toNode(L"polyline")) {
                            mist_assert_l(false, "ukn::tmx::Map: polyline object not supported");
                            config->toParent();
                        }
                    }
                    
                } while(config->toNextChild());
            }
        }
        
        bool Map::deserialize(const ConfigParserPtr& config) {
            if(config->toNode(L"map")) {
                UknString orientation = config->getString(L"orientation", L"orthogonall");
                if(orientation == L"orthogonal")
                    mOrientation = MO_Orthogonal;
                else
                    mOrientation = MO_Isometric;
                
                mMapWidth = config->getInt(L"width");
                mMapHeight = config->getInt(L"height");
                mTileWidth = config->getInt(L"tilewidth");
                mTileHeight = config->getInt(L"tileheight");
                
                if(config->toFirstChild()) {
                    do {
                        UknString node_type = config->getCurrentNodeName();
                        if(node_type == L"tileset") {
                            parseTileset(config);
    
                        } else if(node_type == L"layer") {
                            parseLayer(config);
    
                        } else if(node_type == L"objectgroup") {
                            parseObjectGroup(config);
                        }
                        
                    } while(config->toNextChild());
                    
                    mViewRect.set(Vector2(0, 0), 
                                  Vector2(float(mMapWidth * mTileWidth),
                                          float(mMapHeight * mTileHeight)));
                    
                    // <map>
                    config->toParent();
                    return true;
                }
            }
            return false;
        }
        
        Tile& Layer::getTileAt(const Vector2& pos) {
            return tiles[uint32(pos.x() / parent->getTileWidth() + pos.y() / parent->getTileHeight() * parent->getMapWidth())];
        }
        
        Tile& Map::getTileAt(uint32 layer_index, const Vector2& pos) {
            return mLayers[layer_index]->getTileAt(pos);
        }
        
        const UknString& Map::getName() const {
            return mName;
        }
        
        Box Map::getBound() const {
            return Box(Vector3(0.f, 0.f, 0.f),
                       Vector3(real(mMapWidth * mTileWidth),
                               real(mMapHeight * mTileHeight),
                               0.f));
        }
        
        RenderBufferPtr Map::getRenderBuffer() const {
            return mMapRenderer->getRenderBuffer();
        }
        
        void Map::onRenderBegin() {
        }
        
        void Map::onRenderEnd() {
        }
        
        void Map::orthogonalRender() {
            for(LayerList::iterator it = mLayers.begin(), end = mLayers.end();
                it != end;
                ++it) {
                Layer& layer = **it;
                if(layer.visible) {
                    mMapRenderer->startBatch();
                    
                    int32 startx = 0;
                    int32 starty = 0;
                    
                    int32 endx = layer.width;
                    int32 endy = layer.height;
                    
                    startx = int32(MIST_MAX(mViewRect.left() / mTileWidth, 0));
                    starty = int32(MIST_MAX(mViewRect.top() / mTileHeight, 0));
                    
                    endx = int32(MIST_MIN(mViewRect.right() / mTileWidth + 1, endx));
                    endy = int32(MIST_MIN(mViewRect.bottom() / mTileHeight + 1, endy));
                    
                    for(int32 y = starty; y < endy; ++y) {
                        for(int32 x = startx; x < endx; ++x) {
                            Tile& tile = layer.tiles[x + y * layer.width];
                            TileSet& ts = mTileSets[tile.tileset_id];

                            if(tile.tile_id != -1) {
                                float dx = (float)ts.tile_offset_x + x * mTileWidth;
                                float dy = (float)ts.tile_offset_y + (y + 1) * mTileHeight - ts.tile_height;
                                
                                if(tile.flipped_diagonally) {
                                    dy += ts.tile_height - ts.tile_width;
                                }
                                if(tile.flipped_horizontally) {
                                    dx += tile.flipped_diagonally ? ts.tile_height : ts.tile_width;
                                }
                                if(tile.flipped_vertically) {
                                    dy += tile.flipped_diagonally ? ts.tile_width : ts.tile_height;
                                }
                                
                                mMapRenderer->draw(ts.image,
                                                   Vector2(dx, dy),
                                                   tile.tile_texture_rect,
                                                   Vector2(0,
                                                           0),
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
        
        float2 Map::pixelToTileCoords(float x, float y) const {
            if(mOrientation == MO_Isometric) {
                float ratio = (float)mTileWidth / mTileHeight;
                
                x -= mMapHeight * mTileWidth / 2;
                float mx = y + (x / ratio);
                float my = y - (x / ratio);
                
                return float2(mx / mTileHeight,
                              my / mTileHeight);
            } else {
                return float2(x / mTileWidth,
                              y / mTileHeight);
            }
        }
        
        float2 Map::tileToPixelCoords(float x, float y) const {
            if(mOrientation == MO_Isometric) {
                int origx = mMapHeight * mTileWidth / 2;
                return float2((x - y) * mTileWidth / 2 + origx,
                              (x + y) * mTileHeight / 2);
            } else {
                return float2(x * mTileWidth,
                              y * mTileHeight);
            }
        }
                
        void Map::isometricRender() {
            for(LayerList::iterator it = mLayers.begin(), end = mLayers.end();
                it != end;
                ++it) {
                Layer& layer = **it;
                if(layer.visible) {
                    mMapRenderer->startBatch();
                    
                    mist::Rectangle vr = mViewRect;
                    vr.set(mViewRect.x(),
                           mViewRect.y(),
                           mViewRect.right(),
                           mViewRect.bottom());
            
                    float2 tilePos = pixelToTileCoords(vr.x(), vr.y());
                    
                    int32 rowx = (int32)std::floor(tilePos[0]);
                    int32 rowy = (int32)std::floor(tilePos[1]);
                    
                    float2 startPos = tileToPixelCoords((float)rowx, (float)rowy);
                    
                    float startx = startPos[0] - mTileWidth / 2;
                    float starty = startPos[1] + mTileHeight;
                    
                    bool isUpperHalf = starty - vr.y() > mTileHeight / 2;
                    bool isLeftHalf = vr.x() - startx < mTileWidth / 2;
                    
                    if(isUpperHalf) {
                        if(isLeftHalf) {
                            rowx--;
                            startx -= mTileWidth / 2;
                        } else {
                            rowy--;
                            startx += mTileWidth / 2;
                        }
                        starty -= mTileHeight / 2;
                    }
                                        
                    bool shifted = isUpperHalf ^ isLeftHalf;
                    
                    for(float y = starty; y - mTileHeight < vr.bottom(); y += mTileHeight / 2) {
                        int32 colx = rowx;
                        int32 coly = rowy;
                        
                        for(float x = startx; x < vr.right(); x += mTileWidth) {
                            if(colx >= 0 && colx < layer.width && coly >= 0 && coly < layer.height) {
                                Tile& tile = layer.tiles[colx + coly * layer.width];

                                if(tile.tile_id != -1) {
                                    TileSet& ts = mTileSets[tile.tileset_id];
                                    
                                    float dx = ts.tile_offset_x + x;
                                    float dy = ts.tile_offset_y + y - ts.tile_height;
                                    
                                    if(tile.flipped_diagonally) {
                                        dy += ts.tile_height - ts.tile_width;
                                    }
                                    if(tile.flipped_horizontally) {
                                        dx += tile.flipped_diagonally ? ts.tile_height : ts.tile_width;
                                    }
                                    if(tile.flipped_vertically) {
                                        dy += tile.flipped_diagonally ? ts.tile_width : ts.tile_height;
                                    }
                                    
                                    mMapRenderer->draw(ts.image,
                                                       Vector2(dx,
                                                               dy),
                                                       tile.tile_texture_rect,
                                                       Vector2(0, 0),
                                                       0.f,
                                                       Vector2(1.f, 1.f),
                                                       color::White * layer.opacity);
                                }
                            }
                            
                            colx++;
                            coly--;
                        }
                        
                        if(!shifted) {
                            rowx++;
                            startx += mTileWidth/2;
                            shifted = true;
                        } else {
                            rowy++;
                            startx -= mTileWidth/2;
                            shifted = false;
                        }
                    }
                    
                    mMapRenderer->endBatch();
                }
            }
        }
        
        void Map::render(const EffectTechniquePtr& technique) {
            mMapRenderer->begin(SBS_Deffered, Matrix4::TransMat(mPosition.x(), mPosition.y(), 0.f));
            
            if(mOrientation == MO_Orthogonal)
                orthogonalRender();
            else
                isometricRender();
            
            mMapRenderer->end();
        }
        
    } // namespace tmx
    
} // namespace ukn