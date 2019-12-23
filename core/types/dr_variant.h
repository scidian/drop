//
//      Created by Stephens Nunnally on 12/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_VARIANT_H
#define DR_VARIANT_H

#include "boost/any.hpp"
#include "core/types/dr_color.h"
#include "core/types/dr_bitmap.h"
#include "core/types/dr_point.h"
#include "core/types/dr_pointf.h"
#include "core/types/dr_rect.h"
#include "core/types/dr_rectf.h"
#include "core/types/dr_vec2.h"
#include "core/types/dr_vec3.h"


//####################################################################################
//##    DrVariant
//##        Holds anything, mostly wrapper for boost::any
//############################
class DrVariant
{
public:
    boost::any m_value;                                       // Holds the variant value

public:
    // Constructors
    DrVariant() {}
    template<class T> DrVariant(T value) { m_value = value; }

    // Operator Overloads
    DrVariant&      operator=   (const DrVariant &other);

    // Getters
    std::string     getType() { return m_value.type().name(); }
    boost::any      value() const { return m_value; }

    // Returns
    bool                    toBool();
    double                  toDouble();
    float                   toFloat();
    int                     toInt();
    long                    toLong();
    unsigned int            toUInt();

    std::string             toString();

    std::deque<DrVariant>   toDeque();
    std::list<DrVariant>    toList();
    std::vector<DrVariant>  toVector();

    DrColor                 toColor();
    DrBitmap                toImage();
    DrPoint                 toPoint();
    DrPointF                toPointF();
    DrRect                  toRect();
    DrRectF                 toRectF();
    DrVec2                  toVec2();
    DrVec3                  toVec3();

};


#endif // DR_VARIANT_H















