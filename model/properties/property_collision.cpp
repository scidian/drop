//
//      Created by Stephens Nunnally on 9/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "model/properties/property_collision.h"


//####################################################################################
//##    Adds a Polygon to the list of Polygons (m_polygons)
//####################################################################################
void DrPropertyCollision::addPolygon(std::vector<DrPointF> point_list) {
    m_polygons.push_back( point_list );
}
