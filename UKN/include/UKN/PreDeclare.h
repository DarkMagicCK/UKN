//
//  PreDeclare.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_PreDeclare_h
#define Project_Unknown_PreDeclare_h

#include "mist/Platform.h"
#include "mist/Resource.h"
#include "mist/Entity.h"
#include "mist/Task.h"
#include "mist/FSM.h"
#include "mist/Stream.h"
#include "mist/Component.h"
#include "mist/Ptr.h"
#include "mist/MathUtil.h"

#include <string>

namespace ukn {
    
    using namespace mist;
    
// are we building a dll or not
#if defined(MIST_OS_WINDOWS) && defined(MIST_HAS_DECLSPEC)
    #if defined(UKN_EXPORTS) || defined(UKN_DLL_EXPORTS)
        #define UKN_API __declspec(dllexport)
        #define UKN_EXTERN extern
    #else
        #define UKN_API __declspec(dllimport)
        #define UKN_EXTERN extern
    #endif
#else
    #define UKN_API
    #define UKN_EXTERN
#endif

    
    typedef MistString UknString;
    
    typedef mist::PointTemplate<1, float> float1;
    typedef mist::PointTemplate<2, float> float2;
    typedef mist::PointTemplate<3, float> float3;
    typedef mist::PointTemplate<4, float> float4;
    
    typedef mist::PointTemplate<1, int32> int1;
    typedef mist::PointTemplate<2, int32> int2;
    typedef mist::PointTemplate<3, int32> int3;
    typedef mist::PointTemplate<4, int32> int4;
    
    typedef mist::PointTemplate<1, uint32> uint1;
    typedef mist::PointTemplate<2, uint32> uint2;
    typedef mist::PointTemplate<3, uint32> uint3;
    typedef mist::PointTemplate<4, uint32> uint4;

	typedef mist::PointTemplate<1, bool> bool1;
    typedef mist::PointTemplate<2, bool> bool2;
    typedef mist::PointTemplate<3, bool> bool3;
    typedef mist::PointTemplate<4, bool> bool4;

    struct ContextCfg;
    
    typedef SharedPtr<mist::Resource> ResourcePtr;
    typedef SharedPtr<mist::Stream> StreamPtr;
    typedef SharedPtr<mist::Entity> EntityPtr;
    typedef SharedPtr<mist::Component> ComponentPtr;
    typedef SharedPtr<mist::Task> TaskPtr;
    typedef SharedPtr<mist::FSMState> FSMStatePtr;
    typedef SharedPtr<mist::ComponentHolder> ComponentHolderPtr;
    typedef SharedPtr<mist::ConfigParser> ConfigParserPtr;
    
    class GraphicDevice;
    typedef SharedPtr<GraphicDevice> GraphicDevicePtr;
    
    class GraphicFactory;
    typedef SharedPtr<GraphicFactory> GraphicFactoryPtr;
    
    class WindowFactory;
    typedef SharedPtr<WindowFactory> WindowFactoryPtr;
    
    class GraphicBuffer;
    typedef SharedPtr<GraphicBuffer> GraphicBufferPtr;
    
    class FrameBuffer;
    typedef SharedPtr<FrameBuffer> FrameBufferPtr;
    
    class RenderBuffer;
    typedef SharedPtr<RenderBuffer> RenderBufferPtr;
    
    class RenderView;
    typedef SharedPtr<RenderView> RenderViewPtr;
    
    class RenderEffect;
    typedef SharedPtr<RenderEffect> RenderEffectPtr;
    
    class Effect;
    typedef SharedPtr<Effect> EffectPtr;
    
    class Texture;
    typedef SharedPtr<Texture> TexturePtr;
    
    class Font;
    typedef SharedPtr<Font> FontPtr;
    
    class Camera;
    typedef SharedPtr<Camera> CameraPtr;
    
    class Window;
    typedef SharedPtr<Window> WindowPtr;
    
    class AppInstance;
    typedef SharedPtr<AppInstance> AppPtr;
    
    class SpriteBatch;
    typedef SharedPtr<SpriteBatch> SpriteBatchPtr;
    
    class SceneNode;
    typedef SharedPtr<SceneNode> SceneNodePtr;
    
    class Scene;
    typedef SharedPtr<Scene> ScenePtr;
           
    class Renderable;
    typedef SharedPtr<Renderable> RenderablePtr;
    
    class SceneObject;
    typedef SharedPtr<SceneObject> SceneObjectPtr;
    
    class SceneManager;
    typedef SharedPtr<SceneManager> SceneManagerPtr;
    
    class SequencialAnimation;
    typedef SharedPtr<SequencialAnimation> SequencialAnimationPtr;
    
    class RenderTarget;
    typedef SharedPtr<RenderTarget> RenderTargetPtr;

    class CompositeRenderTarget;
    typedef SharedPtr<CompositeRenderTarget> CompositeRenderTargetPtr;
    
    struct SamplerStateDesc;
    class SamplerStateObject;
    typedef SharedPtr<SamplerStateObject> SamplerStatePtr;
    
    struct BlendStateDesc;
    class BlendStateObject;
    typedef SharedPtr<BlendStateObject> BlendStatePtr;

    struct RasterizerStateDesc;
    class RasterizerStateObject;
    typedef SharedPtr<RasterizerStateObject> RasterizerStatePtr;
    
    struct DepthStencilStateDesc;
    class DepthStencilStateObject;
    typedef SharedPtr<DepthStencilStateObject> DepthStencilStatePtr;

    class LightSource;
    typedef SharedPtr<LightSource> LightSourcePtr;
    
    class LightManager;
    typedef SharedPtr<LightManager> LightManagerPtr;

    struct Material;
    typedef SharedPtr<Material> MaterialPtr;

    class Model;
    typedef SharedPtr<Model> ModelPtr;

    class Mesh;
    typedef SharedPtr<Mesh> MeshPtr;
    
} // namespace ukn


#endif
