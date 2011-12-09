//
//  Asset.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "Asset.h"

namespace ukn {
    
    AssetManager::AssetManager() {
        
    }
    
    AssetManager::~AssetManager() {
        
    }
    
    const AssetManager::AssetNameMap& AssetManager::getAssets() const {
        return mAssetMap;
    }
    
} // namespace ukn