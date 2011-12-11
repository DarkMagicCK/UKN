//
//  Asset.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Asset.h"
#include "UKN/Font.h"
#include "UKN/Texture.h"
#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"
#include "UKN/Stream.h"

namespace ukn {
    
    AssetManager& AssetManager::Instance() {
        static AssetManager instance;
        return instance;
    }
    
    AssetManager::AssetManager() {
        
    }
    
    AssetManager::~AssetManager() {
        
    }
    
    const AssetManager::AssetNameMap& AssetManager::getAssets() const {
        return mAssetMap;
    }
    
    void AssetManager::add(const ukn_wstring& name, const ukn_wstring& path, AssetType type) {
        mAssetMap.insert(std::make_pair(name, AssetInfo(type, name, path)));
    }
    
    template<>
    SharedPtr<Font> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end() && it->second.type == RT_Font) {
            ResourcePtr resource = ResourceLoader::Instance().loadResource(it->second.fullPath);
            
            if(resource) {
                SharedPtr<Font> font = new Font();
                if(font && font->loadFromResource(resource))
                    return font;
            }
        }
        return SharedPtr<Font>();
    }
    
    ConfigParserPtr AssetManager::serialize() {
        
    }
    
    void AssetManager::unserialize(const ConfigParserPtr& config) {
        
    }
    
} // namespace ukn