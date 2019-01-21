//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Base class to receive signals from editor objects and relay them to derived form
//
//
#ifndef INTERFACE_RELAY_H
#define INTERFACE_RELAY_H

#include "colors.h"
#include "debug.h"
#include "enums.h"

class InterfaceRelay
{
public:
    InterfaceRelay() { }
    virtual ~InterfaceRelay();

    virtual void    buildObjectInspector(QList<long> key_list) = 0;
    virtual void    buildTreeSceneList() = 0;

    virtual QColor  getColor(Window_Colors color_role) = 0;

    virtual void    setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void    setLabelText(Label_Names label_name, QString new_text) = 0;

    virtual bool    debugFlag(Debug_Flags option_to_check) = 0;
};




#endif // INTERFACE_RELAY_H
