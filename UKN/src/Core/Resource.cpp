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
        if(mResourceStream->getStreamType() == ST_File) {
            mResourceStream = static_cast<FileStream*>(mResourceStream.get())->readIntoMemory();
        }
    }
    
    Resource::operator bool() {
        return (mResourceStream && mResourceStream->isValid());
    }
    
    class DiskResourceFactory: public ResourceFactory {
    public:
        bool resourceExists(const UknString& path) {
            return File::FileExists(path);
        }
        
        bool pathExists(const UknString& path) {
            return File::PathExists(path);
        }
        
        ResourcePtr onResourceLoad(const UknString& path) {
            FileStream* pfs = new FileStream;
            if(pfs->open(path)) {
                ResourcePtr ptr = MakeSharedPtr<Resource>(path, StreamPtr(pfs));
                return ptr;
            }
            return MakeSharedPtr<Resource>(path, StreamPtr());
        }
        
        void enumResourceNamesInPath(const UknString& path, ResourceFactory::ResourceNames& names) {
            DirectoryIterator it(path);
            while(!it.isEnd()) {
                names.push_back(it.file());
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
    
    void ResourceLoader::addPath(const UknString& path) {
        mResourcePaths.push_back(path);
    }
    
    void ResourceLoader::removePath(const UknString& path) {
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
    
    inline ResourcePtr ResourceLoader::onResourceLoad(const UknString& name, bool isFullPath) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            ResourcePaths::iterator itPath = mResourcePaths.begin();
            
			 if(!isFullPath && mResourcePaths.size() != 0) {
                while(itPath != mResourcePaths.end()) {
                    UknString fullPath = *itPath + L"/" + name;
                    
                    if((*itFac)->resourceExists(fullPath)) {
                        ResourcePtr ptr = (*itFac)->onResourceLoad(fullPath);
                        ptr->setName(fullPath);
                        return ptr;
                    }
                    
                    ++itPath;
                }
            }
            if((*itFac)->resourceExists(name)) {
                ResourcePtr ptr = (*itFac)->onResourceLoad(name);
                ptr->setName(name);
                return ptr;
            }
            
            ++itFac;
        }
        return MakeSharedPtr<Resource>(name, StreamPtr());
    }
    
    ResourcePtr ResourceLoader::loadResource(const UknString& name_or_path, bool isFullPath) {
        return onResourceLoad(name_or_path, isFullPath);
    }
    
    ResourcePtr ResourceLoader::createMemoryResource(const uint8* data, size_t size, const UknString& name) {
        return MakeSharedPtr<Resource>(name, MakeSharedPtr<MemoryStream>(data, size));
    }
    
    ResourcePtr ResourceLoader::createFileResource(const UknString& name) {
        FileStream* file_stream = new FileStream();
        if(file_stream->open(name, true, false, false))
            return MakeSharedPtr<Resource>(name, StreamPtr(file_stream));
        return ResourcePtr();
    }
    
    void ResourceLoader::enumResourceNamesInPath(const UknString& path, FileList& names) {
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
