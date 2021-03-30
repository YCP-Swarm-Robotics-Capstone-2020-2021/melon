#include "spinnakercamera.h"
#include <SpinGenApi/SpinnakerGenApi.h>
#include <spdlog/spdlog.h>

SpinnakerCamera::SpinnakerCamera(StateVariables& state) :
        AbstractCamera(state),
        m_psys(Spinnaker::System::GetInstance()),
        m_pcam(nullptr)
{
}

SpinnakerCamera::~SpinnakerCamera()
{
    m_psys->ReleaseInstance();
}

// Set a new value to an enumeration node's entry value
// Returns false if node value was unable to be set, true if node value was set successfully
bool set_node_val(Spinnaker::GenApi::INodeMap& node_map, const char* node_name, const char* value_name)
{
    // Get the node itself
    Spinnaker::GenApi::CEnumerationPtr node = node_map.GetNode(node_name);
    if(!Spinnaker::GenApi::IsAvailable(node) || !Spinnaker::GenApi::IsWritable(node))
    {
        spdlog::critical("Enumeration node '{}' is not available and/or writable", node_name);
        return false;
    }

    // Get the new value that should be set to the node
    Spinnaker::GenApi::CEnumEntryPtr value = node->GetEntryByName(value_name);
    if(!Spinnaker::GenApi::IsAvailable(value) || !Spinnaker::GenApi::IsReadable(value))
    {
        spdlog::critical("Enumeration value '{}' for enumeration node '{}' is not available and/or readable",
                         node_name, value_name);
        return false;
    }

    // Set the value to the node
    node->SetIntValue(value->GetValue());

    return true;
}

bool SpinnakerCamera::connect()
{
    // Get available cameras
    Spinnaker::CameraList clist = m_psys->GetCameras();
    spdlog::info("{} cameras found", clist.GetSize());
    if(clist.GetSize() < 1)
        return false;

    // Enumerate over cameras and attempt to find the camera with the given serial number
    // CameraList does have a GetBySerial() function, but it just seems to crash the program for some reason
    for(int i = 0; i < clist.GetSize(); ++i)
    {
        Spinnaker::CameraPtr pcam = clist.GetByIndex(i);
        Spinnaker::GenApi::INodeMap& node_map = pcam->GetTLDeviceNodeMap();
        Spinnaker::GenApi::CStringPtr pserial = node_map.GetNode("DeviceSerialNumber");
        if(Spinnaker::GenApi::IsAvailable(pserial) && Spinnaker::GenApi::IsReadable(pserial))
        {
            Spinnaker::GenICam::gcstring serial = pserial->GetValue();
            // TODO: Use serial number from StateVariables
            if(serial == "")
            {
                m_pcam = pcam;
                break;
            }
        }
    }

    if(m_pcam == nullptr)
    {
        // TODO: Insert serial number
        spdlog::critical("Camera with serial number '{}' not found", "");
        return false;
    }

    try
    {
        spdlog::info("Initializing camera");
        m_pcam->Init();
    }
    catch (Spinnaker::Exception& e)
    {
        spdlog::critical("Error initializing camera: \n{}", e.what());
        return false;
    }

    // Get camera nodes (settings)
    Spinnaker::GenApi::INodeMap& node_map = m_pcam->GetNodeMap();

    // TODO: Check return type and do something if false
    // Enable "Continuous" mode so that an unspecified amount of frames can be read continuously
    if(!set_node_val(node_map, "AcquisitionMode", "Continuous"))
    {
        spdlog::error("Failed to set camera 'AcquisitionMode' to 'Continuous'");
        return false;
    }
    // Set the pixel format of the incoming image to BGR8 so that it's compatible with OpenCV
    if(!set_node_val(node_map, "PixelFormat", "BGR8"))
    {
        spdlog::error("Failed to set camera 'PixelFormat' to 'BGR8'");
        return false;
    }

    // TODO: Disable heartbeat in debug? The example does this, but I'm not sure if it's referring to debugging
    //      the hardware in some way or just having the executable compiled in debug mode

    // Start video capture
    m_pcam->BeginAcquisition();

    return true;
}

bool SpinnakerCamera::disconnect()
{
    m_pcam->EndAcquisition();
    m_pcam->DeInit();
    m_pcam = nullptr;

    return true;
}

bool SpinnakerCamera::get_frame(cv::Mat& frame)
{
    try
    {
        // Get image from camera
        Spinnaker::ImagePtr img = m_pcam->GetNextImage();

        // Make sure the image is valid
        if(img->IsIncomplete())
        {
            spdlog::warn("Image incomplete: {}", img->GetImageStatus());
            return false;
        }

        // Convert the image to an OpenCV Mat
        cv::Mat cv_img = cv::Mat(img->GetHeight() + img->GetYPadding(), img->GetWidth() + img->GetXPadding(), CV_8UC3);
        cv_img.data = (uchar*)img->GetData();
        // Perform a deep copy of the Mat and assign it to 'frame'. A deep copy must be done otherwise the
        // 'data' pointer for the Mat will be invalid once the ImagePtr is freed
        frame = cv_img.clone();

        // Release the image
        img->Release();
    }
    catch(Spinnaker::Exception& e)
    {
        spdlog::critical("Error acquiring frame from spinnaker camera: \n{}", e.what());
    }

    return true;
}