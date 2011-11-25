//
//  Resource.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Resource.h"
#include "UKN/Stream.h"

namespace ukn {
    
    ResourceLoader::ResourceLoader() {
        
    }
    
    ResourceLoader::~ResourceLoader() {
        
    }
    
    ResourceLoader& ResourceLoader::Instance() {
        static ResourceLoader static_instance;
        return static_instance;
    }
    
    void ResourceLoader::addPath(const ukn_wstring& path) {
        mResourcePaths.push_back(path);
    }
    
    void ResourceLoader::removePath(const ukn_wstring& path) {
        ResourcePaths::iterator it = mResourcePaths.begin();
        while(it != mResourcePaths.end()) {
            if(*it == path) {
                mResourcePaths.erase(it);
                break;
            }
        }
    }
    
    const ResourceLoader::ResourcePaths& ResourceLoader::getResourcePaths() const {
        return mResourcePaths;
    }
    
    void ResourceLoader::addName(const ukn_wstring& name, const ukn_wstring& alias) {
        mResourceNames.insert(std::make_pair(name, alias));
    }
    
    void ResourceLoader::removeName(const ukn_wstring& name) {
        ResourceNames::iterator it = mResourceNames.find(name);
        if(it != mResourceNames.end()) {
            mResourceNames.erase(it);
        }
    }
    
    const ResourceLoader::ResourceNames& ResourceLoader::getResourceNames() const {
        return mResourceNames;
    }
    
    void ResourceLoader::registerResourceFactory(ResourceFactoryPtr rfac) {
        mResourceFactories.push_back(rfac);
    }
    
    const ResourceLoader::ResourceFactories& ResourceLoader::getResourceFactories() const {
        return mResourceFactories;
    }
    
    inline ResourcePtr ResourceLoader::onResourceLoad(const ukn_wstring& name) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            ResourcePaths::iterator itPath = mResourcePaths.begin();
            while(itPath != mResourcePaths.end()) {
                ukn_wstring fullPath = *itPath + L"/" + name;
                
                if((*itFac)->resourceExists(fullPath)) {
                    return (*itFac)->onResourceLoad(fullPath);
                }
                
                ++itPath;
            }
            
            ++itFac;
        }
        return MakeSharedPtr<Resource>(L"null", MakeSharedPtr<MemoryStream>());
    }
    
    ResourcePtr ResourceLoader::loadResource(const ukn_wstring& name_or_path) {
        ResourceNames::iterator itName = mResourceNames.find(name_or_path);
        if(itName != mResourceNames.end()) {
            return onResourceLoad(itName->second);
        } else {
            return onResourceLoad(name_or_path);
        }
    }
    
} // namespace ukn
