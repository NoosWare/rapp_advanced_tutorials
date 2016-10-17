#Using RAPP and NAO with Qibuild

In this tutorial we'll run an RAPP API and NAOqi project in NAO robot build it with Qibuild.

**This tutorial we assume that RAPP API static is installed, Qibuild installed  and NAOqi C++ SDK is downloaded from Aldebaran webpage**

##Code

In this example we are going to see the available services which RAPP offers and NAO is going to say every one.

We are going to use NAOqi and RAPP libraries: 

```cpp
#include <rapp/cloud/service_controller.hpp>
#include <rapp/cloud/available_services.hpp>
#include <iostream>
#include <alerror/alerror.h>
#include <alproxies/altexttospeechproxy.h>

int main()
{
    rapp::cloud::platform info = {"rapp.ee.auth.gr", "9001", "rapp_token"}; 
    rapp::cloud::service_controller ctrl(info);
    AL::ALTextToSpeechProxy tts("127.0.0.1", 9559);
    
    int i = 0;
    auto cb = [&](std::vector<std::pair<std::string, std::string>> services) {
         std::cout << "available services: " << std::endl;
         for (const auto & pair : services) {
            std::cout << pair.first << " " << pair.second << std::endl;
            tts.say(pair.first);
         }   
    };
    ctrl.make_call<rapp::cloud::available_services>(cb);

    exit(0);
}
```

The example is very similar to `helloworld`. However, we have a difference, which is the function of NAO to
speak:

```cpp
AL::ALTextToSpeechProxy tts("127.0.0.1", 9559);
```

First, we connect with the robot.
You'll have to change the IP of the robot if you want to execute remotely from NAO.

And second, we indicate what it has to say, in this case, the name of the services not the urls:

```cpp
tts.say(pair.first);
```


##CMakeLists

In this example, we have to deal with NAOqi libraries with `qibuild`.

`Qibuild` has its own language similar to cmake to build projects and most of the packages of NAOqi have been written for qibuild. 

```
cmake_minimum_required(VERSION 2.8)
project(helloworld)

find_package(qibuild)

set(CMAKE_CXX_FLAGS 
"-std=gnu++1y -static-libstdc++ -march=atom -mtune=atom -mfpmath=sse")

find_package(rapp)
message(STATUS "libraries: ${RAPP_LIBRARIES}")
message(STATUS "headers: ${RAPP_INCLUDE_DIRS}")

qi_create_bin(helloworld "main.cpp")
qi_use_lib(helloworld ALCOMMON ALPROXIES openssl RAPP boost_system boost_thread pthread)
```

You can follow the tutorials of `qibuild` to have the basic CMakeLists.

For RAPP API you have to look for the package of RAPP.
**NOTE:** You have to add a symbolic link in the NAOqi folder for RAPP library, includes and rapp-config.cmake.
In other way, you can't add to `qi_use_lib`.

To avoid to do this, you can follow the [say_services tutorial](../../say_services/README.md) with CMake.

At last, the `CMAKE_CXX_FLAGS`. 
**DON'T USE `-std=c++1y` or `-std=c++11`, etc. The correct flag is `-std=gnu++1y` or `-std=gnu++11`, etc.**
And we added some optimization flags especially for NAO which is based on Intel Atom processor.
*Availables since gcc 4.5*

##Build and Run

Now we have to save and close the files, and we go to the terminal.

The instructions are the same that [qibuild with C++ SDK tutorials](http://doc.aldebaran.com/qibuild/beginner/qibuild/aldebaran.html).

```
qibuild configure -c cross-atom
qibuild make -c cross-atom
```

Finally, we have to move/copy our executable to NAO robot. 
Now you can run your example in NAO and see all the services that RAPP offers.
