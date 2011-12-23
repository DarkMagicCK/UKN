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

#include <map>
#include <vector>

namespace ukn {
    
    class UKN_API Resource {
    public:
        Resource(const String& name, StreamPtr resourceStream):
        mName(name),
        mResourceStream(resourceStream) {
            
        }
        
        virtual ~Resource() { }
        
        const String& getName() const {
            return this->mName;
        }
        
        void setName(const String& name) {
            this->mName = name;
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
        String mName;
        
        // unique id for the resource
        uint32 mUniqueId;
        
        StreamPtr mResourceStream;
    };
    
    class UKN_API ResourceFactory {
    public:
        virtual bool resourceExists(const String& resource) = 0;
        virtual bool pathExists(const String& path) = 0;
        virtual ResourcePtr onResourceLoad(const String& path) = 0;
        
        typedef std::vector<String> ResourceNames;
        virtual void enumResourceNamesInPath(const String& path, ResourceNames& names) = 0;
    };
    
    class UKN_API ResourceLoader {
    public:
        typedef std::vector<String> ResourcePaths;
        typedef std::vector<ResourceFactoryPtr> ResourceFactories;
        typedef std::vector<String> FileList;

    public:
        ResourceLoader();
        ~ResourceLoader();
        
        static ResourceLoader& Instance();
        
        void    addPath(const String& path);
        void    removePath(const String& path);
        const   ResourcePaths& getResourcePaths() const;
        
        void    registerResourceFactory(ResourceFactoryPtr rfac);
        const   ResourceFactories& getResourceFactories() const;
        
        ResourcePtr loadResource(const String& name_or_path, bool isFullPath=false /* search resource paths? */);
        ResourcePtr createMemoryResource(const uint8* data, size_t size, const String& name=L"memory_buffer");
        ResourcePtr createFileResource(const String& name);
        
        void enumResourceNamesInPath(const String& path, FileList& names);
        
    private:
        ResourcePtr onResourceLoad(const String& name, bool isFullPath);
        
        ResourceFactories mResourceFactories;
      
        ResourcePaths mResourcePaths;
    };
    
    
} // namespace ukn

#endif
