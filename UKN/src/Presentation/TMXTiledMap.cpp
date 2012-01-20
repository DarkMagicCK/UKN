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

namespace ukn {
    
    namespace tmx {
        
        Map::Map() {
            
        }
        
        Map::Map(const String& map_file) {
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
        
        bool Map::serialize(const ConfigParserPtr& config) {
            return false;
        }
        
        bool Map::deserialize(const ConfigParserPtr& config) {
            
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
        
        void Map::render() {
            UKN_ENUMERABLE_FOREACH(TileSet, ts, mTileSets) {
                
            }
        }
        
    } // namespace tmx
    
} // namespace ukn