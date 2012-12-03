//
//  Asset.h
//  Project Unknown
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Asset_h
#define Project_Unknown_Asset_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"
#include "mist/Serializer.h"
#include "mist/Ptr.h"

#include "UKN/PreDeclare.h"

#include <map>
#ifdef MIST_CPP11
#include <unordered_map>
#else

#endif

namespace ukn {
    
    enum AssetType {
        AT_Unknown = -1,
        // Font
        AT_Font = 0,
        // Texture2D
        AT_Texture2D,
        // ConfigParser
        AT_Config,
        // RawData
        AT_Raw,
    };
    
    template<typename T>
    class AssetLoader;
    
    template<>
    class AssetLoader<Font> {
    public:
        static SharedPtr<Font> Load(const UknString& name, const UknString& path);
    };
    
    template<>
    class AssetLoader<Texture> {
    public:
        static SharedPtr<Texture> Load(const UknString& name, const UknString& path);
    };
    
    template<>
    class AssetLoader<ConfigParser> {
    public:
        static SharedPtr<ConfigParser> Load(const UknString& name, const UknString& path);
    };
    
    template<>
    class AssetLoader<Resource> {
    public:
        static SharedPtr<Resource> Load(const UknString& name, const UknString& path);
    };
    
    class AssetManager: Uncopyable, public IConfigSerializable {
    public:
        static AssetManager& Instance();
        
        struct AssetInfo {
            AssetInfo():
            type(AT_Unknown),
            fullPath(UknString()) { 
            
            }
            
            AssetInfo(AssetType t, const UknString& n, const UknString& path):
            type(t),
            name(n),
            fullPath(path) { 
            
            }
            
            AssetType      type;
            UknString      name;
            UknString      fullPath;
        };
        
        typedef std::unordered_map<UknString, AssetInfo> AssetNameMap;
        
    public:
        template<typename T>
        SharedPtr<T> load(const UknString& name) const;
        
        void add(const UknString& name, const UknString& path, AssetType type);
        
        const AssetNameMap& getAssets() const;   
        
        bool serialize(const ConfigParserPtr& config);
        bool deserialize(const ConfigParserPtr& config);
        
        static UknString AssetTypeToString(AssetType type);
        static AssetType StringToAssetType(const UknString& name);
        
    protected:
        AssetManager();
        ~AssetManager();
        
    private:
        AssetNameMap mAssetMap;
    };
    
    template<typename T>
    SharedPtr<T> AssetManager::load(const UknString& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end()) {
            return AssetLoader<T>::Load(name, it->second.fullPath);
        } else {
            // try to load as resource 
            return AssetLoader<T>::Load(name, name);
        }
        return SharedPtr<T>();
    }
    
} // namespace ukn


#endif
