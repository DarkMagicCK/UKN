//
//  Asset.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Asset_h
#define Project_Unknown_Asset_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"

#include <map>

namespace ukn {
    
    enum AssetType {
        // Font
        RT_Font,
        // Texture2D
        RT_Texture2D,
        // ConfigParser
        RT_Config,
        // RawData
        RT_Raw,
    };
    
    class AssetManager: Uncopyable {
    public:
        static AssetManager& Instance();
        
        struct AssetInfo {
            AssetInfo():
            type(RT_Raw),
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
        
        ConfigParserPtr serialize();
        void            unserialize(const ConfigParserPtr& config);
        
    protected:
        AssetManager();
        ~AssetManager();
        
    private:
        AssetNameMap mAssetMap;
    };
    
} // namespace ukn


#endif
