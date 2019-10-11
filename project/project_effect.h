//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PROJECT_EFFECT_H
#define PROJECT_EFFECT_H

#include <QString>

#include "settings/settings.h"

// Forward Declarations
class DrProject;
enum class DrEffectType;


//####################################################################################
//##    DrEffect
//##        Class to hold an effect for use in a project
//############################
class DrEffect : public DrSettings
{
private:
    // External Borrowed Pointers
    DrProject       *m_parent_project;                  // Holds reference to parent Project class that handles key generation for project

    // Local Variables
    QString          m_effect_name;                     // Effect Type as a String, "Light", "Mirror", etc
    DrEffectType     m_effect_type;                     // Effect Type as DrEffectType

public:
    DrEffect(DrProject *parent_project, long key, QString effect_name, DrEffectType effect_type);

    // Functions
    QPixmap         getPixmap();                        // Returns a pixmap for a Light Effect in the editor based on the EffectType

    // Getters / Setters
    QString         getName() override { return m_effect_name; }
    DrEffectType    getEffectType() { return m_effect_type; }

};


#endif // PROJECT_EFFECT_H




