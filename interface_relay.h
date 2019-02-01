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


//####################################################################################
//##    InterfaceRelay
//##        An interface class used to communicate between classes
//############################
class InterfaceRelay
{
public:
    InterfaceRelay() { }
    virtual ~InterfaceRelay();

    virtual void    buildAssetList() = 0;
    virtual void    buildObjectInspector(QList<long> key_list) = 0;
    virtual void    buildTreeSceneList() = 0;

    virtual void    populateScene(long from_scene_key) = 0;

    virtual void    setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void    setAdvisorInfo(QString header, QString body) = 0;
    virtual void    setLabelText(Label_Names label_name, QString new_text) = 0;

    virtual void    updateObjectInspectorAfterItemChange(long item_key) = 0;
    virtual void    updateSceneTreeSelectionBasedOnSelectionGroup() = 0;
};



// External functions, can be called Globally without a local InterfaceRelay instance
namespace Dr {

    void SetActiveRelay(InterfaceRelay *new_relay);
    void SetLabelText(Label_Names label, QString text);

}



#endif // INTERFACE_RELAY_H
