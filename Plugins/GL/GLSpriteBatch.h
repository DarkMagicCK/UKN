//
//  GLSpriteBatch.h
//  Project Unknown
//
//  Created by Robert Bu on 12/8/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLSpriteBatch_h
#define Project_Unknown_GLSpriteBatch_h

#include "UKN/SpriteBatch.h"

namespace ukn {

    /**
     * Specialized sprite batch for faster 2d texture rendering
     **/
    class GLSpriteBatch: public SpriteBatch {
    public:
        GLSpriteBatch();
        virtual ~GLSpriteBatch();
        
        virtual void onRender();
    };
    
    
} // namespace ukn

#endif
