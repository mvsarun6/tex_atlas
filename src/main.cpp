/**
  ******************************************************************************
  * File Name          : main.cpp
  * Description        : 
  ******************************************************************************
  * @attention
  *
  * Copyright (C) xxxxx.  All Rights Reserved.
  *
  * Liscence info
  *
  ******************************************************************************
  */
#include <iostream>
#include <string.h>
#include <fstream> //for fstream
#include <vector>
#include <sstream> //for stringstream
#include <algorithm> //for find, count_if



//libpng
extern "C"
{
  #include "png.h"
  #include <stddef.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdio.h>
}

using namespace std;

void wait_for_keypress()
{
    int x;
    std::cout<<"\n\n\nPress any key and enter to exit........";
    std::cin>>x;
}

int main(int argc, char* argv[])
{
int ret;

//std::cout<<"\n\nC++ version = "<<__cplusplus<<std::endl;

/* Check if there is any argument provided
   If no argument provided, return program with error message
*/
if(argc<2){
    std::cout<<"\nError : No path has been given, Exiting program....";
    wait_for_keypress();
    return 0;
}
//std::cout<<"Arg count = "<<argc<<std::endl; //For debugging, to be deleted

/* Extract file name to a c++ string */
char pathcbuffer[255];
char *ptr = argv[1];
strcpy(pathcbuffer,ptr);
string fpath = pathcbuffer;

std::cout<<"\nFile to be read = "<<fpath<<std::endl; //print path, For debugging, to be deleted







      png_image image;

      /* Only the image structure version number needs to be set. */
      memset(&image, 0, sizeof image);
      image.version = PNG_IMAGE_VERSION;

   int result = 1;

      if (png_image_begin_read_from_file(&image, argv[1]))
      {

        std::cout<<"height ="<<image.height<<" width ="<<image.width<<std::endl;
         png_bytep buffer;

         /* Change this to try different formats!  If you set a colormap format
          * then you must also supply a colormap below.
          */
         image.format = PNG_FORMAT_RGBA;

         buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));

         if (buffer != NULL)
         {
            if (png_image_finish_read(&image, NULL/*background*/, buffer,
               0/*row_stride*/, NULL/*colormap for PNG_FORMAT_FLAG_COLORMAP */))
            {
               if (png_image_write_to_file(&image, argv[2],
                  0/*convert_to_8bit*/, buffer, 0/*row_stride*/,
                  NULL/*colormap*/))
                  result = 0;

               else
                  fprintf(stderr, "pngtopng: write %s: %s\n", argv[2],
                      image.message);
            }

            else
               fprintf(stderr, "pngtopng: read %s: %s\n", argv[1],
                   image.message);

            free(buffer);
         }

         else
         {
            fprintf(stderr, "pngtopng: out of memory: %lu bytes\n",
               (unsigned long)PNG_IMAGE_SIZE(image));

            /* This is the only place where a 'free' is required; libpng does
             * the cleanup on error and success, but in this case we couldn't
             * complete the read because of running out of memory and so libpng
             * has not got to the point where it can do cleanup.
             */
            png_image_free(&image);
         }
      }

wait_for_keypress();
return 0;
}
