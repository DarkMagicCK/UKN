//
//  Renderable.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Renderable_h
#define Project_Unknown_Renderable_h

#include "mist/Platform.h"
#include "mist/MathUtil.h"
#include "mist/StringUtil.h"

#include "UKN/PreDeclare.h"

namespace ukn {
    
    struct UKN_API Material {
        float3 ambient;
        float3 diffuse;
        float3 specular;
        float3 emit;
        float opacity;
        float specular_power;
        float shininess;

        typedef std::vector<std::pair<std::string, std::string> > TextureVec;
        TextureVec textures;
    };

    typedef SharedPtr<Material> MaterialPtr;

    class UKN_API Renderable: public virtual Interface {
    public:
        Renderable();
        virtual ~Renderable();

        virtual const UknString& getName() const = 0;
        
        virtual Box getBound() const = 0;
        virtual RenderBufferPtr getRenderBuffer() const = 0;
        
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        
        virtual void render();

    public:
        const Matrix4& getModelMatrix() const;
        void setModelMatrix(const Matrix4& mat);

        MaterialPtr getMaterial() const;

    public:
        const TexturePtr& getDiffuseTex() const;
        const TexturePtr& getSpecularTex() const;
        const TexturePtr& getNormalTex() const;
        const TexturePtr& getHeightTex() const;
        const TexturePtr& getEmitTex() const;

    protected:
        Matrix4 mModelMatrix;
        MaterialPtr mMaterial;

        TexturePtr mDiffuseTex;
        TexturePtr mSpecularTex;
        TexturePtr mNormalTex;
        TexturePtr mHeightTex;
        TexturePtr mEmitTex;
    };
    
    
} // namespace ukn

#endif
