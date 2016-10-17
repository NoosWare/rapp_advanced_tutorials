#Create a project with Qibuild for NAO

**This tutorial assumes that you have installed Qibuild and RAPP libraries**

This is a simple tutorial for creating a project with Qibuild. 

First, we create the workspace. In our case we create `helloworld/`.
Second, we get into the folder created and we use the next command: 

```
qibuild init
qisrc add helloworld
```

A folder with that name has been created and inside there are a CMakeLists.txt, main.cpp, qiproject.xml and a test.cpp

The next steps are necessary for compile the project with the toolchain of NAOqi.
The toolchain file it's necessary when cross-compiling with CMake.
The toolchain of NAOqi must be in the naoqi-sdk folder.

```
qitoolchain create cross-atom /path/to/the/toolchain.xml
qibuild add-config cross-atom --toolchain cross-atom
```

The first command create the toolchain cross-atom and the second one associate the toolchain with the build configuration.

...

For compile and build our project are the next commands:

```
qibuild configure -c cross-atom
qibuild make -c cross-atom 
```

**NOTE:** When you use RAPP API with Aldebaran libraries you have to specify in the CMakeLists.txt the next command:

```
set(CMAKE_CXX_FLAGS "-std=gnu++1y")
```

Instead of `"-std=c++1y"`.

You'll find two examples:

* Helloworld: In this example NAO tell us the services that RAPP offers
* Face detection: In this example you can take a picture with NAO camera and use RAPP for detecting faces in that picture.
