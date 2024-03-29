//
//      Created by Stephens Nunnally on 12/5/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/enums_engine.h"


namespace Dr {


//####################################################################################
//##    Returns a std::string respresentaiton of the passed Editor Enum
//####################################################################################
std::string StringFromBodyType(Body_Type type) {
    switch (type) {
        case Body_Type::Static:             return "Static";
        case Body_Type::Dynamic:            return "Dynamic";
        case Body_Type::Kinematic:          return "Kinematic";
    }
}

std::string StringFromShapeType(Shape_Type type) {
    switch (type) {
        case Shape_Type::Segment:           return "Segment";
        case Shape_Type::Box:               return "Box";
        case Shape_Type::Circle:            return "Circle";
        case Shape_Type::Polygon:           return "Polygon";
    }
}

std::string StringFromPedal(Pedal pedal) {
    switch (pedal) {
        case Pedal::None:               return "None";
        case Pedal::Brake:              return "Brake";
        case Pedal::Clockwise:          return "Clockwise";
        case Pedal::CounterClockwise:   return "Counter Clockwise";
    }
}


}   // End namespace Dr
