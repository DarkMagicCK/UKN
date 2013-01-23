#ifndef Project_Unknown_D3D11FrameBuffer_h
#define Project_Unknown_D3D11FrameBuffer_h

#include "mist/Platform.h"
#include "UKN/FrameBuffer.h"

namespace ukn {

	class D3D11GraphicDevice;

	class D3D11FrameBuffer: public FrameBuffer {
    public:
        D3D11FrameBuffer(bool offscreen, D3D11GraphicDevice* device);
        virtual ~D3D11FrameBuffer();
        
        bool requiresFlipping() const;
        
        void onBind() override;
        void onUnbind() override;
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil) override;
        
        void swapBuffers() override;
        
        SharedPtr<uint8> readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format);
        
    private:
        bool mOffscreen;
        D3D11GraphicDevice* mGraphicDevice;

        friend class GLWindow;
    };
    
    typedef SharedPtr<D3D11FrameBuffer> GLFrameBufferPtr;

} // namespace ukn

#endif