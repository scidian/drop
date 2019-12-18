//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_RECT_H
#define DR_RECT_H

// Forward Declarations
#include "3rd_party/glm/fwd.hpp"
class DrPoint;
class DrRectF;


//####################################################################################
//##    DrRect
//##        An int Rectangle Class
//############################
class DrRect
{

public:
    int     left;                   // As vec4, x
    int     top;                    // As vec4, y
    int     width;                  // As vec4, z
    int     height;                 // As vec4, w

    // Constructors
    DrRect();
    DrRect(int left_, int top_, int width_, int height_);
    DrRect(const DrPoint &top_left, const DrPoint &bottom_right);
    DrRect(const DrRect  &r);
    DrRect(const DrRectF &r);
    DrRect(const glm::vec4 &v);

    // Conversion
    DrRectF         toRectF();
    glm::vec4       toGlmVec4();

    // Getters
    DrPoint         topLeft();
    DrPoint         topRight();
    DrPoint         bottomLeft();
    DrPoint         bottomRight();

};



#endif // DR_RECT_H
