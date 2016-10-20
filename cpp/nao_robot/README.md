# README


## NAOqi

Aldebaran's OPEN NAO version is: Gentoo 2.6.33.9-rt31-aldebaran-rt
It runs on an i686 Intel(R) Atom(TM) CPU Z530 @ 1.60GH (although that depends on your robot model).
It *appears* to support C++11/14 but it uses older versions of GLIBC.

## RPATH

OPEN Nao has a file `/home/nao/naoqi/preferences/autoload.ini` in which you can set the modules/libraries to be loaded.

```
# autoload.ini
#
# Use this file to list the cross-compiled modules that you wish to load.
# You must specify the full path to the module, python module or program.

[user]
#the/full/path/to/your/liblibraryname.so  # load liblibraryname.so

[python]
#the/full/path/to/your/python_module.py   # load python_module.py

[program]
#the/full/path/to/your/program            # load program
```

In the above example, we can use the *user* mode to load a library.
However, OPEN Nao does **NOT** respect the RPATH for `librapp.so` meaning
that when librapp uses RPATH to define where it should load its own libraries from,
it is ignored.

## System Libraries

Most of NAO's system libraries are under `/usr/lib/*` (such as boost, ssl and crypto) 
In detail:

* libboost_system.so.1.55.0 (libc6) => /usr/lib/libboost_system.so.1.55.0
* libssl.so.1.0.0 (libc6) => /usr/lib/libssl.so.1.0.0
* libcrypto.so.1.0.0 (libc6) => /usr/lib/libcrypto.so.1.0.0
* libstdc++.so => /usr/lib/libstdc++.so.6.0.14
* libc.so.6 (libc6, OS ABI: Linux 2.6.9) => /lib/libc.so.6

If we take a peek at the `libstdc++` we can see that:

* it supports up to `GLIBCXX_3.4.14`
* it supports up to `GLIBC_2.3`

Unfortunatelly, C++11/14 and the modern compilers (gcc-4.9, g++-4.9) have higher ABI's:

* for C++ we have `GLIBCXX_3.4.20`
* for C (used by boost and ssl) we have `GLIBC_2.4`

## ATOM CPU flags and optimisation

For ATOM (NAO) use full atom flags:

1. base: `-O2 -ffast-math -mfpmath=sse -m32 -march=atom`
2. base++: `-O2 -ffast-math -mfpmath=sse -ftree-loop-if-convert -m32 -march=atom`
3. peak: `-Ofast -funroll-loops -mfpmath=sse -m32 -march=atom`, for 4.4 and 4.5 versions `-Ofast` is replaced with `-O3 -ffast-math`

## Illegal Instruction from *libboost_system1.55.so*

There is an *illegal instruction* coming from *epoll* related to `boost::asio` from `libboost_system1.55.so`.
This is most likely because it comtains an instruction which will not work on ATOM CPU.
If we want to static link with boost we must either: 

* build libboost for ATOM cpu (regardless of version) and then static link against it
* or use the libraries from the robot (which have already been built for ATOM)

## How To Setup

This tutorial assumes that you have all dependencies installed, 
and that you already built the librapp.a as a static library using:

```
git clone https://github.com/rapp-project/rapp-api.git
mkdir -p rapp-api/cpp/build && cd rapp-api/cpp/build
cmake .. -DOPEN_NAO=ON
```

Copy the produced static library under your `lib/` directory.

## Static Linking

This serves as a reference point to building the rpath inside an executable.
It probably won't be enough for GLIBC ABI miss-match but it can be used to specify **which** libraries to load from **where**.

```{r, engine='bash', count_lines}
g++ main.cpp -o app.static --std=c++14 -I/usr/local/include -L/urs/lib/i386-linux-gnu -static -lboost_system -lssl -lcrypto -lpthread -ldl -L lib -lrapp -Os -s -O2 -march=atom -mtune=atom -mfpmath=sse 
```

This will package all libraries within a single executbale.
The downside is that this executable is large: 3.1Mb!

1. Can we ship on NAO the libc, libstdc++, libboost_system, libssl and libcrypto? Is that enough?
2. Alternatively, can an old VM (old Kernel, GLIBC, etc) be used to build the library and executables?
3. Maybe an old VM (Ubuntu 12.XX) would be of more use, and I'd have to build GCC from scratch in there.


Static linking is the easy and dirty way out: we bundle with our app everything: boost, ssl, stdc++, libc.
This on its own accord creates certain issues:

