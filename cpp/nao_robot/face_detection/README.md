#Face detection with NAO cameras

In this tutorial we are going to do a simple example using the camera of NAO for taking a picture and 
use RAPP API for detecting faces.

**NOTE:** This tutorial assumes that you install and build RAPP API with `-OPEN_NAO=ON` flag. 

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

We are going to use only `cmake`.

We start with the basic configuration:

```
    cmake_minimum_required(VERSION 2.8)
    project(face_detection)

    add_executable(face_detection main.cpp)
```
In this case, most of the libraries are inside to NAOqi SDK so we have to specify the library path. 

The first library we are going to use is RAPP. In this case we are using the static one with the variable
`${RAPP_STATIC_LIBRARIES}`.

```
    find_package(rapp REQUIRED)
    message(STATUS "libraries: ${RAPP_STATIC_LIBRARIES}")
```

To look for NAoqi libraries we have to specify manually the libraries and the paths:

```
    set(NAOQI ${NAOQI} "$ENV{HOME}/naoqi-sdk-2.1.4.13-linux32/")
    set(LIB_PATH ${LIB_PATH} "${NAOQI}/lib")
    set(INCLUDE_PATH ${INCLUDE_PATH} "${NAOQI}/include")

    find_library(ALPROXIES_LIBRARY NAMES alproxies HINTS ${LIB_PATH})
    message(STATUS ${ALPROXIES_LIBRARY})
    find_library(ALERROR_LIBRARY NAMES alerror HINTS ${LIB_PATH})
    message(STATUS ${ALERROR_LIBRARY})
    find_library(ALCOMMON_LIBRARY NAMES alcommon HINTS ${LIB_PATH})
    message(STATUS ${ALCOMMON_LIBRARY})
    find_library(ALVALUE_LIBRARY NAMES alvalue HINTS ${LIB_PATH})
    message(STATUS ${ALVALUE_LIBRARY})
    find_library(QI_LIBRARY NAMES qi HINTS ${LIB_PATH})
    message(STATUS ${QI_LIBRARY})
    find_library(QITYPE_LIBRARY NAMES qitype HINTS ${LIB_PATH})
    message(STATUS ${QITYPE_LIBRARY})
    find_library(OPENCV_CORE_LIBRARY NAMES opencv_core HINTS ${LIB_PATH})
    message(STATUS ${OPENCV_CORE_LIBRARY})
    find_library(OPENCV_HIGHGUI_LIBRARY NAMES opencv_highgui HINTS ${LIB_PATH})
    message(STATUS ${OPENCV_HIGHGUI_LIBRARY})

    set(NAO_LIBRARIES ${ALPROXIES_LIBRARY}
                      ${ALCOMMON_LIBRARY}
                      ${ALERROR_LIBRARY}
                      ${ALVALUE_LIBRARY}
                      ${QI_LIBRARY}
                      ${QITYPE_LIBRARY}
                      ${OPENCV_CORE_LIBRARY}
                      ${OPENCV_HIGHGUI_LIBRARY})

```

With the `message` you can see where the library was found.

There are some libraries which are common to RAPP and Aldebaran. These one you can choose to use it from the system or from NAOqi too.
In this example we choose the NAoqi ones.

```
    find_library(Boost_SYSTEM NAMES boost_system HINTS ${LIB_PATH})
    find_library(Boost_THREAD NAMES boost_thread HINTS ${LIB_PATH})
    find_library(Boost_CHRONO NAMES boost_chrono HINTS ${LIB_PATH})
    set(Boost_LIBRARIES ${Boost_SYSTEM}
                        ${Boost_CHRONO}
                        ${Boost_THREAD})
    message(STATUS ${Boost_LIBRARIES})

    find_library(Threads NAMES pthread HINTS ${LIB_PATH})
    message(STATUS ${Threads})

    find_library(SSL NAMES ssl HINTS ${LIB_PATH})
    find_library(CRYPTO NAMES crypto HINTS ${LIB_PATH})
    set(OPENSSL_LIBRARIES ${SSL}
                          ${CRYPTO})
    message(STATUS ${OPENSSL_LIBRARIES})
```

To include the headers we have to include the directories. You can add those that you need:

```
    include_directories("/usr/include"
                        "/usr/local/include"
                        ${INCLUDE_PATH})
```

At this point you can link your libraries to your executable:

```
    target_link_libraries(face_detection ${RAPP_STATIC_LIBRARIES}
                                         ${Boost_LIBRARIES} 
                                         ${Threads}
                                         ${OPENSSL_LIBRARIES}
                                         ${NAO_LIBRARIES})
```

Finally, we are going to add some flags.
    
For example, in RAPP we are using higher compilers of c++ than g++4.8, then we are going to use the static libstdc++ to avoid future problems in NAO.
And because we are using c++14 we have to use the flag `std=gnu++1y`. 

**DON'T USE `std=c++1y`, `c++0x`, `c++11`, etc. They are not going to work with NAOqi**

##Execute the program

After saving our main.cpp and CMakeLists.txt we are going to build it:

```
    mkdir build
    cd build
    cmake ..
    make
```

If everything goes well, then you have an executable `face_detection` in your build folder.

You can execute remotely from your computer using the IP of your robot, or copy to NAO and 
use the local IP "127.0.0.1". 
