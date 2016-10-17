#Using RAPP and NAO with Qibuild

In this tutorial we'll run an RAPP API and NAOqi project in NAO robot build it with Qibuild.

**This tutorial we assume that RAPP API static is installed, Qibuild installed  and NAOqi C++ SDK is downloaded from Aldebaran webpage**

##Code

In this example we are going to use RAPP API and NAOqi C++ SDK (2.1.4 version).
The goal is to take a picture with NAO and use RAPP to look for faces in that image.

This example is based in the [NAO Getting an image example](http://doc.aldebaran.com/2-1/dev/cpp/examples/vision/getimage/getimage.html#cpp-tutos-get-image).
So, following it the first thing is to take the picture from NAO.
This is done with `showImages` function:

```cpp
void showImages(const std::string& robotIp, cv::Mat& rapp_image)
{
    AL::ALVideoDeviceProxy camProxy(robotIp, 9559);
    const std::string clientName = camProxy.subscribe("test", AL::kQVGA, AL::kBGRColorSpace, 30);
    cv::Mat imgHeader = cv::Mat(cv::Size(320, 240), CV_8UC3);
    AL::ALValue img = camProxy.getImageRemote(clientName);

    imgHeader.data = (uchar*) img[6].GetBinary();
    imgHeader.copyTo(rapp_image);

    camProxy.releaseImage(clientName);
    camProxy.unsubscribe(clientName);
}
```

In that function, if we want to save the image for using later we have to copied it in a variable. 

*Be careful!* You have to copy with `copyTo` function. If you only do an assingment, it'll be a segmentation fault because when the function finish, it deletes the data and your new object was pointing to empty data. 

Now we have the main part:

First, we'll take the IP of the robot with an argument. 

After that we can initialize the rapp platform and the callback which we need for making the call.
In this case, we are going to say how many faces we have found and ,in the case of finding one or more, we draw a rectangle in the face found and save the last in a file. 
In other way, we can't know what NAO is seeing.

```cpp

    rapp::cloud::platform info = {"155.207.19.229", "9001", "rapp_token"}; 
    rapp::cloud::service_controller ctrl(info);

    auto callback = [&](std::vector<rapp::object::face> faces) { 
        std::cout << "Found: " << faces.size() << " faces" << std::endl; 
        for(auto each_face : faces) {
            cv::rectangle(frame,
            cv::Point(each_face.get_left_x(), each_face.get_left_y()),
            cv::Point(each_face.get_right_x(), each_face.get_right_x()),
            cv::Scalar(255,0,0),
            1, 8, 0);
            cv::imwrite( "face.png", frame);
        }
    };
```

We are going to do a loop for taking the images and detect the faces.

*Be careful!* To avoid block the platform doing calls, we are going to add
a boost::chrono to do a call every 500 ms.

**NOTE: Avoid use std::chrono. It can't be use with NAO.**

```cpp

    auto before = boost::chrono::system_clock::now();
    for (;;) {
		auto now = boost::chrono::system_clock::now();
        auto elapsed = boost::chrono::duration_cast<boost::chrono::milliseconds>(now - before).count(); 
		if (elapsed > 500) {
        ...
        }
   }
```

The next step is to take the image from the `showImages` function:

```cpp
    try
    {
       showImages(robotIp, frame);
    }
    catch (const AL::ALError& e)
    {
        std::cerr << "Caught exception " << e.what() << std::endl;
    }
```

Once we have the image, we are going to use RAPP for detecting faces.

However, the type of data which OpenCV has is different of `rapp::object::picture` has.

Because of that we have to use the `cv::imencode` to fill a buffer with the image and read it after with 
the `picture` class.

```cpp
     if(!frame.empty()) {
        std::vector<int> param = {{ CV_IMWRITE_PNG_COMPRESSION, 3 }};
        cv::vector<uchar> buf;
        cv::imencode(".png", frame, buf, param);
        std::vector<rapp::types::byte> bytes(buf.begin(), buf.end());
        rapp::object::picture pic(bytes);

        before=now;
        ctrl.make_call<rapp::cloud::face_detection>(pic, true, callback);
    }
```

**NOTE:** When you use NAOqi SDK and OpenCV library from SDK you can't use `cv::imshow` to see the image. It's limited.

You can read more [here](http://doc.aldebaran.com/2-1/dev/cpp/examples/vision/opencv.html#cpp-tutos-opencv).

##CMakeLists

In this example, we have to deal with NAOqi libraries with `qibuild`.

`Qibuild` has its own language similar to cmake to build projects and most of the packages of NAOqi have been written for qibuild. 

```
cmake_minimum_required(VERSION 2.6)
project(face_detection)

find_package(qibuild)

find_package(rapp)
message(STATUS "libraries: ${RAPP_LIBRARIES}")
message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")

set(CMAKE_CXX_FLAGS "-std=gnu++1y -static-libstdc++")
qi_create_bin(face_detection "main.cpp")
qi_use_lib(face_detection ALCOMMON ALPROXIES RAPP openssl boost_system boost_thread pthread ALVISION OPENCV2_CORE OPENCV2_HIGHGUI)
```

You can follow the tutorials of `qibuild` to have the basic CMakeLists.

For RAPP API you have to look for the package of RAPP.
**NOTE:** You have to add a symbolic link in the NAOqi folder for RAPP library, includes and rapp-config.cmake.
In other way, you can't add to `qi_use_lib`.

To avoid to do this, you can follow the [face_detection  tutorial](../../face_detection/README.md) with CMake.

Like we are working with OpenCV, we have to add the libraries that NAOqi has for OpenCV.

At last, the `CMAKE_CXX_FLAGS`. 
**DON'T USE `-std=c++1y` or `-std=c++11`, etc. The correct flag is `-std=gnu++1y` or `-std=gnu++11`, etc.**
And making static libstdc++.
You can add the flags for optimizing Atom processors that we used in previous tutorials.

##Build and Run

Now we have to save and close the files, and we go to the terminal.

The instructions are the same that [qibuild with C++ SDK tutorials](http://doc.aldebaran.com/qibuild/beginner/qibuild/aldebaran.html).

```
qibuild configure -c cross-atom
qibuild make -c cross-atom
```

Finally, we have to move/copy our executable to NAO robot. 
Now you can run your example in NAO and see all the services that RAPP offers.
