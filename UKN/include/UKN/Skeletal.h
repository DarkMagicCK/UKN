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
    
    typedef SharedPtr<Bone> BonePtr;
    
    class Bone {
    public:
        typedef std::list<BonePtr> ChildrenList;

    public:
        Bone(const ukn_string& name);
        ~Bone();
        
        ChildrenList& getChildren();
        const ChildrenList& getChildren() const;
        
        float   getLength() const;
        float   getAngle() const;
        Vector2 getPosition() const;
        
        void setLength(float length);
        void setAngle(float angle);
        void setPosition(const Vector2& pos);
        
        AnimationPlayer& getAnimation();
        const AnimationPlayer& getAnimation() const;
                
        void  setUserData(void* data);
        void* getUserData() const;
        
        size_t size() const;
        ukn_string getName() const;
        
        BonePtr findChild(const ukn_string& name) const;
        
        void playAnimation(const ukn_string& name);
        void stopAnimation();
        void pauseAnimation();
        
        TexturePtr getTexture() const;
        
    private:
        friend class SkeletalAnimation;
        
        ukn_string mName;
        
        Bone*        mParent;
        ChildrenList mChildren;
        
        float mLength;
        float mAngle;
        Vector2 mPostion;
        
        AnimationPlayer mAnimation;
        
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
        
        size_t size() const;
        
        // render the skeletal into a spritebatch
        void render(const SpriteBatch& spriteBatch = SpriteBatch::DefaultObject());
        
        TexturePtr getTexture() const;
        
    private:
        // root bone
        BonePtr mRoot;
    };
    
} // namespace ukn

#endif
