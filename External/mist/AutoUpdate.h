//
//  AutoUpdate.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_AutoUpdate_h
#define Project_Unknown_AutoUpdate_h

#include "mist/Platform.h"
#include "mist/Util.h"

namespace mist {
    
    /**
     * Inheritate from this to make the class "auto update" in every frame by calling the onUpdate function
     **/
    class AutoUpdate: public mist::AutoListElement<AutoUpdate> {
    public:
        AutoUpdate();
        virtual ~AutoUpdate();
        
        virtual void onUpdate() = 0;
        
        void enable();
        void disable();
        
        bool isEnabled() const;
        
    private:
        bool mEnabled;
    };
    
} // namespace ukn


#endif
