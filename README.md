This project is about:
------------------------
Generates texture atlas(texture_atlas.png) with metadata (texture_atlas_metadata.txt) by reading images from a given path as commandline argument.
Both texture_atlas and texture_atlas_metadata will be stored in folder "out" inside given input path.

The project is tested in both linux and windows.

Known issue : Image rotation can be implemented to further reduce output texture atlas PNG size


list of third-party dependencies
----------------------------
In this project libpng (version : v1.6.37). libpng has dependency with zlib, so zlib (version : v1.2.11) source files also used.




#######################################################################################

Folder Structure:
-----------------
1. bin* - Created during runtime with the output executable
2. build* - Created during runtime with build environment
3. libpng - Contains source files of libpng(version : v1.6.37) and zlib (version : v1.2.11)
4. src - application code (main.cpp)


#######################################################################################

Building instructions
------------------------
------------------------


Tools to be installed:
----------------------
It uses cmake (3.10 or above) and can be used with any build tools (Tested with ninja and mingw32-make)
1. gcc compiler package (Eg. install with Mingw)
2. cmake (3.10 or above)
3. Ninja (If using ninja build)
   or Can also use mingw32-make come with the Mingw
   or make 
   or Microsoft VS2017 or VS2019 (use appropriate argument in start.sh) eg : 
   or Can use any other build tool
   or 

Build Process:
-------------
It uses cmake and can be used with any build tools (Tested with ninja, make and mingw32-make)

Changes to be done by user:
1. In toolchain.cmake:
   -update GCCTOOLPATH with the gcc installation folder
   -update CMAKE_C_COMPILER and CMAKE_CXX_COMPILER
2. Update build tool in start.sh and build.sh according to your preferred build tool

build steps:
-------------

* step 1: Run start.sh (Sets up build environment) - Make, MS VS, Ninja and mingw32-make commands already provided in the script, enable your preferred build tool.
         Note 1: If MingW is installed, mingw32-make is already installed with
         Note 2: If using ninja build (Ninja to be installed, and ninja path must be added in environment variables)
         Note 3: If using MS VS build (MS VS to be installed, and update start.sh appropriately)
         when running this script, folder 'build' will be automatically created with build environment.


* step 2: Run build.sh (Ninja, Make and mingw32-make commands already provided in the script, enable your preferred build tool.)
         Preferred build command will be run from the folder 'build' and output executable will be created in bin folder

         1. -> Compiles application (main.cpp) with libpng source files to create final application executable
* step 2 ( for MS VS) : sln file will be generated after start.sh, can build the project by opening it.

* step 3: Run the created executable (in bin folder) with the path where PNG files are stored as argument


#######################################################################################
