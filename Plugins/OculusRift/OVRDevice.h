#ifndef UKN_OVR_DEVICE_H_
#define UKN_OVR_DEVICE_H_

#pragma once

#include "OVR.h"

#include "mist/Platform.h"
#include "mist/MathUtil.h"

#ifdef MIST_OS_WINDOWS

    #pragma comment(lib, "mist.lib")
    #pragma comment(lib, "ukn_dll.lib")
    // OVR::Timer
    #pragma comment(lib, "winmm.lib")

    #ifdef MIST_PLATFORM_32

        #pragma comment(lib, "libovr.lib")

    #elif defined(MIST_PLATFORM_64)

        #pragma comment(lib, "libovr64.lib")

    #endif

#endif

namespace ukn {
    
    using namespace mist;

    class OVRDevice: public OVR::MessageHandler {
    public:
        static OVRDevice& Instance();

    public:
        OVRDevice();
        virtual ~OVRDevice();

        bool checkDevice();
        void close();

        bool hasDevice() const;

        // OVR Device resolution, currently should be 1280 * 800 for DK
        uint32 getHResolution() const;
        uint32 getVResolution() const;

        float getDistortionScale();

    public:
        // getters
        const OVR::HMDInfo& getHMDInfo() const;
        const OVR::DeviceInfo& getDeviceInfo() const;

        OVR::SensorFusion& getSensorFusion();
        const OVR::SensorFusion& getSensorFusion() const;
        
        OVR::Util::Render::StereoConfig& getStereoConfig();
        const OVR::Util::Render::StereoConfig& getStereoConfig() const;

    public:
        // OVR::MessageHandler
        virtual void OnMessage(const OVR::Message& msg) override;

    private:
        bool mHasDevice;

        OVR::Ptr<OVR::DeviceManager> mManager;
        OVR::Ptr<OVR::HMDDevice> mDevice;
        OVR::Ptr<OVR::SensorDevice> mSensor;

        OVR::HMDInfo mHMDInfo;
        OVR::DeviceInfo mDeviceInfo;
        OVR::SensorFusion mSensorFusion;
        OVR::Util::Render::StereoConfig mStereoConfig;
    };

    class OVRUtils {
    public:
        static void OVRMatToMat(const OVR::Matrix4f& ovrMat, Matrix4& mat);
        static Matrix4 OVRMatToMat(const OVR::Matrix4f& ovrMat);

    };

}


#endif