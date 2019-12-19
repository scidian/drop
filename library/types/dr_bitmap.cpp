//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/stb/stb_image.h"
#include "library/dr_debug.h"
#include "library/types/dr_bitmap.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrBitmap::DrBitmap() {
    data.clear();
}

DrBitmap::DrBitmap(const DrBitmap &bitmap) {
    channels =  bitmap.channels;
    width =     bitmap.width;
    height =    bitmap.height;
    data.resize(width * height * c_number_channels);                                            // Resize data vector
    memcpy(&data[0], &bitmap.data[0], data.size());                                             // Copy data
}

// Create empty bitmap
DrBitmap::DrBitmap(int width_, int height_) {
    channels = c_number_channels;
    width = width_;
    height = height_;
    data.resize(width * height * c_number_channels);
    std::fill(data.begin(), data.end(), 0);
}

DrBitmap::DrBitmap(std::string filename) {
    loadFromFile(filename);
}

DrBitmap::DrBitmap(const unsigned char *from_data, const int &number_of_bytes, bool compressed, int width_, int height_) {
    loadFromMemory(from_data, number_of_bytes, compressed, width_, height_);
}


//####################################################################################
//##    Loading Images
//####################################################################################
void DrBitmap::loadFromFile(std::string filename) {
    // Load Image
    unsigned char* ptr = stbi_load(filename.data(), &width, &height, &channels, c_number_channels);

    // Error Check
    if (ptr == nullptr || width == 0 || height == 0) {
        Dr::PrintDebug("stbi_load_from_file error!"); width = 0; height = 0; return;
    }

    // Copy Image
    data.resize(width * height * c_number_channels);                                            // Resize data vector
    memcpy(&data[0], ptr, data.size());                                                         // Copy data
    stbi_image_free(ptr);                                                                       // Free the loaded pixels
}

void DrBitmap::loadFromMemory(const unsigned char *from_data, const int &number_of_bytes, bool compressed, int width_, int height_) {
    // Load Raw Data
    if (compressed == false) {
        width = width_;
        height = height_;
        data.resize(number_of_bytes);                                                           // Resize data vector
        memcpy(&data[0], from_data, number_of_bytes);                                           // Copy data

    // Load Image
    } else {
        const stbi_uc *compressed_data = reinterpret_cast<const stbi_uc*>(from_data);
        unsigned char* ptr = stbi_load_from_memory(compressed_data, number_of_bytes, &width, &height, &channels, 4);

        // Error Check
        if (ptr == nullptr || width == 0 || height == 0) {
            Dr::PrintDebug("stbi_load_from_memory error!"); width = 0; height = 0; return;
        }

        // Copy Image
        data.resize(width * height * c_number_channels);                                            // Resize data vector
        memcpy(&data[0], ptr, data.size());                                                         // Copy data
        stbi_image_free(ptr);                                                                       // Free the loaded pixels
    }
}





















