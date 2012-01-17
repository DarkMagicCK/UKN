//
//  SequencialAnimation.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/17/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_SequencialAnimation_h
#define Project_Unknown_SequencialAnimation_h

#include "UKN/Platform.h"
#include "UKN/Basic.h"
#include "UKN/Serializer.h"
#include "UKN/PreDeclare.h"
#include "UKN/Util.h"
#include "UKN/Event.h"

namespace ukn {
    
    enum SequencialAnimationType {
        // continous grids
        SA_Grid,
        
        // seprate grids
        SA_GridCustom,
        
        // seprate files
        SA_SeprateFiles,
        
        // unknown
        SA_Unknown = -1,
    };
    
    enum 
    
    class SequencialAnimation: public virtual Object, public ConfigSerializable {
    public:
        typedef Array<GridInfo> GridList;
        typedef Event<NullEventArgs> CompleteEvent;

    public:
        struct GridInfo {
            int32 texture_pos_x;
            int32 texture_pos_y;
            int32 texture_width;
            int32 texture_height;
            
            TexturePtr texture;
            uint32 count;
        };
        
        SequencialAnimation();
        virtual ~SequencialAnimation();
        
        virtual bool serialize(const ConfigParserPtr& config);
        virtual bool deserialize(const ConfigParserPtr& config);
        
        uint32 getCount() const;
        
        const GridInfo& getRootInfo() const;
        const GridList& getGridList() const;
        
        void play();
        void pause();
        void stop();
        
        void setStatus(AnimationStatus status);
        AnimationStatus getStatus() const;
        
        CompleteEvent& onComplete();
        
    private:
        GridList mGrids;
        CompleteEvent mCompleteEvent;
        
        AnimationStatus mCurrentStatus;
        size_t mCurrentGridIndex;
    };
    
} // namespace ukn

#endif
