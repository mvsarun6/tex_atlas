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
#include <algorithm>

#include <filesystem>



//libpng and other c includes
extern "C"
{
  #include "png.h"
  #include <stddef.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdio.h>
  #include <math.h>
}

using namespace std;

typedef unsigned int uint_32;
typedef unsigned char uint_8;

#define METADATA_FILE "texture_atlas_metadata.txt"
#define TEX_ATLAS_NAME "texture_atlas.png"
#define BPP 4 //bytes per pixel

/*******************Class Imageinfo *************************************/

//Imageinfo class, holds all details about an input png file
class Imageinfo
{
   private:
      string filepath; //filepath of the input PNG
      uint_32 width;
      uint_32 height;
      bool docked; //0- not being placed into output atlas
      uint_32 xpos; //Valid only if docked=1, xpos in output atlas
      uint_32 ypos; //Valid only if docked=1, ypos in output atlas
 
   public:
      
      Imageinfo():
      filepath{"NA"}, width{0},height{0},docked{0},xpos{0},ypos{0}
      {
      }
      
      Imageinfo(string str, uint_32 w, uint_32 h):
      filepath{str}, width{w},height{h},docked{0},xpos{0},ypos{0}
      {
      }

      ~Imageinfo()=default;

      void setfilepath(string str)  {         filepath = str;  }
      void setwidth(uint_32 w)      {         width = w;       }
      void setheight(uint_32 h)     {         height = h;      }
      void setdocked()              {         docked = 1;      }
      void cleardocked()            {         docked = 0;      }
      void setxpos(uint_32 x)       {         xpos = x;        }
      void setypos(uint_32 y)       {         ypos = y;        }

      string getfilepath()       {         return filepath;    }
      uint_32 getwidth()         {         return width;       }
      uint_32 getheight()        {         return height;      }
      bool isdocked()            {         return docked;      }
      uint_32 getxpos()          {         return xpos;        }
      uint_32 getypos()          {         return ypos;        }

      //Extract png file name from file path
      string getfilename()       {   
         string tempstr =   filepath;  
         
         int pos=0;
         int i=0;
         for(auto it = tempstr.begin();it!=tempstr.end();it++)
         {
            if(*it=='\\' || *it=='//')
            {
               pos= i;
            }
            i++;
         } 
         tempstr.erase(tempstr.begin(),tempstr.begin()+pos+1);
         return tempstr;    
         }
      
      //Overload < for height comparison
      bool operator<(Imageinfo &obj2)
      {
         return (height<obj2.height);
      }

      //Overload < for height comparison
      bool operator==(Imageinfo &obj2)
      {
         return (height==obj2.height);
      }
};

//Display all PNG details of Imageinfo vector
void display(std::vector<Imageinfo> &obj)
{
    std::cout <<"\n-----display texture names:\n";
    for (auto &filen : obj)
    {
        std::cout << filen.getfilename() <<"\t\tw/h:"<<filen.getwidth() <<","<<filen.getheight() 
                  << "\t\tx/y:"<<filen.getxpos()<<","<<filen.getypos()<<"   dock:"<<filen.isdocked()<<std::endl;
    }
    //std::cout <<"-----display end-----\n";
}

//Get maximum height among the undocked images
int get_MaxUndockHeight(std::vector<Imageinfo> &obj)
{
   int maxh =0;
    for (auto &filen : obj)
    {
        if(!filen.isdocked() && maxh<filen.getheight())
        {
           maxh = filen.getheight();
        }
    }
    return maxh;
}


//reset docking info, xpos, ypos
void reset_dockingdata(std::vector<Imageinfo> &obj)
{
   int maxh =0;
    for (auto &filen : obj)
    {
        filen.setxpos(0);
        filen.setypos(0);
        filen.cleardocked();
    }
}

