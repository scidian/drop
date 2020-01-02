//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/stb/stb_image.h"
#include "3rd_party/stb/stb_image_resize.h"
#include "3rd_party/stb/stb_image_write.h"
#include "core/dr_debug.h"
#include "core/types/dr_bitmap.h"
#include "core/types/dr_color.h"
#include "core/types/dr_rect.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrBitmap::DrBitmap() {  data.clear(); }
DrBitmap::~DrBitmap() { data.clear(); }

DrBitmap::DrBitmap(const DrBitmap &bitmap) {
    channels =  bitmap.channels;
    width =     bitmap.width;
    height =    bitmap.height;
    data.resize(width * height * bitmap.channels);                                              // Resize data vector
    memcpy(&data[0], &bitmap.data[0], data.size());                                             // Copy data
}

// Create empty bitmap
DrBitmap::DrBitmap(int width_, int height_) {
    channels =  c_number_channels;
    width =     width_;
    height =    height_;
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
//##    Manipulation
//####################################################################################
DrBitmap DrBitmap::copy() { return (*this); }

DrBitmap DrBitmap::copy(DrRect &copy_rect) {
    // Bounds checking
    if (copy_rect.right() > width - 1)   copy_rect.width =  width - copy_rect.left() - 1;
    if (copy_rect.bottom() > height - 1) copy_rect.height = height - copy_rect.top() - 1;

    // Create empty DrBitmap
    DrBitmap copy(copy_rect.width, copy_rect.height);

    // Copy source
    int source_x = copy_rect.left();
    for (int x = 0; x < copy.width; ++x) {
        int source_y = copy_rect.top();
        for (int y = 0; y < copy.height; ++y) {
            copy.setPixel(x, y, this->getPixel(source_x, source_y));
            source_y++;
        }
        ++source_x;
    }
    return copy;
}

DrRect DrBitmap::rect() const {
    return DrRect(0, 0, width, height);
}

DrColor DrBitmap::getPixel(int x, int y) const {
    size_t index = (y * this->width * channels) + (x * channels);
    return DrColor(data[index+2], data[index+1], data[index], data[index+3]);
}

// DrBitmap data is in the format provided by QImage (Format_ARGB32)
void DrBitmap::setPixel(int x, int y, DrColor color) {
    size_t index = (y * this->width * channels) + (x * channels);
    data[index]   = color.blue();
    data[index+1] = color.green();
    data[index+2] = color.red();
    data[index+3] = color.alpha();
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
        unsigned char* ptr = stbi_load_from_memory(compressed_data, number_of_bytes, &width, &height, &channels, c_number_channels);

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

// Aligns pixel format (stb ABGR vs QImage ARGB) for stbi_write
void DrBitmap::saveFormat(std::vector<unsigned char> &formatted) {
    formatted.resize(width * height * channels);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            DrColor pixel = getPixel(x, y);

            size_t index = (y * width * channels) + (x * channels);
            formatted[index] =   pixel.red();
            formatted[index+1] = pixel.green();
            formatted[index+2] = pixel.blue();
            formatted[index+3] = pixel.alpha();
        }
    }
}

int DrBitmap::saveAsBmp(std::string filename) {
    std::vector<unsigned char> formatted;
    saveFormat(formatted);
    int result = stbi_write_bmp(filename.c_str(), width, height, channels, formatted.data());
    return result;
}

int DrBitmap::saveAsJpg(std::string filename, int quality) {
    std::vector<unsigned char> formatted;
    saveFormat(formatted);
    int result = stbi_write_jpg(filename.c_str(), width, height, channels, formatted.data(), quality);
    return result;
}

int DrBitmap::saveAsPng(std::string filename) {
    std::vector<unsigned char> formatted;
    saveFormat(formatted);
    int result = stbi_write_png(filename.c_str(), width, height, channels, formatted.data(), width * channels);
    return result;
}



















