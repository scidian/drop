//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_RECTF_H
#define DR_RECTF_H

// Forward Declarations
#include "3rd_party/glm/fwd.hpp"
class DrPointF;
class DrRect;


//####################################################################################
//##    DrRectF
//##        A double Rectangle Class
//############################
class DrRectF
{

public:
    double  left;                   // As vec4, x
    double  top;                    // As vec4, y
    double  width;                  // As vec4, z
    double  height;                 // As vec4, w

    // Constructors
    DrRectF();
    DrRectF(double left_, double top_, double width_, double height_);
    DrRectF(const DrPointF &top_left, const DrPointF &bottom_right);
    DrRectF(const DrRect  &r);
    DrRectF(const DrRectF &r);
    DrRectF(const glm::vec4 &v);

    // Conversion
    DrRect          toRect();
    glm::vec4       toGlmVec4();

    // Getters
    double          right();
    double          bottom();
    DrPointF        topLeft();
    DrPointF        topRight();
    DrPointF        bottomLeft();
    DrPointF        bottomRight();

};


#endif // DR_RECTF_H











