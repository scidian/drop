//
//      Created by   on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Physics Playground
//
//
#include <QDebug>
#include "playground.h"

//######################################################################################################
//##    Constructor
//######################################################################################################
DrPlayground::DrPlayground(FormPlayground *form_playground) : m_form_playground(form_playground) {

}

DrPlayground::~DrPlayground() {
    clearSpace();
}


//######################################################################################################
//##    Returns object at point in Space, or nullptr if there is none
//######################################################################################################
DrToy* DrPlayground::findToyAtPoint(cpVect point, double max_distance) {
    DrToy* toy = nullptr;
    cpShape *shape = nullptr;

    cpPointQueryInfo *query_info {};
    shape = cpSpacePointQueryNearest( m_space, point, max_distance, CP_SHAPE_FILTER_ALL, query_info );

    if (shape) {
        toy = static_cast<DrToy*>(cpShapeGetUserData(shape));
    }

    return toy;
}





