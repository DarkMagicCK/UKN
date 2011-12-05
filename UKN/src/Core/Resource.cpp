//
//  Resource.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Resource.h"
#include "UKN/Stream.h"
#include "UKN/StringUtil.h"
#include "UKN/FileUtil.h"

namespace ukn {
    
    void Resource::readIntoMemory() {
        if(mResourceStream->getStreamType() == Stream::ST_File) {
            mResourceStream = static_cast<FileStream*>(mResourceStream.get())->readIntoMemory();
        }
    }
    
    Resource::operator bool() {
        return (mResourceStream && mResourceStream->isValid());
    }
    
    class DiskResourceFactory: public ResourceFactory {
    public:
        bool resourceExists(const ukn_wstring& path) {
            return file_exists(path);
        }
        
        bool pathExists(const ukn_wstring& path) {
            return path_exists(path);
        }
        
        ResourcePtr onResourceLoad(const ukn_wstring& path) {
            FileStream* pfs = new FileStream;
            if(pfs->open(path)) {
                return MakeSharedPtr<Resource>(path, pfs);
            }
            return MakeSharedPtr<Resource>(path, StreamPtr());
        }
        
        void enumResourceNamesInPath(const ukn_wstring& path, ResourceFactory::ResourceNames& names) {
            DirectoryIterator it(path);
            while(!it.isEnd()) {
                names.push_back(string_to_wstring(it.file()));
                ++it;
            }
        }
    };
    
    ResourceLoader::ResourceLoader() {
        registerResourceFactory(MakeSharedPtr<DiskResourceFactory>());
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
    
    void ResourceLoader::registerResourceFactory(ResourceFactoryPtr rfac) {
        mResourceFactories.push_back(rfac);
    }
    
    const ResourceLoader::ResourceFactories& ResourceLoader::getResourceFactories() const {
        return mResourceFactories;
    }
    
    inline ResourcePtr ResourceLoader::onResourceLoad(const ukn_wstring& name, bool isFullPath) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            ResourcePaths::iterator itPath = mResourcePaths.begin();
            
			 if(!isFullPath && mResourcePaths.size() != 0) {
                while(itPath != mResourcePaths.end()) {
                    ukn_wstring fullPath = *itPath + L"/" + name;
                    
                    if((*itFac)->resourceExists(fullPath)) {
                        return (*itFac)->onResourceLoad(fullPath);
                    }
                    
                    ++itPath;
                }
            }
            if((*itFac)->resourceExists(name)) {
                return (*itFac)->onResourceLoad(name);
            }
            
            ++itFac;
        }
        return MakeSharedPtr<Resource>(name, StreamPtr());
    }
    
    ResourcePtr ResourceLoader::loadResource(const ukn_wstring& name_or_path, bool isFullPath) {
        return onResourceLoad(name_or_path, isFullPath);
    }
    
    ResourcePtr ResourceLoader::createMemoryResource(const uint8* data, size_t size, const ukn_wstring& name) {
        return MakeSharedPtr<Resource>(name, MakeSharedPtr<MemoryStream>(data, size));
    }
    
    ResourcePtr ResourceLoader::createFileResource(const ukn_wstring& name) {
        FileStream* file_stream = new FileStream();
        if(file_stream->open(name, true, false, false))
            return MakeSharedPtr<Resource>(name, file_stream);
        return ResourcePtr();
    }
    
    void ResourceLoader::enumResourceNamesInPath(const ukn_wstring& path, FileList& names) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            if((*itFac)->pathExists(path)) {
                (*itFac)->enumResourceNamesInPath(path, names);
                break;
            }
            
            ++itFac;
        }
    }
    
} // namespace ukn
