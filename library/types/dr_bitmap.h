//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_BITMAP_H
#define DR_BITMAP_H

#include <string>
#include <vector>

// Local constants
const int   c_number_channels = 4;


//####################################################################################
//##    DrBitmap
//##        Holds an image, compatible / loads with stb_image
//############################
class DrBitmap
{
public:
    int     channels =  c_number_channels;          // Number of 8-bit components per pixel: R, G, B, A
    int     width =     0;                          // Image width
    int     height =    0;                          // Image height

    std::vector<unsigned char> data;                // Pixel data


public:
    // Constructors
    DrBitmap();
    DrBitmap(const DrBitmap &bitmap);

    void    loadFromFile(std::string filename);
    void    loadFromMemory(const unsigned char *from_data, const int &number_of_bytes);

};


#endif // DR_BITMAP_H
















