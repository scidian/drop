//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "library/dr_debug.h"
#include "library/types/dr_image.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage() {
    data.clear();
}

DrImage::DrImage(const DrImage &image) {
    channels = image.channels;
    width = image.width;
    height = image.height;
    data.resize(width * height * c_number_channels);                                            // Resize data vector
    memcpy(&data[0], &image.data[0], data.size());                                              // Copy data
}


//####################################################################################
//##    Loading Images
//####################################################################################
void DrImage::loadFromFile(std::string filename) {
    // Load image
    unsigned char* ptr = stbi_load(filename.data(), &width, &height, &channels, c_number_channels);

    if (!ptr || !width || !height) { Dr::PrintDebug("stbi_load_from_memory error!"); }          // Error
    data.resize(width * height * c_number_channels);                                            // Resize data vector
    memcpy(&data[0], ptr, data.size());                                                         // Copy data
    stbi_image_free(ptr);                                                                       // Free the loaded pixels
}

void DrImage::loadFromMemory(const unsigned char *from_data, const int &number_of_bytes) {
    // Load image
    const stbi_uc *compressed_data = reinterpret_cast<const stbi_uc*>(from_data);
    unsigned char* ptr = stbi_load_from_memory(compressed_data, number_of_bytes, &width, &height, &channels, 4);

    if (!ptr || !width || !height) { Dr::PrintDebug("stbi_load_from_memory error!"); }          // Error
    data.resize(width * height * c_number_channels);                                            // Resize data vector
    memcpy(&data[0], ptr, data.size());                                                         // Copy data
    stbi_image_free(ptr);                                                                       // Free the loaded pixels
}





















