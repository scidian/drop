//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/glm/vec4.hpp"
#include "library/dr_math.h"
#include "library/types/dr_point.h"
#include "library/types/dr_rect.h"
#include "library/types/dr_rectf.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrRect::DrRect() {
    left =      0;
    top =       0;
    width =     0;
    height =    0;
}
DrRect::DrRect(int left_, int top_, int width_, int height_) {
    left =      left_;
    top =       top_;
    width =     width_;
    height =    height_;
}
DrRect::DrRect(const DrPoint &top_left, const DrPoint &bottom_right) {
    left =      top_left.x;
    top =       top_left.y;
    width =     (bottom_right.x - top_left.x);
    height =    (bottom_right.y - top_left.y);
}
DrRect::DrRect(const DrRect &r) {
    left =      r.left;
    top =       r.top;
    width =     r.width;
    height =    r.height;
}
DrRect::DrRect(const DrRectF &r) {
    left =      static_cast<int>(r.left);
    top =       static_cast<int>(r.top);
    width =     static_cast<int>(r.width);
    height =    static_cast<int>(r.height);
}
DrRect::DrRect(const glm::vec4 &v) {
    left =      static_cast<int>(v.x);
    top =       static_cast<int>(v.y);
    width =     static_cast<int>(v.z);
    height =    static_cast<int>(v.w);
}


//####################################################################################
//##    Conversion
//####################################################################################
DrRectF DrRect::toRectF()               { return DrRectF(*this); }
glm::vec4 DrRect::toGlmVec4()           { return glm::vec4(left, top, width, height); }


//####################################################################################
//##    Getters
//####################################################################################
int     DrRect::right()                 { return (left + width - 1); }
int     DrRect::bottom()                { return (top + height - 1); }
DrPoint DrRect::topLeft()               { return DrPoint(left, top); }
DrPoint DrRect::topRight()              { return DrPoint(left + width - 1, top); }
DrPoint DrRect::bottomLeft()            { return DrPoint(left, top + height - 1); }
DrPoint DrRect::bottomRight()           { return DrPoint(left + width - 1, top + height - 1); }