//Get maximum width PNG
int get_MaxWidth(std::vector<Imageinfo> &obj)
{
   int maxw =0;
    for (auto &filen : obj)
    {
        if(maxw<filen.getwidth())
        {
           maxw = filen.getwidth();
        }
    }
    return maxw;
}
/********************************************************/
typedef struct FreeSpace
{
      uint_32 xpos;
      uint_32 ypos;
      uint_32 width;
      uint_32 height;

      FreeSpace():
      xpos{0},ypos{0},width{0},height{0}
      {         
      }

      uint_32 area()
      {
         return width*height;
        }

        
      //Overload < for area comparison
      bool operator<(FreeSpace &obj2)
      {
         return ((width*height)<(obj2.width*obj2.height));
      }

      //Overload < for area comparison
      bool operator==(FreeSpace &obj2)
      {
         return ((width*height)==(obj2.width*obj2.height));
      }


      void display()
      {
         std::cout<<"\nFreespace display x:"<< xpos<<" y:"<<ypos<<" w:"<<width<<" h:"<<height<<"\n";
      }

}FreeSpace;


typedef struct ImgCanvas
{
  uint_32 width;
  uint_32 height;

  ImgCanvas():
  width{0},height{0}
  {
  }

  ~ImgCanvas()
  {
  }

  uint_32 get_length ()
  {
     return width*height;
  }

}ImgCanvas;

/********************************************************/


int store_image(std::vector<Imageinfo> &images, unsigned long buffaddr, int buffwidth,int buffheight)
{
    int ret =0;

    std::cout<<"\nImage processing inprogress....";
    for(auto &img: images)
    {
       
       uint_32 x = img.getxpos();
       uint_32 y = img.getypos();
    
          png_image image;
          /* Only the image structure version number needs to be set. */
          memset(&image, 0, sizeof image);
          image.version = PNG_IMAGE_VERSION;
    
          if(png_image_begin_read_from_file(&image, img.getfilepath().c_str() ))
          {
                   
             png_bytep buffer;
    
             /* Change this to try different formats!  If you set a colormap format
              * then you must also supply a colormap below.
              */
             image.format = PNG_FORMAT_RGBA;
             
             //std::cout<<"\n"<<img.getfilepath()<<" PNG image size : "<<PNG_IMAGE_SIZE(image)<<"\n";
    
             buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));
             if (buffer != NULL)
             {
                if (png_image_finish_read(&image, NULL/*background*/, buffer,
                   0/*row_stride*/, NULL/*colormap for PNG_FORMAT_FLAG_COLORMAP */))
                {

                   for(int i=0;i<img.getheight();i++)
                   {
                      //std::cout<<".";
                      //std::cout<<"Exit "<< i <<" : store_image";
                      memcpy((void *)      (buffaddr+(x+(buffwidth*y))*BPP) ,     (void *)buffer,     img.getwidth()*BPP  );
                      y++;
                      buffer=buffer+(img.getwidth()*BPP);   
                   }
                   ret=1;
    
                }
                else
                {
                   std::cout<<"Error : PNG to memory";
                }
    
             }
             else
             {
                std::cout<<"Error : PNG open";
             }
    
          }
          
          png_image_free(&image);
    }

    return ret;
      
}

void wait_for_keypress()
{
    int x;
    std::cout<<"\n\n\nTo exit program : Press any key and enter........";
    std::cin>>x;
}


//Create image data vector
std::vector<Imageinfo> imagefiles;

//Create Output image canvas
ImgCanvas Canvas;


