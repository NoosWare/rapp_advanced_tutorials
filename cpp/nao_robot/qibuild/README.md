#Create a project with Qibuild for NAO

**TODO: Finish and write better**

First, we create the workspace. In our case we create `computer_vision/qi_wrokspace/`.
Second, we get into the folder created and we use the next command: 

```
qibuild init
```

After that, we create the folder of our project:

```
qisrc create name_of_the_folder
```

A folder with that name has been created and inside there are a CMakeLists.txt, main.cpp, qiproject.xml and a test.cpp
The next steps are necessary for compile the project with the toolchain of NAOqi:

```
qitoolchain create cross-atom /path/to/the/toolchain.xml
qibuild add-config cross-atom --toolchain cross-atom
```

The first command create the toolchain cross-atom and the second one, associate the toolchain with the build configuration.

...

For compile and build our project are the next commands:

```
qibuild configure -c cross-atom
qibuild make -c cross-atom 
```
