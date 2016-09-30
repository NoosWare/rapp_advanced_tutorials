#Nao with cmake files

This project and `helloworld_static` have been created to do the same 
purpose than the tutorial written in `README.md` which you can find in `/nao_robot`.

The difference is `nao_turorial` use directly the compiler gcc with 
the necessary flags to compile, and `helloworld_static` and `helloworld` 
are doing the same but using CMake.

`helloworld_static` is for having all the libraries static and `helloworld`
has only static libgcc libc++ and rapp.
