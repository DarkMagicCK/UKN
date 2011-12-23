//
//  Asset.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Asset.h"
#include "UKN/Font.h"
#include "UKN/Texture.h"
#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"
#include "UKN/Stream.h"
#include "UKN/StringUtil.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/Logger.h"

namespace ukn {
    
    SharedPtr<Font> AssetLoader<Font>::Load(const ukn_wstring& name, const ukn_wstring& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource) {
            resource->setName(name);
            
            SharedPtr<Font> font = new Font();
            if(font && font->loadFromResource(resource))
                return font;
        }
        
        return SharedPtr<Font>();
    }
    
    SharedPtr<Texture> AssetLoader<Texture>::Load(const ukn_wstring& name, const ukn_wstring& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource) {
            resource->setName(name);
            
            return Context::Instance().getGraphicFactory().load2DTexture(resource);
        }
        
        return SharedPtr<Texture>();
    }
    
    SharedPtr<ConfigParser> AssetLoader<ConfigParser>::Load(const ukn_wstring& name, const ukn_wstring& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource) {
            resource->setName(name);
            
            return MakeConfigParser(resource); 
        }
        
        return SharedPtr<ConfigParser>();
    }
    
    SharedPtr<Resource> AssetLoader<Resource>::Load(const ukn_wstring& name, const ukn_wstring& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource)
            resource->setName(name);
        
        return resource;
    }
    
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
    
    ukn_string AssetManager::AssetTypeToString(AssetType type) {
        switch(type) {
            case AT_Font:       return "font"; break;
            case AT_Texture2D:  return "texture2d"; break;
            case AT_Config:     return "config"; break;
            case AT_Raw:        return "raw"; break;
            default:
                return ukn_string();
        }
    }
    
    AssetType AssetManager::StringToAssetType(const ukn_string& name) {
        if(name == "font")
            return AT_Font;
        if(name == "texture2d")
            return AT_Texture2D;
        if(name == "config")
            return AT_Config;
        if(name == "raw")
            return AT_Raw;
        return AT_Unknown;
    }
    
    bool AssetManager::serialize(const ConfigParserPtr& config) {        
        if(config) {
            config->beginNode("assets");
            
            AssetNameMap::const_iterator it = mAssetMap.begin();
            while(it != mAssetMap.end()) {
                
                config->beginNode("resource");
                config->setString("name", wstring_to_string(it->first));
                config->setString("path", wstring_to_string(it->second.fullPath));
                config->setString("type", AssetTypeToString(it->second.type));
                config->endNode();
 
                ++it;
            }
            
            config->endNode();
            
            return true;
        } else {
            log_error("ukn::AssetManager::serialize: unable to serialize asset manager, invalid config ptr");
        }
        return false;
    }
    
    bool AssetManager::deserialize(const ConfigParserPtr& config) {
        if(config && config->toNode("/assets")) {
            if(config->toFirstChild()) {
                do {
                    AssetInfo info;
                    
                    info.type = StringToAssetType(config->getString("type"));
                    if(info.type != AT_Unknown) {
                        info.name = string_to_wstring(config->getString("name"));
                        info.fullPath = string_to_wstring(config->getString("path"));
                        
                        if(!info.name.empty() && 
                           !info.fullPath.empty()) {
                            mAssetMap.insert(std::make_pair(info.name, info));
                        }
                    }
                } while( config->toNextChild() );
                
                return true;
            }
        } else {
            log_error("ukn::AssetManager::deserialize: unable to deserialize asset manager, invalid config state");
        }
        return false;
    }
    
} // namespace ukn