#define MAX_TRY 10 //(- X to +X times)
/*
plot_docking():
Plot docking points(in imagefiles) for each input PNG's and find corresponding output canvas area based on input idx
*/
void plot_docking(int try_idx)
{
   
   //set image canvas width, Its fixed. Height can be varied
   uint_32 locCanW=0;
   uint_32 locCanH=0;
   int num_of_image_w =(ceil(sqrt(imagefiles.size()))-try_idx);
   if( num_of_image_w <=0)
   {
       return;
   }

   for(int i=0;i<num_of_image_w;i++)
   {
      locCanW+=imagefiles[i].getwidth();  //do not modify locCanW here after     
   }
   
   //make sure canvas width is within the png which has largest width
  if(locCanW<get_MaxWidth(imagefiles))
  {
     locCanW=get_MaxWidth(imagefiles);
  }

    //MAIN LOOP
   /* Dock all PNG files in appropriate position and update its xpos and ypos in image data vector
      No memory will be created at this moment for final atlas creation
      image canvas height will be increased if needed (note: width is fixed) */
   for(int i=0;i<imagefiles.size();i++) //Main For Loop 
   {
        int locx=0;
        int locy=Canvas.height;  
        std::vector<FreeSpace> CanvasFreespace; 
        
       //Get maximum height among the undocked images and add that to the canvas height
        locCanH = get_MaxUndockHeight(imagefiles);
        //Canvas.length += locCanW*locCanH*BPP;
        Canvas.width = locCanW; //do not modify
        Canvas.height += locCanH;


        //PART 1
        /* Reserve a row with MaxUndockHeight and place PNG's in order until it reached fixed width
           And also find largest free space in the canvas */
        while(locx<Canvas.width)
        {
            int updated=0;
            int widthavail = Canvas.width - locx;
            for(auto &img : imagefiles )
            {
            if(!img.isdocked())
            {
                if(img.getwidth()<=widthavail && locCanH>=img.getheight())
                {
                    img.setxpos(locx);
                    img.setypos(locy);
                    img.setdocked();
                    //std::cout<<"\n dockingimage ="<<img.getfilename();
                   // if((CanvasFreespace.area()*1)< (locCanH-img.getheight()) * (Canvas.width-locx) )
                       {
                           FreeSpace tempFS;
                           tempFS.xpos =  locx;
                           tempFS.ypos = img.getheight()+locy;
                           tempFS.width = Canvas.width-locx;
                           tempFS.height = Canvas.height - tempFS.ypos;

                           if(CanvasFreespace.size()==0)
                           {
                              CanvasFreespace.push_back(tempFS);
                           }
                           else if(tempFS.ypos < CanvasFreespace.back().ypos)
                           {
                              CanvasFreespace.push_back(tempFS);
                           }
                       }
                       locx+=img.getwidth();
                       updated=1;
                       break;
                   }
                }
            }
    
            if(updated==0)
            {
                break;
            }
        }//WHILE
    
        //CanvasFreespace.display();

 //  std::sort(CanvasFreespace.begin(),CanvasFreespace.end());
 //  std::reverse(CanvasFreespace.begin(),CanvasFreespace.end());


        //PART 2
   /* Place PNG's in already found free space in already resrved row */
     //std::cout<<"\n Top free sp size = "<<CanvasFreespace.size();
     for(int i=0;i<CanvasFreespace.size();i++)
     {
       // std::cout<<"\n below i ="<< i <<" free sp size = "<<CanvasFreespace.size();
        if(i==0)
        {
            locx = CanvasFreespace[i].xpos;
            locy = CanvasFreespace[i].ypos;
        }
        else
        {
           if(get_MaxUndockHeight(imagefiles)< (CanvasFreespace[i-1].ypos - CanvasFreespace[i].ypos))
           {
              if(locx<CanvasFreespace[i].xpos)
              {
               locx = CanvasFreespace[i].xpos;
              }
              locy = CanvasFreespace[i].ypos;
           }
           else if (locx<=CanvasFreespace[i].xpos || locy <=CanvasFreespace[i].ypos)
           {
               locx = CanvasFreespace[i].xpos;
               locy = CanvasFreespace[i].ypos;
           }
           else //if (locx>CanvasFreespace[i].xpos && locy > CanvasFreespace[i].ypos)
           {
               locy = CanvasFreespace[i].ypos;
           }
        }

        //std::cout<<"\nlocx and locy "<<locx<<" "<<locy<<"   for H "<<locCanH;

        while(locx<Canvas.width)
        {
            int updated=0;
            int widthavail = Canvas.width - locx;
            for(auto &img : imagefiles )
            {
                if(!img.isdocked())
                {
                    
                    if(img.getwidth()<=widthavail && Canvas.height>=(img.getheight()+locy))
                    {
                        img.setxpos(locx);
                        img.setypos(locy);
                        img.setdocked();
                        //std::cout<<"\n 2 : dockingimage ="<<img.getfilename();
                        {
                           FreeSpace tempFS;
                           tempFS.xpos =  locx;
                           tempFS.ypos = img.getheight()+locy;
                           tempFS.width = Canvas.width-locx;
                           tempFS.height = Canvas.height - tempFS.ypos;
                           if(CanvasFreespace.size()==0)
                           {
                              CanvasFreespace.push_back(tempFS);
                           }
                           else if(tempFS.ypos < CanvasFreespace.back().ypos)
                           {
                              CanvasFreespace.push_back(tempFS);
                           }
                        }
                        locx+=img.getwidth();
                        updated=1;
                        break;
                   }
                }
            }
    
            if(updated==0)
            {
                break;
             }
        }//WHILE  
     }
        
    }//mainforloop
}


