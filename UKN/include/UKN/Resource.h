//
//  Resource.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Resource_h
#define Project_Unknown_Resource_h

#include "Platform.h"
#include "PreDeclare.h"
#include "Util.h"

#include <map>
#include <vector>

namespace ukn {
    
    class UKN_API Resource {
    public:
        Resource(const ukn_wstring& name, StreamPtr resourceStream):
        mName(name),
        mResourceStream(resourceStream) {
            
        }
        
        virtual ~Resource() { }
        
        const ukn_wstring& getName() const {
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
        ukn_wstring mName;
        
        // unique id for the resource
        uint32 mUniqueId;
        
        StreamPtr mResourceStream;
    };
    
    class UKN_API ResourceFactory {
    public:
        virtual bool resourceExists(const ukn_wstring& path) = 0;
        virtual ResourcePtr onResourceLoad(const ukn_wstring& path) = 0;
        
        typedef std::vector<ukn_wstring> ResourceNames;
        virtual void enumResourceNamesInPath(const ukn_wstring& path, ResourceNames& names) = 0;
    };
    
    class UKN_API ResourceLoader {
    public:
        ResourceLoader();
        ~ResourceLoader();
        
        static ResourceLoader& Instance();
        
        void addPath(const ukn_wstring& path);
        void removePath(const ukn_wstring& path);
        
        typedef std::vector<ukn_wstring> ResourcePaths;
        const ResourcePaths& getResourcePaths() const;
        
        void addName(const ukn_wstring& name, const ukn_wstring& alias);
        void removeName(const ukn_wstring& name);
        
        typedef std::map<ukn_wstring, ukn_wstring> ResourceNames;
        const ResourceNames& getResourceNames() const;
        
        void registerResourceFactory(ResourceFactoryPtr rfac);
        
        typedef std::vector<ResourceFactoryPtr> ResourceFactories;
        const ResourceFactories& getResourceFactories() const;
        
        ResourcePtr loadResource(const ukn_wstring& name_or_path);
        ResourcePtr createMemoryResource(const uint8* data, size_t size, const ukn_wstring& name=L"memory_buffer");
        ResourcePtr createFileResource(const ukn_wstring& name);
        
    private:
        ResourcePtr onResourceLoad(const ukn_wstring& name);
        
        ResourceFactories mResourceFactories;
        
        ResourcePaths mResourcePaths;
        ResourceNames mResourceNames;
    };
    
    
} // namespace ukn

#endif
