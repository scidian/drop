//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Base class to receive signals from editor objects and relay them to derived form
//
//
#ifndef INTERFACE_RELAY_H
#define INTERFACE_RELAY_H

#include "enums.h"

class DrObject;
class DrSettings;

//####################################################################################
//##    InterfaceRelay
//##        An interface class used to communicate between classes
//############################
class InterfaceRelay
{
public:
    InterfaceRelay() { }
    virtual ~InterfaceRelay();

    virtual void    buildAssetTree() = 0;
    virtual void    buildObjectInspector(QList<long> key_list) = 0;
    virtual void    buildProjectTree() = 0;
    virtual void    buildScene(long from_stage_key) = 0;

    virtual void    updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<long> property_keys) = 0;
    virtual void    updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys) = 0;

    virtual void    updateItemSelection(Editor_Widgets selected_from) = 0;





    virtual void    centerViewOnPoint(QPointF center_point) = 0;

    virtual void    setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void    setAdvisorInfo(QString header, QString body) = 0;
    virtual void    setLabelText(Label_Names label_name, QString new_text) = 0;
};



// External functions, can be called Globally without a local InterfaceRelay instance
namespace Dr {

    void SetActiveRelay(InterfaceRelay *new_relay);
    void SetLabelText(Label_Names label, QString text);

}



#endif // INTERFACE_RELAY_H
