#include "OVRDevice.h"

#include "mist/Singleton.h"
#include "mist/Logger.h"
#include "mist/Convert.h"


namespace ukn {

    using namespace OVR;

    namespace {
        mist::SingletonHolder<OVRDevice, void> g_device;
    }

    OVRDevice& OVRDevice::Instance() {
        return *g_device.get();
    }

    OVRDevice::OVRDevice():
    mHasDevice(false) {
        System::Init();

        mManager = *DeviceManager::Create();
        mManager->SetMessageHandler(this);
    }

    OVRDevice::~OVRDevice() {

    }

    bool OVRDevice::hasDevice() const {
        return mHasDevice;
    }

    bool OVRDevice::checkDevice() {
        if(mManager) {
            if(mHasDevice) {
                mist::log_warning(L"libOVR: attempt reintialize a initialized device");
                return false;
            }
            mDevice = mManager->EnumerateDevices<HMDDevice>().CreateDevice();

            if(mDevice) {
                if(!mDevice->GetDeviceInfo(&mHMDInfo)) {
                    mist::log_error(L"libOVR: Error getting device info");
                    return false;
                }
                mSensor = mDevice->GetSensor();
                if(!mSensor) {
                    mist::log_error("libOVR: error getting sensor from device");
                    return false;
                }
                mSensor->GetDeviceInfo(&mDeviceInfo);
                
                mist::log_info(
                    mist::format_string(
                        "libOVR: got OVR device\n\tManuafactor = %s Version = %d\n\tDisplay Name = %s, Eye Distance = %.2f\n\tDistortionK = %.2f %.2f %.2f %.2f",
                        mDeviceInfo.Manufacturer,
                        mDeviceInfo.Version,
                        mHMDInfo.DisplayDeviceName,
                        mHMDInfo.EyeToScreenDistance,
                        mHMDInfo.DistortionK[0],
                        mHMDInfo.DistortionK[0],
                        mHMDInfo.DistortionK[0],
                        mHMDInfo.DistortionK[0]
                    )
                );

                if(!mSensorFusion.AttachToSensor(mSensor)) {
                    mist::log_error("libOVR: error attaching sensor fusion to sensor");
                    return false;
                }
                mSensorFusion.SetDelegateMessageHandler(this);
                mSensorFusion.SetPredictionEnabled(true);

                mStereoConfig.SetFullViewport(OVR::Util::Render::Viewport(0, 0, getHResolution(), getVResolution()));
                mStereoConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
                mStereoConfig.Set2DAreaFov(mist::math::degree_to_radius(85.f));

                mist::log_info(mist::format_string("libOVR: scene distortion scale = %.2f", mStereoConfig.GetDistortionScale()));

                mist::log_info("libOVR: initialization succeed, you can use OVRCameraController now");
                mHasDevice = true;
                return true;
            }
            
            return true;
        } else {
            mist::log_error(L"libOVR: cannot create device manager");
        }
        return false;
    }

    void OVRDevice::close() {
        if(mDevice) {
            mDevice->Disconnect(mSensor);
            mSensorFusion.AttachToSensor(0);
            mDevice = 0;
            mSensor = 0;
        }
    }

    uint32 OVRDevice::getHResolution() const {
        return mHMDInfo.HResolution;
    }

    uint32 OVRDevice::getVResolution() const {
        return mHMDInfo.VResolution;
    }

    const OVR::HMDInfo& OVRDevice::getHMDInfo() const {
        return mHMDInfo;
    }

    const OVR::DeviceInfo& OVRDevice::getDeviceInfo() const {
        return mDeviceInfo;
    }

    OVR::SensorFusion& OVRDevice::getSensorFusion() {
        return mSensorFusion;
    }

    const OVR::SensorFusion& OVRDevice::getSensorFusion() const {
        return mSensorFusion;
    }
        
    OVR::Util::Render::StereoConfig& OVRDevice::getStereoConfig() {
        return mStereoConfig;
    }

    const OVR::Util::Render::StereoConfig& OVRDevice::getStereoConfig() const {
        return mStereoConfig;
    }

    void OVRDevice::OnMessage(const OVR::Message& message) {

    }


    void OVRUtils::OVRMatToMat(const OVR::Matrix4f& ovrMat, Matrix4& mat) {
        
        memcpy(mat.x, ovrMat.M, sizeof(float) * 16);
    }

    Matrix4 OVRUtils::OVRMatToMat(const OVR::Matrix4f& ovrMat) {
        Matrix4 mat;
        OVRMatToMat(ovrMat, mat);
        return mat;
    }

}