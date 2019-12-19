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
    ~DrBitmap();
    DrBitmap(const DrBitmap &bitmap);
    DrBitmap(int width_, int height_);
    DrBitmap(std::string filename);
    DrBitmap(const unsigned char *from_data, const int &number_of_bytes,
             bool compressed = true, int width_ = 0, int height_ = 0);

    // Image Loaders
    void    loadFromFile(std::string filename);
    void    loadFromMemory(const unsigned char *compressed_data, const int &number_of_bytes,
                           bool compressed = true, int width_ = 0, int height_ = 0);

    // Functions
    bool    isValid() const     { return (width > 0 && height > 0); }

};


#endif // DR_BITMAP_H
