* libc requires at runtime the `dl` (dlopen, dlsym) often seen as `-ldl` even if we statically link
* static link to libc disables `getaddressbyhostname` and disables some network functionality.
* one way around this, is to use musl-libc: http://git.musl-libc.org/cgit/musl
* another way is to rely on absolute IP addresses

Here we use:

* a static `libboost_system` (we have copied the version of libboost_system from NAO to our machine)
* a static `librapp` which we created earlier using the `-DOPEN_NAO=ON` flag
* a static `libpthread` from our machine
* a static `libsll` and `libcrypto` because there is a miss-match in the ABI on the robot
* a static `libdl` (dlopen, etc.)
* all static `libgcc_s`, `libstdc++` and `libc`

As a result we get a warning:

```
/usr/lib/gcc/i686-linux-gnu/4.9/../../../i386-linux-gnu/libcrypto.a(dso_dlfcn.o): In function `dlfcn_globallookup':
(.text+0xa): warning: Using 'dlopen' in statically linked applications requires at runtime the shared libraries from the glibc version used 
for linking
lib/librapp.a(asio_http.cpp.o): In function `boost::asio::detail::resolve_op<boost::asio::ip::tcp, boost::_bi::bind_t<void, boost::_mfi::mf2
<void, rapp::cloud::asio_http, boost::system::error_code, boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> >, boost::_bi::list
3<boost::_bi::value<rapp::cloud::asio_http*>, boost::arg<1> (*)(), boost::arg<2> (*)()> > >::do_complete(boost::asio::detail::task_io_servic
e*, boost::asio::detail::task_io_service_operation*, boost::system::error_code const&, unsigned int)':
asio_http.cpp:(.text._ZN5boost4asio6detail10resolve_opINS0_2ip3tcpENS_3_bi6bind_tIvNS_4_mfi3mf2IvN4rapp5cloud9asio_httpENS_6system10error_co
deENS3_23basic_resolver_iteratorIS4_EEEENS5_5list3INS5_5valueIPSB_EEPFNS_3argILi1EEEvEPFNSL_ILi2EEEvEEEEEE11do_completeEPNS1_15task_io_servi
ceEPNS1_25task_io_service_operationERKSD_j[_ZN5boost4asio6detail10resolve_opINS0_2ip3tcpENS_3_bi6bind_tIvNS_4_mfi3mf2IvN4rapp5cloud9asio_htt
pENS_6system10error_codeENS3_23basic_resolver_iteratorIS4_EEEENS5_5list3INS5_5valueIPSB_EEPFNS_3argILi1EEEvEPFNSL_ILi2EEEvEEEEEE11do_complet
eEPNS1_15task_io_serviceEPNS1_25task_io_service_operationERKSD_j]+0xcb): warning: Using 'getaddrinfo' in statically linked applications requ
ires at runtime the shared libraries from the glibc version used for linking
```

This warning means two things:

1. We cannot use `getaddrinfo` which is used by `boost::asio::detail::resolve_op<>` 
2. Therefore we can only rely on IPs and not hostnames since resolution won't work

Possible solutions are:

1. Use IPs right away instead of hostnames (e.g., replace `rapp.auth.gr` with `155.207.19.229`)
2. Use an external DNS resolver library to get the actual IP address of the hostname (e.g., `UDNS`, `GNU adns`, etc...)
3. Communicate via pipes, stdmessages or some other way with the system, and get the address.
4. Mix static linking with dynamic linking (more on that later)

## Dynamic Linking

The conventional way is to use shared linraries which are dynamically loaded when executing.
You can try this by using:

```{r, engine='bash', count_lines}
g++ main.cpp -o app.dyn --std=c++14 -I/usr/local/include -L/urs/lib/i386-linux-gnu -lboost_system -lssl -lcrypto -lpthread -Wl,-rpath=/home/nao/.lib/ -L ../lib -lrapp -march=atom -mtune=atom -mfpmath=sse -s -Os -O2
```

*NOTE* here we have to set the RPATH to the shared librapp, else it won't be found.

The issues with this approach are numerous:

* Your application must be built using the same library versions or there will be an ABI miss-match.
* Your building platform (computer or VM) must have the same libc (GLIBC) version as the one on the robot
* Your building platform must have the same boost version as the one on the robot.

The advantages are:

