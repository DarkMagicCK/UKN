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
#include "UKN/Animation.h"
#include "UKN/Event.h"

namespace ukn {
    
    enum SequencialAnimationMode {
        // continous grids
        SA_Grid,
        
        // seprate grids
        SA_GridCustom,
        
        // seprate files
        SA_SeperateFiles,
        
        // unknown
        SA_Unknown = -1,
    };
        
    class SequencialAnimation: public IConfigSerializable, public virtual Object {
    public:
        struct GridInfo {
            int32 texture_pos_x;
            int32 texture_pos_y;
            int32 grid_width;
            int32 grid_height;
            
            TexturePtr texture;
            uint32 count;
            
            SequencialAnimationMode mode;
            
            GridInfo():
            mode(SA_Unknown) { }
        };
    
    public:
        typedef Array<GridInfo> GridList;
        typedef Event<NullEventArgs> CompleteEvent;
        
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
        
        void update();
        
        void setStatus(AnimationStatus status);
        AnimationStatus getStatus() const;
        
        int32 getFrameRate() const;
        void setFrameRate(int32 fr);
        
        void setRepeatCount(int32 repeatCount);
        int32 getRepeatCount() const;
        
        CompleteEvent& onComplete();
        
        const GridInfo& getCurrentGridInfo() const;
        
    private:
        GridList mGrids;
        uint32   mTotalCount;
        ukn_string mName;
        CompleteEvent mCompleteEvent;
        
        AnimationStatus mCurrentStatus;
        size_t mCurrentGridIndex;
        
        int32 mFrameRate;
        int32 mRepeatCount;
        
        float mFrameDelta;
        float mCurrentDelta;
        int32 mCurrentRepeatCount;
        GridInfo mCurrentGridInfo;
    };
    
} // namespace ukn

#endif
