//
//  Skeletal.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/25/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Skeletal_h
#define Project_Unknown_Skeletal_h

#include "UKN/Platform.h"
#include "UKN/Animation.h"
#include "UKN/Ptr.h"
#include "UKN/MathUtil.h"
#include "UKN/Serializer.h"
#include "UKN/PreDeclare.h"
#include "UKN/SpriteBatch.h"

namespace ukn {
    
    class Bone;
    
    struct BoneKeyFrameData {
        Vector2 Scale;
        Vector2 Position;
        float   Rotation;
        float   Opacity;
        float   LayerDepth;
        bool    Visible;
        
        int32   Duration;
        
        BoneKeyFrameData():
        Scale(Vector2(1.f, 1.f)),
        Position(Vector2()),
        Rotation(0),
        Opacity(0),
        LayerDepth(0),
        Visible(true),
        Duration(500) {
            
        }
    };
    
    inline BoneKeyFrameData lerp(const BoneKeyFrameData& t1, const BoneKeyFrameData& t2, real t) {
        BoneKeyFrameData data;
        data.Scale = lerp(t1.Scale, t2.Scale, t);
        data.Position = lerp(t1.Position, t2.Position, t);
        data.Rotation = lerp(t1.Rotation, t2.Rotation, t);
        data.Opacity = lerp(t1.Opacity, t2.Opacity, t);
        data.LayerDepth = lerp(t1.LayerDepth, t2.LayerDepth, t);
        
        if(t < 1.0f)
            data.Visible = t1.Visible;
        else
            data.Visible = t2.Visible;
        
        return data;
    }
    
    enum BoneAnimationStatus {
        BAS_Playing,
        BAS_Paused,
        BAS_Stopped,
    };
    
    struct BoneAnimationCompleteArgs {
        uint32 PastTime;
        
        BoneAnimationCompleteArgs(uint32 past_time):
        PastTime(past_time) {
            
        }
    };
    
    struct BoneAnimation {
        typedef std::vector<BoneKeyFrameData> KeyFrameList;
        
        KeyFrameList KeyFrames;
        
        ukn_string  Name;
        int         RepeatCount;
        bool        Default;
        
        Event<BoneAnimationCompleteArgs> CompleteEvent;
        
        BoneAnimation();
        ~BoneAnimation();
        
        void play();
        void stop();
        void pause();
        
        void update(uint32 deltaTime);
        
        size_t getTimePos(uint32 time) const;
        size_t getTimePos(uint32 time, uint32& prevTime) const;
        
        uint32 getDuration() const;
        uint32 getPausedTime() const;
        uint32 getTotalPlayedTime() const;
        
        BoneAnimationStatus getStatus() const;
        
        const BoneKeyFrameData& getCurrentFrameData() const;
        
    private:
        void setStatus(BoneAnimationStatus status);
        
        int32               mCurrentTime;
        int32               mCurrentFrameIndex;
        int32               mCurrentRepeatCount;
        BoneAnimationStatus mCurrentStatus;
        BoneKeyFrameData    mCurrentTransform;
        
        int32               mTotalPlayedTime;
        int32               mPausedTime;
    };

    typedef SharedPtr<Bone> BonePtr;
    
    class Bone {
    public:
        typedef std::list<BonePtr> ChildrenList;
        typedef std::map<ukn_string, BoneAnimation> AnimationMap;

    public:
        Bone(const ukn_string& name);
        ~Bone();
        
        ChildrenList& getChildren();
        const ChildrenList& getChildren() const;
        
        float   getLength() const;
        float   getRotation() const;
        float   getOpacity() const;
        float   getLayerDepth() const;
        Vector2 getPosition() const;
        Vector2 getScale() const;
        Vector2 getTextureCenter() const;
        bool    isVisible() const;
        
        void    setLength(float length);
        void    setRotation(float angle);
        void    setOpacity(float opacity);
        void    setLayerDepth(float depth);
        void    setScale(const Vector2& scale);
        void    setPosition(const Vector2& pos);
        void    setTextureCenter(const Vector2& center);
        void    setVisible(bool flag);
    
        AnimationMap&       getAnimation();
        const AnimationMap& getAnimation() const;
                
        void  setUserData(void* data);
        void* getUserData() const;
        
        size_t     getSize() const;
        ukn_string getName() const;
        
        BonePtr findChild(const ukn_string& name) const;
        
        void playAnimation(const ukn_string& name);
        void stopAnimation();
        void pauseAnimation();
        
        bool isInheritRotation() const;
        bool isInheritPosition() const;
        bool isInheritVisibility() const;
        bool isInheritScale() const;
        bool isInheritOpacity() const;
        
        void setInheritRotation(bool flag);
        void setInheritPosition(bool flag);
        void setInheritVisibility(bool flag);
        void setInheritScale(bool flag);
        void setInheritOpacity(bool flag);
            
        void update(uint32 past_time);
        TexturePtr  getTexture() const;
        
    private:
        friend class SkeletalAnimation;
        
        ukn_string   mName;
        
        Bone*        mParent;
        ChildrenList mChildren;
        
        float mLength;
        float mRotation;
        float mOpacity;
        float mMaxAngle, mMinAngle;
        float mLayerDepth;
        Vector2 mPosition;
        Vector2 mScale;
        Vector2 mTextureCenter;
        bool mVisible;

        bool mInheritRotation;
        bool mInheritPosition;
        bool mInheritVisibility;
        bool mInheritScale;
        bool mInheritOpacity;
        
        AnimationMap mAnimations;
        AnimationMap::iterator mCurrentAnimation;
        
        void* mUserData;
        TexturePtr mTexture;
    };
        
    class SkeletalAnimation: public ConfigSerializable {
    public:
        SkeletalAnimation();
        ~SkeletalAnimation();

        // set / get root position
        void    setPosition(const Vector2& pos);
        Vector2 getPosition() const;
        
        bool serialize(const ConfigParserPtr& config);
        bool deserialize(const ConfigParserPtr& config);
        
        void play(const ukn_string& name);
        void pause();
        void stop();
        
        BonePtr getRoot() const;
        BonePtr getBone(const ukn_string& name) const;
        
        void addBone(const BonePtr& bone, const ukn_string& parent);
        
        size_t getSize() const;
        
        void update();
        // render the skeletal into a spritebatch
        void render(SpriteBatch& spriteBatch = SpriteBatch::DefaultObject());
        
    private:
        void onAnimationComplete(void* sender, BoneAnimationCompleteArgs& args);
        
        // root bone
        BonePtr mRoot;
        ukn_string mDefaultAnimation;
    };
    
} // namespace ukn

#endif
