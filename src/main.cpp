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


wait_for_keypress();
return 0;
}
