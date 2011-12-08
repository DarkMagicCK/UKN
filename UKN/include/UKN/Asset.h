//
//  Asset.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Asset_h
#define Project_Unknown_Asset_h

#include "UKN/Platformh.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"

namespace ukn {
    
    class AssetManager: Uncopyable {
    public:
        static AssetManager& Instance();
        
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
    
        struct AssetInfo {
            AssetInfo():
            type(RT_Raw),
            fullPath(ukn_wstring()) { 
            
            }
            
            AssetInfo(AssetType t, const ukn_wstring& path):
            type(t),
            fullPath(path) { 
            
            }
            
            AssetType   type;
            ukn_wstring fullPath;
        };
        
        typedef std::map<ukn_wstring, AssetInfo> AssetNameMap;
        
    public:
        template<typename T>
        SharedPtr<T> load(const ukn_wstring& name);
        
    protected:
        AssetManager();
        ~AssetManager();
        
    private:
        AssetNameMap mAssetMap;
    };
    
} // namespace ukn


#endif