int main(int argc, char* argv[])
{
    int ret;
    namespace fs = std::filesystem;

   /* Check if there is any argument provided
      If no argument provided, return program with error message
   */
   if(argc<2){
        std::cout<<"\nError : No path has been given, Exiting program....";
        wait_for_keypress();
        return 0;
    } 

   /* Extract file name to a c++ string */
   string fpath (argv[1]);

   if(!fs::is_directory(fpath))
   {
        std::cout<<"\nError : The given path is not valid....";
        wait_for_keypress();
        return 0;
   }

    
    //Get all available PNG file details
    for (const auto  &entry : fs::directory_iterator(fpath))
    {
        string tempfile = entry.path().string();        
        if( tempfile.compare( (tempfile.end()-4)-tempfile.begin(),4,".png") ==0  || tempfile.compare((tempfile.end()-4)-tempfile.begin(),4,".PNG")==0)
        {
            png_image image;
            /* Only the image structure version number needs to be set. */
            memset(&image, 0, sizeof image);
            image.version = PNG_IMAGE_VERSION;

            if(png_image_begin_read_from_file(&image, tempfile.c_str()))
            {
               Imageinfo tempimg(tempfile, image.width, image.height);
               imagefiles.push_back(tempimg);
               png_image_free(&image);
            }
            else
            {
               std::cout<<"Error - png file read failed : "<<tempfile<<std::endl;
            }
        }
    }

    //Check if there is PNG files available
    if(imagefiles.size()==0)
    {
       std::cout<<"ERROR : no png files found in the given folder\n";
       wait_for_keypress();
       return 0;
    }

   //display(imagefiles);

   //Sort PNG file based on its height
   std::sort(imagefiles.begin(),imagefiles.end());

   //Sort PNG files in ascending order (based on height) //overloaded < and == based on height
   std::reverse(imagefiles.begin(),imagefiles.end());

   //display(imagefiles);
   
   /* Create output image canvas   */

   //Find the minimum canvas area by looping different idx, from -MAX_TRY to +MAX_TRY times
   int min_canvas_area_index = 0;
   int min_length = 0;
   for(int idx_try_find_min_area = -MAX_TRY; idx_try_find_min_area <= MAX_TRY; idx_try_find_min_area++)
   {
       Canvas.height = 0;
       Canvas.width = 0;
       reset_dockingdata(imagefiles);
       
       //Get docking canvas area with new index
       plot_docking(idx_try_find_min_area);
       
       if((Canvas.get_length()!=0) && (min_length==0 || min_length > Canvas.get_length()) )
       {
         min_length = Canvas.get_length();
         min_canvas_area_index = idx_try_find_min_area;
       }  

     //  std::cout<<"\n min_canvas_area_index = "<<min_canvas_area_index<<" "<<Canvas.height<<" "<<Canvas.width;
  }
 
  //Set canvas size and imagefiles docking points with already found min_canvas_area_index
  Canvas.height = 0;
  Canvas.width = 0;
  reset_dockingdata(imagefiles);
  plot_docking(min_canvas_area_index);


    //display(imagefiles);
    
    //Allocate memory for the image canvas with earlier fixed width and final calculated height
    void *text_atlas_buff = malloc(Canvas.width*Canvas.height*BPP);
    //std::cout<<"\nAllocate "<<Canvas.width*Canvas.height*BPP<<" bytes";
    if(text_atlas_buff==NULL)
    {
       std::cout<<"ERROR : memory allocation failed\n";
       wait_for_keypress();
       return 0;
    }

    //Set all pixels of output image canvas with white color
    memset(text_atlas_buff,0xFF, Canvas.width*Canvas.height*BPP);
    
    //Place all the input PNG's into the image canvas buffer according to its x,y docking position which were calculated earlier
    if(!store_image(imagefiles, (unsigned long) text_atlas_buff,Canvas.width,Canvas.height))
    {
         std::cout<<"ERROR : store image\n";
         wait_for_keypress();
         free(text_atlas_buff);
         return 0;
    }

   
    //deduce output directory for output atlas png
    string outfile(argv[1]);
    fs::current_path(outfile);  
    fs::create_directories("out");

    outfile.append("/out/");
    outfile.append(TEX_ATLAS_NAME);
    std::cout<<"\n\nTexture atlas output path :"<<outfile<<"\n";
    
    //remove the atlas png, if it was already present in the directory
    fs::remove(outfile);
    
    display(imagefiles);
   
   //Convert the output buffer data into PNG file format and store it into output directory
    png_image exportimage;
    memset(&exportimage, 0, sizeof exportimage);
    exportimage.version = PNG_IMAGE_VERSION;
    png_image_begin_read_from_file(&exportimage, imagefiles[1].getfilepath().c_str());
    exportimage.height = Canvas.height;
    exportimage.width = Canvas.width;

    std::cout<<"\nImage processing inprogress....";
    if(png_image_write_to_file(&exportimage,outfile.c_str() , 0/*convert to 8bit*/, text_atlas_buff, 0/*stride*/, NULL/*col map*/) )
    {
        std::cout<<"\nOutput texture atlas saved at : "<<outfile;
        std::cout<<"\nWidth ="<<exportimage.width<<"  Height ="<<exportimage.height;
    }
    else
    { 
        std::cout<<"ERROR : png write\n";
    }
    free(text_atlas_buff);
    png_image_free(&exportimage);

    /********************Metadata Export*************************/

    //deduce output path for metadata
    string metadatafilename(fpath);
    metadatafilename.append("/out/");
    metadatafilename.append(METADATA_FILE);

    //remove the metadata, if it was already present in the directory
    fs::remove(metadatafilename);

    //create/open the metadata file
   ofstream metafile (metadatafilename);
   if (!metafile.is_open())
   {
      std::cout <<"\nERROR : Unable to open file";
      wait_for_keypress();
      return 0;

   }

    //store the metadata from image data vector
   metafile << "Texture atlas name : "<<TEX_ATLAS_NAME<<"\n";
   metafile << "Width              : "<<Canvas.width<<"\n";
   metafile << "Height             : "<<Canvas.height<<"\n";   
   metafile << "Size               : "<<Canvas.width*Canvas.height<<" Pixels\n";
   metafile << "Number of Textures : "<<imagefiles.size()<<"\n";
   metafile<<"\n\n";

   for(int i=0;i<imagefiles.size();i++)
   {
      metafile<<"Texture "<<i<<":"<<"\n";
      metafile<<"{\n";
      metafile << "    Name   : "<<imagefiles[i].getfilename()<<"\n";
      metafile << "    width  : "<<imagefiles[i].getwidth()<<"\n";
      metafile << "    height : "<<imagefiles[i].getheight()<<"\n"; 
      metafile << "    size   : "<<imagefiles[i].getwidth()*imagefiles[i].getheight()<<"\n"; 
      metafile << "    xpos   : "<<imagefiles[i].getxpos()<<"\n";
      metafile << "    ypos   : "<<imagefiles[i].getypos()<<"\n";
      metafile<<"}\n"; 
   }

      metafile<<"\n"; 

      metafile.close();
      std::cout<<"\n\nMetadata exported at : "<<metadatafilename;

   std::cout<<"\n\n---PROGRAM ENDS---\n\n";

   
   imagefiles.clear();

   wait_for_keypress();

    return 0;
}

