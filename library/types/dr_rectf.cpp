//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/glm/vec4.hpp"
#include "library/dr_math.h"
#include "library/types/dr_pointf.h"
#include "library/types/dr_rect.h"
#include "library/types/dr_rectf.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrRectF::DrRectF() {
    left =      0.0;
    top =       0.0;
    width =     0.0;
    height =    0.0;
}
DrRectF::DrRectF(double left_, double top_, double width_, double height_) {
    left =      left_;
    top =       top_;
    width =     width_;
    height =    height_;
}
DrRectF::DrRectF(const DrPointF &top_left, const DrPointF &bottom_right) {
    left =      top_left.x;
    top =       top_left.y;
    width =     (bottom_right.x - top_left.x);
    height =    (bottom_right.y - top_left.y);
}
DrRectF::DrRectF(const DrRect &r) {
    left =      r.left;
    top =       r.top;
    width =     r.width;
    height =    r.height;
}
DrRectF::DrRectF(const DrRectF &r) {
    left =      r.left;
    top =       r.top;
    width =     r.width;
    height =    r.height;
}
DrRectF::DrRectF(const glm::vec4 &v) {
    left =      static_cast<double>(v.x);
    top =       static_cast<double>(v.y);
    width =     static_cast<double>(v.z);
    height =    static_cast<double>(v.w);
}


//####################################################################################
//##    Conversion
//####################################################################################
DrRect DrRectF::toRect()                { return DrRect(*this); }
glm::vec4 DrRectF::toGlmVec4()          { return glm::vec4(left, top, width, height); }


//####################################################################################
//##    Getters
//####################################################################################
DrPointF DrRectF::topLeft()             { return DrPointF(left, top); }
DrPointF DrRectF::topRight()            { return DrPointF(left + width, top); }
DrPointF DrRectF::bottomLeft()          { return DrPointF(left, top + height); }
DrPointF DrRectF::bottomRight()         { return DrPointF(left + width, top + height); }

















