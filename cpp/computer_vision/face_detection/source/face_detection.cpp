/**
 * Copyright 2015 RAPP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * #http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <opencv2/opencv.hpp>
#include <rapp/cloud/service_controller.hpp>
#include <rapp/cloud/vision_detection.hpp>
#include <rapp/objects/picture.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

#include <functional>
#include <iostream>
#include <chrono>

/*
 * \brief Example of face_detection showing the result in
 *  a opencv interface.
 */
int main()
{
    /* 
     * Initialization of the camera.
     * If your device is not in dev0, you'll have to change to the correct one.
     */
    cv::VideoCapture camera(0); 
    if(!camera.isOpened()) { 
        std::cout << "Failed to connect to the camera" << std::endl;
        return -1;
    }
    /*
     * Create a window to see the result of the 
     * face detection in the pictures that we are taking.
     * And initialization of the matrix where we are
     * going to save the images of the camera
     */
    cv::namedWindow("Face detection", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;

    /*
     * Construct the platform info setting the hostname/IP, port and authentication token
     * Then proceed to create a cloud controller.
     * We'll use this object to create cloud calls to the platform.
     */
    rapp::cloud::platform info = {"rapp.ee.auth.gr", "9001", "rapp_token"}; 
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
     * We create a variable chrono to count the time.
     * It's going to be used for making the call every
     * 500 ms. If we don't do it, we could block the platform.
     * Other way, if you don't use the windows interface, it's to 
     * make the call like the `loop.cpp` example.
     */
	auto before = std::chrono::system_clock::now();

    /*
     * Infinite loop until we press a key.
     * All it does is every 500 ms is going to take the picture
     * from the camera and create a picture object with this 
     * image. After that we make the call to do the face detection
     */
    for (;;) {
		auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - before).count(); 

		if (elapsed > 500) {
			camera >> frame;

            std::vector<int> param = {{ CV_IMWRITE_PNG_COMPRESSION, 3 }};
            cv::vector<uchar> buf;
            cv::imencode(".png", frame, buf, param);
            std::vector<rapp::types::byte> bytes(buf.begin(), buf.end());
            auto pic = rapp::object::picture(bytes);

            before = now;
            ctrl.make_call<rapp::cloud::face_detection>(pic, true, callback);
            cv::imshow("Face detection", frame);
		}
		if (cv::waitKey(30) >= 0) {
			break;
		}
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    }
    return 0;
}
