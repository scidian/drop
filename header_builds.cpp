//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      For building header only libraries
//
//


//####################################################################################
//##    STB Libraries
//####################################################################################
#define STB_IMAGE_IMPLEMENTATION
#include "3rd_party/stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rd_party/stb/stb_image_resize.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rd_party/stb/stb_image_write.h"


//####################################################################################
//##    Font Stash
//####################################################################################
#include <stdio.h>					// malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h>					// memset
#define FONTSTASH_IMPLEMENTATION	// Expands implementation
#include "3rd_party/fontstash/fontstash.h"

///#include <GLFW/glfw3.h>          // Or any other GL header of your choice.
#include <QOpenGLWidget>
#define GLFONTSTASH_IMPLEMENTATION	// Expands implementation
#include "3rd_party/fontstash/glfontstash.h"







