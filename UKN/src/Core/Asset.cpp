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
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"


#include "mist/ConfigParser.h"
#include "mist/Resource.h"
#include "mist/Stream.h"
#include "mist/StringUtil.h"
#include "mist/Logger.h"
#include "mist/FileUtil.h"

namespace ukn {

    
    SharedPtr<Font> AssetLoader<Font>::Load(const UknString& name, const UknString& path) {
        ResourcePtr resource;
        
        if(mist::File::FileExists(path)) {
            resource = ResourceLoader::Instance().loadResource(path);
        } else {
            resource = ResourceLoader::Instance().loadResource(mist::Path::GetFont() + path);
        }
        
        if(resource) {            
            SharedPtr<Font> font = new Font();
            if(font && font->loadFromResource(resource))
                return font;
        }
        
        return SharedPtr<Font>();
    }
    
    SharedPtr<Texture> AssetLoader<Texture>::Load(const UknString& name, const UknString& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource) {            
            return Context::Instance().getGraphicFactory().load2DTexture(resource);
        }
        
        return SharedPtr<Texture>();
    }
    
    SharedPtr<ConfigParser> AssetLoader<ConfigParser>::Load(const UknString& name, const UknString& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
        if(resource) {            
            return ConfigParser::MakeParser(resource); 
        }
        
        return SharedPtr<ConfigParser>();
    }
    
    SharedPtr<Resource> AssetLoader<Resource>::Load(const UknString& name, const UknString& path) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(path);
        
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
    
    void AssetManager::add(const UknString& name, const UknString& path, AssetType type) {
        mAssetMap.insert(std::make_pair(name, AssetInfo(type, name, path)));
    }
    
    UknString AssetManager::AssetTypeToString(AssetType type) {
        switch(type) {
            case AT_Font:       return L"font"; break;
            case AT_Texture2D:  return L"texture2d"; break;
            case AT_Config:     return L"config"; break;
            case AT_Raw:        return L"raw"; break;
            default:
                return UknString();
        }
    }
    
    AssetType AssetManager::StringToAssetType(const UknString& name) {
        if(name == L"font")
            return AT_Font;
        if(name == L"texture2d")
            return AT_Texture2D;
        if(name == L"config")
            return AT_Config;
        if(name == L"raw")
            return AT_Raw;
        return AT_Unknown;
    }
    
    bool AssetManager::serialize(const ConfigParserPtr& config) {        
        if(config) {
            config->beginNode(L"assets");
            
            AssetNameMap::const_iterator it = mAssetMap.begin();
            while(it != mAssetMap.end()) {
                
                config->beginNode(L"resource");
                config->setString(L"name", it->first);
                config->setString(L"path", it->second.fullPath);
                config->setString(L"type", AssetTypeToString(it->second.type));
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
        if(config && config->toNode(L"assets")) {
            if(config->toFirstChild()) {
                do {
                    AssetInfo info;
                    
                    info.type = StringToAssetType(config->getString(L"type"));
                    if(info.type != AT_Unknown) {
                        info.name = config->getString(L"name");
                        info.fullPath = config->getString(L"path");
                        
                        if(!info.name.empty() && 
                           !info.fullPath.empty()) {
                            mAssetMap.insert(std::make_pair(info.name, info));
                        }
                    }
                } while( config->toNextChild() );
                
                config->toParent();
                return true;
            }
        } else {
            log_error("ukn::AssetManager::deserialize: unable to deserialize asset manager, invalid config state");
        }
        return false;
    }
    
} // namespace ukn