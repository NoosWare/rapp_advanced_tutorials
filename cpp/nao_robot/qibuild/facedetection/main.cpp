// Aldebaran includes.
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

// Opencv includes.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
// RAPP API includes
#include <rapp/cloud/service_controller.hpp>
#include <rapp/cloud/vision_detection.hpp>
#include <rapp/objects/picture.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

/**
* \brief Shows images retrieved from the robot.
*
* \param robotIp the IP adress of the robot
*/
void showImages(const std::string& robotIp, cv::Mat& rapp_image)
{
    /** Create a proxy to ALVideoDevice on the robot.*/
    AL::ALVideoDeviceProxy camProxy(robotIp, 9559);

    /** Subscribe a client image requiring 320*240 and BGR colorspace.*/
    const std::string clientName = camProxy.subscribe("test", AL::kQVGA, AL::kBGRColorSpace, 30);

    /** Create an cv::Mat header to wrap into an opencv image.*/
    cv::Mat imgHeader = cv::Mat(cv::Size(320, 240), CV_8UC3);

    /** Retrieve an image from the camera.
    * The image is returned in the form of a container object, with the
    * following fields:
    * 0 = width
    * 1 = height
    * 2 = number of layers
    * 3 = colors space index (see alvisiondefinitions.h)
    * 4 = time stamp (seconds)
    * 5 = time stamp (micro seconds)
    * 6 = image buffer (size of width * height * number of layers)
    */
    AL::ALValue img = camProxy.getImageRemote(clientName);

    /** Access the image buffer (6th field) and assign it to the opencv image
    * container. */
    imgHeader.data = (uchar*) img[6].GetBinary();
    imgHeader.copyTo(rapp_image);

    /** Tells to ALVideoDevice that it can give back the image buffer to the
    * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
    camProxy.releaseImage(clientName);

    /** Cleanup.*/
    camProxy.unsubscribe(clientName);
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage 'getimages robotIp'" << std::endl;
        return 1;
    }

    const std::string robotIp(argv[1]);
    cv::Mat frame;

    /*
     * Construct the platform info setting the hostname/IP, port and authentication token
     * Then proceed to create a cloud controller.
     * We'll use this object to create cloud calls to the platform.
     */
    rapp::cloud::platform info = {"155.207.19.229", "9001", "rapp_token"}; 
    rapp::cloud::service_controller ctrl(info);

    /*
     * Construct a lambda, std::function or bind your own functor.
     * In this example we'll pass an inline lambda as the callback.
     * All it does is to show how many faces have been found and 
     * show a rectangle in the picture where is that face.
     */
    auto callback = [&](std::vector<rapp::object::face> faces) { 
        std::cout << "Found: " << faces.size() << " faces" << std::endl; 
        for(auto each_face : faces) {
            cv::rectangle(frame,
            cv::Point(each_face.get_left_x(), each_face.get_left_y()),
            cv::Point(each_face.get_right_x(), each_face.get_right_x()),
            cv::Scalar(255,0,0),
            1, 8, 0);
        }
    };

    /*
     * Call the function to take the image from NAO. It reads
     * from the camera of NAO and create a picture object with this 
     * image. After that we make the call to do the face detection
     */
    try
    {
       showImages(robotIp, frame);
    }
    catch (const AL::ALError& e)
    {
        std::cerr << "Caught exception " << e.what() << std::endl;
    }
    
    if(!frame.empty()) {
        std::vector<int> param = {{ CV_IMWRITE_PNG_COMPRESSION, 3 }};
        cv::vector<uchar> buf;
        cv::imencode(".png", frame, buf, param);
        std::vector<rapp::types::byte> bytes(buf.begin(), buf.end());
        rapp::object::picture pic(bytes);

        ctrl.make_call<rapp::cloud::face_detection>(pic, true, callback);
    }

    return 0;
}
