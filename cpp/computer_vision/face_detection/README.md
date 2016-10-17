#Face detection

**This tutorial assumes that RAPP API and OpenCV are installed.**

In the following document it's going to explain how to create your own OpenCV project
based in RAPP and how to do a proper CMakeLists.txt

Using CMake requires you to configure your CMakeLists.txt in the **root** of your RAPP project.
Essentially, the structure is the following:

```
project/
        CMakeLists.txt
        source/
                face_detection.cpp
        build/
```

##Writing the code

The first step is to write your code.
In this case, we created a project called `face_detection` with a folder `source` where we have our
example call `face_detection.cpp`.
You can see the complete example [here](source/face_detection.cpp).

In this example we are going to take images from a camera and we are going to use this image
to call the RAPP platform and look for faces.

For taking a image from our usb camera we have to use OpenCV:

```cpp
cv::VideoCapture camera(0); 
if(!camera.isOpened()) { 
    std::cout << "Failed to connect to the camera" << std::endl;
    return -1;
}
```

We created the parameter `camera` which is our `dev0`. You will have to check if your camera is
in the correct device and change the number if it's necessary. In the case that the program
don't recognise any devices, the program will close.

In other examples, we only see the result with a stdout. Now we would like to see the position
of the faces in the image that our camera is recording. So, we are going to create an OpenCV window:

```cpp
cv::namedWindow("Face detection", cv::WINDOW_AUTOSIZE);
```

The size of the window depends of your camera resolution.
At the same time, we can initialize our OpenCV matrix where we are going to save the image data
from the camera:

```cpp
cv::Mat frame;
```

At this point, we can continue our program like RAPP API examples.
We are going to initialize the platform information and the service controller, which is in charge
of make the cloud calls to the RAPP platform:

```cpp
rapp::cloud::platform info = {"rapp.ee.auth.gr", "9001", "rapp_token"}; 
rapp::cloud::service_controller ctrl(info);
```

One the parts that it's needed to make the face_detection call is to send a callback, 
so we are going to do one before the `for` loop because we don't need to create it in every loop:

```cpp
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
```

This callback shows how many faces have been found and, if there is any, 
it draws a blue rectangle in the image where the face is.

Before writing the `for loop` we have to keep in mind that we can't use a simple loop
to make the calls because we can **block the platform** if we don't stop sending calls. 
Because of that we have the example `rapp-api/cpp/examples/loop.cpp`, where wait a second 
for doing a call. However, we have the problem that `loop.cpp` example can't be use with
the window interface of OpenCV. So, in the case we want to use the OpenCV interface, we are
going to use a `std::chrono` object which is going to count the time between loops.
In this example, we are going to make a call every 500 ms.

```cpp
auto before = std::chrono::system_clock::now();
for (;;) {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - before).count(); 
    ...
}
```

Another issue that we can find is to create a `picture` object, which we need it to make the face_detection call,
because to create one it's needed a `ifstream`. This means, that `cv::Mat` we have to convert it in a correct way. 
The good news is OpenCV has a function which is perfect for this `cv::imencode`.
*To see more information, you can visit this web page: [cv::imencode](http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html).*
Then, every 500 ms we have to pass the parameter in this way:

```cpp
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
```

After having the `picture` object, we can make the call and then show in the OpenCV interface (`cv::imshow`).
However, the interface is not going to refresh the image until we use `cv::waitKey` function.
*To see more information you can visit this web site: [OpenCV interface](http://docs.opencv.org/2.4/modules/highgui/doc/user_interface.html).*

*NOTE: You'll have to add the propers headers at the begining of the file. If you have some doubts, you can see the complete example link above*

##Write the CMakeLists.txt

In this case it assumes that you have built your RAPP API in the **static** and **shared** libraries mode.

This file is going to be the same that we have in `helloworld/CMakeLists.txt` file.
We only have to add the OpenCV library and change the names of the project and executable.

*NOTE:* If you want to use only the **static** libraries, you can see `helloworld_static` project.

Then, the only lines that we have to add are:

```
find_package(OpenCV REQUIRED)

 ...

target_link_libraries(face_detection ${RAPP_LIBRARIES}
                                     ${OpenCV_LIBS})

```

##Repository detail

Before to do anything we have to be careful in the case that we are using a repository.
If you are not using with your project, then ignore this part.
In the case you are using one with Github, you will have to create a file call `.gitignore`
(inside your project directory) where you only write this:

```
build/
```

This means that you are not going to save this folder in the repository. It is good to do it
because in the case you shared your project, the other person won't have problems building it.
It's a specific folder for every user.

##Building your code

The next step is about create our `build` folder and run our code.

Now we are going to work in the terminal.

1. Go to your project path (in our case `face_detection/`)
2. Build your project
```
mkdir build
cd build 
cmake ..
```

3. If everything is ok, you will have created your executable `face_detection` in the folder build.
4. Run your executable
    ```
    ./face_detection
    ```

Now you can explore and make your own projects!
