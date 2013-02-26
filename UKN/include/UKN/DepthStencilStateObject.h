#ifndef UKN_DEPTH_STENCIL_STATE_OBJECT_H_
#define UKN_DEPTH_STENCIL_STATE_OBJECT_H_

#include "ukn/PreDeclare.h"
#include "ukn/GraphicSettings.h"

namespace ukn {

    // depth stencil state
    // for opengl, will use gl...Separate if available
    struct UKN_API DepthStencilStateDesc {
        bool depth_enable;
        bool depth_write_mask; /* true = depth write, false = depth read */
        RenderStateParam depth_func;
        
        bool stencil_enable;
        uint8 stencil_write_mask;
//        uint8 stencil_read_mask; /* stencil read mask in opengl ? */
        uint8 stencil_ref;

        struct StencilState {
            RenderStateParam func;
            RenderStateParam stencil_fail_op;
            RenderStateParam depth_fail_op;
            RenderStateParam pass_op;

            StencilState():
            func(RSP_CompAlways),
            stencil_fail_op(RSP_StencilOpKeep),
            depth_fail_op(RSP_StencilOpIncr),
            pass_op(RSP_StencilOpKeep) { }
        };
        StencilState front;
        StencilState back;

        DepthStencilStateDesc():
        depth_enable(true),
        depth_write_mask(true),
        depth_func(RSP_CompLess),
        stencil_enable(false),
        stencil_write_mask(0xFF),
        stencil_ref(0) { 
            back.depth_fail_op = RSP_StencilOpDecr;
        }
    };

    class UKN_API DepthStencilStateObject {
    public:
        DepthStencilStateObject(const DepthStencilStateDesc& desc);

        virtual ~DepthStencilStateObject() = 0;
    
        const DepthStencilStateDesc& getDesc() const;

    public:
        // Default
        //   depth = true, depth write = true
        // Depth Read
        //   depth = true, depth write = false
        // None
        //   depth = false, depth write = false
        static void InitializeBuildInStates();

        static DepthStencilStatePtr Default();
        static DepthStencilStatePtr DepthRead();
        static DepthStencilStatePtr None();

    private:
        DepthStencilStateDesc mDesc;
    };

    typedef SharedPtr<DepthStencilStateObject> DepthStencilStatePtr;
}

#endif