* You don't have to rebuild anything for ATOM cpu, the shared libraries on your NAO are already optimised for that.
* Your binary will be considerably smaller since you're only shipping your code and not the libraries.

However, using RAPP API 0.7.0 when we do **not** statically link with boost, we will get miss-matched GLIBC ABI.

```
./app.dyn: /usr/lib/libcrypto.so.1.0.0: no version information available (required by ./app.dyn)
./app.dyn: /usr/lib/libstdc++.so.6: version `GLIBCXX_3.4.15' not found (required by ./app.dyn)
./app.dyn: /usr/lib/libssl.so.1.0.0: no version information available (required by ./app.dyn)
./app.dyn: /usr/lib/libcrypto.so.1.0.0: no version information available (required by /home/nao/.lib/librapp.0.7.so)
./app.dyn: /usr/lib/libstdc++.so.6: version `GLIBCXX_3.4.20' not found (required by /home/nao/.lib/librapp.0.7.so)
./app.dyn: /usr/lib/libstdc++.so.6: version `GLIBCXX_3.4.15' not found (required by /home/nao/.lib/librapp.0.7.so)
./app.dyn: /usr/lib/libstdc++.so.6: version `GLIBCXX_3.4.18' not found (required by /home/nao/.lib/librapp.0.7.so)
./app.dyn: /usr/lib/libssl.so.1.0.0: no version information available (required by /home/nao/.lib/librapp.0.7.so)
./app.dyn: /usr/lib/libssl.so.1.0.0: no version information available (required by /home/nao/.lib/librapp.0.7.so)
```

## Hybrid Mode = ON

You can try to mix and match static and dynamic libraries by using:

```{r, engine='bash', count_lines}
g++ main.cpp -o app.hybrid.tls --std=c++14 -I/usr/local/include -L/urs/lib/i386-linux-gnu -static-libstdc++ -static-libgcc -Wl,-Bstatic -lc -lssl -lcrypto -L lib -lrapp -Wl,-Bdynamic -lboost_system -lpthread -ldl -march=atom -mtune=atom -mfpmath=sse -s -Os -O2
```

We end up with a 2.5Mb executable which has mixed shared and static libraries.
In this instance we did:

* dynamic: `boost`, `pthreads` and `dl`
* static: `rapp`, `ssl`, `crypto`, `gcc`, `stdc++`

This still produces a large file which only relies on NAO's boost and threads.
If you do not *want* SSL/TLS or you want don't care about ABI issues, you can also do:

```{r, engine='bash', count_lines}
g++ main.cpp -o app.hybrid.no_tls --std=c++14 -I/usr/local/include -L/urs/lib/i386-linux-gnu -static-libstdc++ -Wl,-Bstatic -lrapp -lboost_system -Wl,-Bdynamic -lpthread -lc -lssl -lcrypto -ldl -march=atom -mtune=atom -mfpmath=sse -s -Os -O2
```

Now we end up with a 859Kb executable, a considerable improvement in size, but we have
sacrificed compatability with SSL/TLS.
In this case we did:

* dynamic: `pthreads`, `ssl`, `crypto`, `dl`
* static: `rapp`, `stdc++`, `boost`

The executable when run will warn you about ABI miss-match during runtime:

```
./app.mixed: /usr/lib/libcrypto.so.1.0.0: no version information available (required by ./app.mixed)
./app.mixed: /usr/lib/libssl.so.1.0.0: no version information available (required by ./app.mixed)
```

If you only use HTTP for communicating with the cloud, then there exists no problem (TLS won't be used).

## CMake and QIbuild

You can use CMake or qibuild in your projects.

| Tutorials | Tools | Link |
|-----------|-------|------|
|Helloworld | RAPP + CMake| [Helloworld](helloworld/)|
|Helloworld | RAPP + NAOqi + qibuild | [qiHelloworld](qibuild/helloworld/)|
|Helloworld Static | RAPP + CMake | [Helloworld static](helloworld_static/)|
|Face detection | RAPP + NAOqi + CMake| [Face detection](face_detection/)|
|Face detection| RAPP + NAOqi + qibuild | [qiFace detection](qibuild/facedetection/)|
|Say Services | RAPP + NAOqi + CMake| [Say services](say_services/)|
|           |       |   |

## Getting Help

You can contact us on Gitter: 

[![Join the chat at https://gitter.im/rapp-project/rapp-api](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rapp-project/rapp-api?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
