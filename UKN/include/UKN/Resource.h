//
//  Resource.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Resource_h
#define Project_Unknown_Resource_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/Util.h"
#include "UKN/StringUtil.h"
#include "UKN/Logger.h"
#include "UKN/AutoUpdate.h"
#include "UKN/TimeUtil.h"
#include "UKN/Exception.h"

#include <map>
#include <vector>
#include <set>

#ifdef UKN_OS_WINDOWS
#include <unordered_map>
#endif

namespace ukn {
    
    class UKN_API Resource: public IResource {
    public:
        Resource(const ukn_string& name, StreamPtr resourceStream):
        mName(name),
        mResourceStream(resourceStream) {
            
        }
        
        virtual ~Resource() { }
        
        const ukn_string& getName() const {
            return this->mName;
        }
        
        uint32 getUniqueId() const {
            return this->mUniqueId;
        }
        
        StreamPtr getResourceStream() const {
            return this->mResourceStream;
        }
        
        void readIntoMemory();
        
        operator bool();
        
    private:
        friend class ResourceLoader;
        
        void setName(const ukn_string& name) {
            this->mName = name;
        }
        
    private:
        ukn_string mName;
        
        // unique id for the resource
        uint32 mUniqueId;
        
        StreamPtr mResourceStream;
    };
    
    class UKN_API ResourceFactory {
    public:
        virtual ~ResourceFactory() { }
        virtual bool resourceExists(const ukn_string& resource) = 0;
        virtual bool pathExists(const ukn_string& path) = 0;
        virtual ResourcePtr onResourceLoad(const ukn_string& path) = 0;
        
        typedef std::vector<ukn_string> ResourceNames;
        virtual void enumResourceNamesInPath(const ukn_string& path, ResourceNames& names) = 0;
    };
    
    class UKN_API ResourceLoader {
    public:
        typedef std::vector<ukn_string> ResourcePaths;
        typedef std::vector<ResourceFactoryPtr> ResourceFactories;
        typedef std::vector<ukn_string> FileList;

    public:
        static ResourceLoader& Instance();
        
        void    addPath(const ukn_string& path);
        void    removePath(const ukn_string& path);
        const   ResourcePaths& getResourcePaths() const;
        
        void    registerResourceFactory(ResourceFactoryPtr rfac);
        const   ResourceFactories& getResourceFactories() const;
        
        ResourcePtr loadResource(const ukn_string& name_or_path, bool isFullPath=false /* search resource paths? */);
        ResourcePtr createMemoryResource(const uint8* data, size_t size, const ukn_string& name=L"memory_buffer");
        ResourcePtr createFileResource(const ukn_string& name);
        
        void enumResourceNamesInPath(const ukn_string& path, FileList& names);
        
    protected:
        ResourceLoader();
        ~ResourceLoader();
        
    private:
        ResourcePtr onResourceLoad(const ukn_string& name, bool isFullPath);
        
        ResourceFactories mResourceFactories;
        ResourcePaths mResourcePaths;
    };
    
    template<typename T>
    struct ResourcePoolObject {
        T obj;
        uint64 last_time;
        uint64 owner_num;
        ukn_string name;
        
        friend bool operator < (const ResourcePoolObject<T>& l, const ResourcePoolObject<T>& r) {
            return l.live_time < r.live_time;
        }
    };
    
    template<typename T, int MAX_SIZE>
    class ResourcePool: public AutoUpdate {
    public:
        typedef Function<bool(const ukn_string& name, T& obj)> new_obj;
        typedef Function<bool(T&)> dispose_obj;
        
        enum {
            max_size = MAX_SIZE
        };
        
    public:
        ResourcePool(const new_obj& new_func, const dispose_obj& dispoce_func):
        mNew(new_func),
        mDispose(dispoce_func),
        mSize(0) {
            
        }
        
        ~ResourcePool() {
            iterator it = mResources.begin();
            for(; it != mResources.end(); ++it) {
                if(!mDispoce(it->second.obj)) {
                    log_error("ukn::ResourcePool<T, SIZE>: error disposing object");
                }
            }
        }
        
        T& get(const ukn_string& name) {
            iterator it;
            if((it = mResources.find(name)) != mResources.end()) {
                it->seoncd.owner_num++;
                if(it->second.owner_num == 1)
                    mGarbageSet.erase(name);
                return it->second.obj;
            }
            
            if(mSize >= MAX_SIZE)
                garbage_collection();
            
            it = mResources.insert(std::make_pair(name, ResourcePoolObject<T>())).first;
            it->second.live_time = FrameCounter::Instance().getRunningTime();
            it->second.owner_num = 1;
            it->second.name = name;
            if(!mNew(name, it->second.obj)) {
                UKN_THROW_EXCEPTION("ukn::ResourcePool<T, SIZE>::get: error creating new resource obj");
            }
            
            mSize ++;
            
            return it->second.obj;
            
        }
        
        void release(T& obj) {
            iterator it = mResources.begin();
            for(; it != mResources.end(); ++it) {
                if(it->second.obj == obj) {
                    release_obj(it);
                }
            }
        }
        
        bool release_by_name(const ukn_string& name) {
            iterator it = mResources.find(name);
            if(it != mResources.end()) {
                release_obj(it);
                return true;
            }
            return false;
        }
        
    private:
        void garbage_collection() {
            while(mSize >= max_size && !mGarbageSet.empty()) {
                iterator it = mResources.find(*mGarbageSet.begin());
                delete_obj(it);
                
                mGarbageSet.erase(mGarbageSet.begin());
            }
        }
        
        typedef typename ukn_hash_map<ukn_string, ResourcePoolObject<T> > ResourceMap;
        typedef typename ResourceMap::iterator iterator;
        
        void relaes_obj(iterator it) {
            it->second.owner_num--;
            if(it->second.owner_num == 0) {
                if(mSize >= max_size) {
                    delete_obj(it);
                } else {
                    mGarbageSet.insert(it->second.name);
                }
            }
        }
        
        void delete_obj(iterator it);
        
        ResourceMap mResources;
        uint32 mSize;
        
        new_obj mNew;
        dispose_obj mDispose;
        
        std::set<ukn_string> mGarbageSet;
    };
        
} // namespace ukn

#endif
