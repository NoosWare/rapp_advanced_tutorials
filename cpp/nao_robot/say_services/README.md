#Nao with cmake files

In this tutorial we'll run an RAPP API and NAOqi project in NAO robot build it with CMake. 

**This tutorial we assume that RAPP API static is installed and NAOqi C++ SDK is downloaded from Aldebaran webpage**

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

In this example, we have to deal with NAOqi libraries without using `qibuild`.

`Qibuild` has its own language similar to cmake to build projects and most of the packages of NAOqi have been written for qibuild. 
Because of that we can't look for packages of Naoqi libraries. We have to look for the libraries manually:

```
set(NAOQI ${NAOQI} "$ENV{HOME}/naoqi-sdk-2.1.4.13-linux32/")
set(LIB_PATH ${LIB_PATH} "${NAOQI}/lib")
set(INCLUDE_PATH ${INCLUDE_PATH} "${NAOQI}/include")

find_library(ALPROXIES_LIBRARY NAMES alproxies HINTS ${LIB_PATH})
message(STATUS ${ALPROXIES_LIBRARY})
```

You'll have to change the path where you have the NAOqi SDK and the name of the version that you have.
The libraries from NAOqi that you'll need for this example are:

* alproxies
* alerror
* alcommon
* alvalue
* qi
* qitype

RAPP library you can look for it with the command `find_library` or `find_package`.
If you use the first option, you have to add the next line before start looking for it:

```
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
```

In the case you use `find_package` you only have to use `RAPP_STATIC_LIBRARY` param for the static one.

*Be careful with `Boost` version*.
 
Because if you are using in your computer a different version of `Boost 1.55`, you have to do static this library.
If you already have 1.55, then you can use the share one.
In our case, we were using Boost 1.54, so we wanted the static version.

You can set the libraries to put together in one parameter the libraries from NAOqi and the rest, so it's easiest for 
doing changes and more clear to link the libraries.

```
target_link_libraries(say_services ${RAPP_LIBRARY}
                                   ${NAO_LIBRARIES})
```
 
One important thing is to make static `gcc` and `libstdc++` because we are using a higher versions than NAO has in its system.

```
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-static-libstdc++ -static-libgcc")
```

At last, the `CMAKE_CXX_FLAGS`. 
**DON'T USE `-std=c++1y` or `-std=c++11`, etc. The correct flag is `-std=gnu++1y` or `-std=gnu++11`, etc.**
And we added some optimization flags especially for NAO which is based on Intel Atom processor.
*Availables since gcc 4.5*


##Build and Run

Now we have to save and close the files, and we go to the terminal.
In there we'll do the next steps:

```
mkdir build
cd build
cmake ..
make
```

After all, you'll have your executable in `build/` folder.

Finally, we have to move/copy our executable to NAO robot. 
Now you can run your example in NAO and see all the services that RAPP offers.
