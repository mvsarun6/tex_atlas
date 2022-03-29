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

#include <filesystem>



//libpng
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
#define BPP 4

/*******************Class Imageinfo *************************************/
class Imageinfo
{
   private:
      string filepath;
      uint_32 width;
      uint_32 height;
      bool docked;
      uint_32 xpos;
      uint_32 ypos;
 
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
      void setxpos(uint_32 x)       {         xpos = x;        }
      void setypos(uint_32 y)       {         ypos = y;        }

      string getfilepath()       {         return filepath;    }
      uint_32 getwidth()         {         return width;       }
      uint_32 getheight()        {         return height;      }
      bool isdocked()            {         return docked;      }
      uint_32 getxpos()          {         return xpos;        }
      uint_32 getypos()          {         return ypos;        }

      
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
      

      bool operator<(Imageinfo &obj2)
      {
         return (height<obj2.height);
      }
      bool operator==(Imageinfo &obj2)
      {
         return (height==obj2.height);
      }
};


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

      void display()
      {
         std::cout<<"\nFreespace display x:"<< xpos<<" y:"<<ypos<<" w:"<<width<<" h:"<<height<<"\n";
      }

}FreeSpace;


typedef struct ImgCanvas
{
  size_t length;
  uint_32 width;
  uint_32 height;

  ImgCanvas():
  length{0},width{0},height{0}
  {
  }

  ~ImgCanvas()
  {
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

   std::vector<Imageinfo> imagefiles;


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

    if(imagefiles.size()==0)
    {
       std::cout<<"ERROR : no png files found in the given folder\n";
       wait_for_keypress();
       return 0;
    }

   //display(imagefiles);
   std::sort(imagefiles.begin(),imagefiles.end());
   std::reverse(imagefiles.begin(),imagefiles.end());
   //display(imagefiles);
    
   ImgCanvas Canvas;

   int NoProcImg =0;
   uint_32 locCanW=0;
   uint_32 locCanH=0;
   for(int i=0;i<ceil(sqrt(imagefiles.size()));i++)
   {
      locCanW+=imagefiles[i].getwidth();  //do not modify locCanW here after     
   }

   for(int i=0;i<imagefiles.size();i++) //Main For Loop
   {
        int locx=0;
        int locy=Canvas.height;  
        FreeSpace CanvasFreespace; 
        
        locCanH = get_MaxUndockHeight(imagefiles);
        Canvas.length += locCanW*locCanH*BPP;
        Canvas.width = locCanW; //do not modify
        Canvas.height += locCanH;
        //std::cout<<"\ncanvas size ="<<Canvas.length<<" canvas width ="<<Canvas.width<<" canvas height ="<<Canvas.height;
        
        //PART 1
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
                    if((CanvasFreespace.area()*1.5)< (locCanH-img.getheight()) * (Canvas.width-locx) )
                       {
                           CanvasFreespace.xpos =  locx;
                           CanvasFreespace.ypos = img.getheight();
                           CanvasFreespace.width = Canvas.width-locx;
                           CanvasFreespace.height = Canvas.height - CanvasFreespace.ypos;
                       }
                       locx+=img.getwidth();
                       NoProcImg++;
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
    
        //PART 2            
        locx=CanvasFreespace.xpos;
        locy=CanvasFreespace.ypos;   
        while(locx<Canvas.width)
        {
            int updated=0;
            int widthavail = Canvas.width - locx;
            for(auto &img : imagefiles )
            {
                if(!img.isdocked())
                {
                    
                    if(img.getwidth()<=widthavail && locCanH>=(img.getheight()+locy))
                    {
                        img.setxpos(locx);
                        img.setypos(locy);
                        img.setdocked();
                        if((CanvasFreespace.area()*1.5)< (locCanH-img.getheight()) * (Canvas.width-locx) )
                        {
                            CanvasFreespace.xpos =  locx;
                            CanvasFreespace.ypos = img.getheight();
                            CanvasFreespace.width = Canvas.width-locx;
                            CanvasFreespace.height = Canvas.height - CanvasFreespace.ypos;
                        }
                        locx+=img.getwidth();
                        NoProcImg++;
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
    
    
    }//mainforloop
  
    //display(imagefiles);
    
    
    void *text_atlas_buff = malloc(Canvas.width*Canvas.height*BPP);
       std::cout<<"\nAllocate "<<Canvas.width*Canvas.height*BPP<<" bytes";
    if(text_atlas_buff==NULL)
    {
       std::cout<<"ERROR : memory allocation failed\n";
       wait_for_keypress();
       return 0;
    }
    memset(text_atlas_buff,0xFF, Canvas.width*Canvas.height*BPP);
    
    if(!store_image(imagefiles, (unsigned long) text_atlas_buff,Canvas.width,Canvas.height))
    {
         std::cout<<"ERROR : store image\n";
         wait_for_keypress();
         free(text_atlas_buff);
         return 0;
    }

   
   

    string outfile(argv[1]);
    fs::current_path(outfile);  
    fs::create_directories("out");

    outfile.append("/out/");
    outfile.append(TEX_ATLAS_NAME);
    std::cout<<"\n\nTexture atlas output path :"<<outfile<<"\n";

    fs::remove(outfile);
    
    display(imagefiles);

    png_image exportimage;
    /* Only the image structure version number needs to be set. */
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

    string metadatafilename(fpath);
    metadatafilename.append("/out/");
    metadatafilename.append(METADATA_FILE);
    fs::remove(metadatafilename);

   //std::cout<<"\n"<<metadatafilename;
   ofstream metafile (metadatafilename);
   if (!metafile.is_open())
   {
      std::cout <<"\nERROR : Unable to open file";
      wait_for_keypress();
      return 0;

   }

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

      wait_for_keypress();

    return 0;
}
