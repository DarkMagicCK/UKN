//
//  Resource.cpp
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Resource.h"
#include "mist/Stream.h"
#include "mist/StringUtil.h"
#include "mist/FileUtil.h"

namespace mist {
    
    StreamPtr Resource::readIntoMemory() {
        if(mResourceStream) {
            if(mResourceStream->getStreamType() == ST_File) {
                mResourceStream = static_cast<FileStream*>(mResourceStream.get())->readIntoMemory();
            }
            return this->mResourceStream;
        }
        return StreamPtr();
    }
    
    Resource::operator bool() {
        return (mResourceStream && mResourceStream->isValid());
    }

    ResourcePtr Resource::MakeResourcePtr(const StreamPtr& stream, const MistString& name) {
        return MakeSharedPtr<Resource>(name, stream);
    }
    
    class DiskResourceFactory: public ResourceFactory {
    public:
        bool resourceExists(const MistString& path) {
            return File::FileExists(path);
        }
        
        bool pathExists(const MistString& path) {
            return File::PathExists(path);
        }
        
        ResourcePtr createResource(const MistString& path) {
            FileStream* pfs = new FileStream;
            if(pfs->open(path)) {
                ResourcePtr ptr = MakeSharedPtr<Resource>(path, StreamPtr(pfs));
                return ptr;
            }
            return MakeSharedPtr<Resource>(path, StreamPtr());
        }
        
        void enumResourceNamesInPath(const MistString& path, ResourceFactory::ResourceNames& names) {
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
    
    void ResourceLoader::addPath(const MistString& path) {
        mResourcePaths.push_back(path);
    }
    
    void ResourceLoader::removePath(const MistString& path) {
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
    
    inline ResourcePtr ResourceLoader::onResourceLoad(const MistString& name, bool isFullPath) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            ResourcePaths::iterator itPath = mResourcePaths.begin();
            
			 if(!isFullPath && mResourcePaths.size() != 0) {
                while(itPath != mResourcePaths.end()) {
                    MistString fullPath = *itPath + L"/" + name;
                    
                    if((*itFac)->resourceExists(fullPath)) {
                        ResourcePtr ptr = (*itFac)->createResource(fullPath);
                        ptr->setName(name);
                        return ptr;
                    }
                    
                    ++itPath;
                }
            }
            if((*itFac)->resourceExists(name)) {
                ResourcePtr ptr = (*itFac)->createResource(name);
                ptr->setName(name);
                return ptr;
            }
            
            ++itFac;
        }
        log_error(L"ResourceManager: Error loading resource: " + name);
        return ResourcePtr();
    }
    
    ResourcePtr ResourceLoader::loadResource(const MistString& name_or_path, bool isFullPath) {
        return onResourceLoad(name_or_path, isFullPath);
    }
    
    ResourcePtr ResourceLoader::createMemoryResource(const uint8* data, size_t size, const MistString& name) {
        return MakeSharedPtr<Resource>(name, MakeSharedPtr<MemoryStream>(data, size));
    }
    
    ResourcePtr ResourceLoader::createFileResource(const MistString& name) {
        FileStream* file_stream = new FileStream();
        if(file_stream->open(name, true, false, false))
            return MakeSharedPtr<Resource>(name, StreamPtr(file_stream));
        return ResourcePtr();
    }
    
    void ResourceLoader::enumResourceNamesInPath(const MistString& path, FileList& names) {
        ResourceFactories::iterator itFac = mResourceFactories.begin();
        while(itFac != mResourceFactories.end()) {
            if((*itFac)->pathExists(path)) {
                (*itFac)->enumResourceNamesInPath(path, names);
                break;
            }
            
            ++itFac;
        }
    }
    
} // namespace mist
