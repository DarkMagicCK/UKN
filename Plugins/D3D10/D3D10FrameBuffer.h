#ifndef Project_Unknown_D3D10FrameBuffer_h
#define Project_Unknown_D3D10FrameBuffer_h

#include "mist/Platform.h"
#include "UKN/FrameBuffer.h"

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10FrameBuffer: public FrameBuffer {
    public:
        D3D10FrameBuffer(bool offscreen, D3D10GraphicDevice* device);
        virtual ~D3D10FrameBuffer();
        
        bool requiresFlipping() const;
        
        void onBind() override;
        void onUnbind() override;
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil) override;
        
        void swapBuffers() override;
        
        SharedPtr<uint8> readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format);
        
    private:
        bool mOffscreen;
        D3D10GraphicDevice* mGraphicDevice;

        friend class GLWindow;
    };
    
    typedef SharedPtr<D3D10FrameBuffer> GLFrameBufferPtr;

} // namespace ukn

#endif