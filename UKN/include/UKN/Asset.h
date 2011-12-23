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
        static SharedPtr<Font> Load(const ukn_wstring& name, const ukn_wstring& path);
    };
    
    template<>
    class AssetLoader<Texture> {
    public:
        static SharedPtr<Texture> Load(const ukn_wstring& name, const ukn_wstring& path);
    };
    
    template<>
    class AssetLoader<ConfigParser> {
    public:
        static SharedPtr<ConfigParser> Load(const ukn_wstring& name, const ukn_wstring& path);
    };
    
    template<>
    class AssetLoader<Resource> {
    public:
        static SharedPtr<Resource> Load(const ukn_wstring& name, const ukn_wstring& path);
    };
    
    class AssetManager: Uncopyable, public ConfigSerializable {
    public:
        static AssetManager& Instance();
        
        struct AssetInfo {
            AssetInfo():
            type(AT_Unknown),
            fullPath(ukn_wstring()) { 
            
            }
            
            AssetInfo(AssetType t, const ukn_wstring& n, const ukn_wstring& path):
            type(t),
            name(n),
            fullPath(path) { 
            
            }
            
            AssetType   type;
            ukn_wstring name;
            ukn_wstring fullPath;
        };
        
        typedef ukn_hash_map<ukn_wstring, AssetInfo> AssetNameMap;
        
    public:
        template<typename T>
        SharedPtr<T> load(const ukn_wstring& name) const;
        
        void add(const ukn_wstring& name, const ukn_wstring& path, AssetType type);
        
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
    SharedPtr<T> AssetManager::load(const ukn_wstring& name) const {
        AssetNameMap::const_iterator it = mAssetMap.find(name);
        if(it != mAssetMap.end()) {
            return AssetLoader<T>::Load(name, it->second.fullPath);
        }
        return SharedPtr<T>();
    }
    
} // namespace ukn


#endif
