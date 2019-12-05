//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DREFFECT_H
#define DREFFECT_H

#include "model/settings/settings.h"

// Forward Declarations
class DrProject;
enum class DrEffectType;


//####################################################################################
//##    DrEffect
//##        Class to hold an Effect for use in a Project
//############################
class DrEffect : public DrSettings
{
private:
    // Local Variables
    DrEffectType     m_effect_type;                     // Effect Type as DrEffectType

public:
    // Constructor
    DrEffect(DrProject *parent_project, long key, DrEffectType effect_type);

    // DrSettings Overrides
    virtual DrType  getType() override      { return DrType::Effect; }

    // Functions
    QPixmap         getPixmap();                        // Returns a pixmap for an Effect in the editor based on the EffectType

    // Getters / Setters
    DrEffectType    getEffectType() { return m_effect_type; }

};


#endif // DREFFECT_H



