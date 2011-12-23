//
//  Asset.h
//  Project Unknown
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Asset_h
#define Project_Unknown_Asset_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"
#include "UKN/Serializer.h"
#include "UKN/Ptr.h"

#include <map>

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
        static SharedPtr<Font> Load(const String& name, const String& path);
    };
    
    template<>
    class AssetLoader<Texture> {
    public:
        static SharedPtr<Texture> Load(const String& name, const String& path);
    };
    
    template<>
    class AssetLoader<ConfigParser> {
    public:
        static SharedPtr<ConfigParser> Load(const String& name, const String& path);
    };
    
    template<>
    class AssetLoader<Resource> {
    public:
        static SharedPtr<Resource> Load(const String& name, const String& path);
    };
    
    class AssetManager: Uncopyable, public ConfigSerializable {
    public:
        static AssetManager& Instance();
        
        struct AssetInfo {
            AssetInfo():
            type(AT_Unknown),
            fullPath(String()) { 
            
            }
            
            AssetInfo(AssetType t, const String& n, const String& path):
            type(t),
            name(n),
            fullPath(path) { 
            
            }
            
            AssetType   type;
            String name;
            String fullPath;
        };
        
        typedef ukn_hash_map<String, AssetInfo> AssetNameMap;
        
    public:
        template<typename T>
        SharedPtr<T> load(const String& name) const;
        
        void add(const String& name, const String& path, AssetType type);
        
        const AssetNameMap& getAssets() const;   
        
        bool serialize(const ConfigParserPtr& config);
        bool deserialize(const ConfigParserPtr& config);
        
        static ukn_string AssetTypeToString(AssetType type);
        static AssetType  StringToAssetType(const ukn_string& name);
        
    protected:
        AssetManager();
        ~AssetManager();
        
    private:
        AssetNameMap mAssetMap;
    };
    
    
    template<typename T>
    SharedPtr<T> AssetManager::load(const String& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end()) {
            return AssetLoader<T>::Load(name, it->second.fullPath);
        }
        return SharedPtr<T>();
    }
    
} // namespace ukn


#endif
