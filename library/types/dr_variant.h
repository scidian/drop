//
//      Created by Stephens Nunnally on 12/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_VARIANT_H
#define DR_VARIANT_H

#include <string>

#include "boost/any.hpp"
#include "library/dr_containers.h"
#include "library/types/dr_point.h"
#include "library/types/dr_pointf.h"


//####################################################################################
//##    DrVariant
//##        Holds anything
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
    DrVariant&      operator=(const DrVariant &other);

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

    DrPoint                 toPoint();
    DrPointF                toPointF();

};


#endif // DR_VARIANT_H















