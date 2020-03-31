//
//      Created by Stephens Nunnally on 3/31/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_VARIABLE_H
#define DR_VARIABLE_H

#include "project/settings/settings.h"


//####################################################################################
//##    DrVariable
//##        Class to hold a Variable for use in a Project
//############################
class DrVariable : public DrSettings
{
private:
    // Local Variables
    Variant_Type    m_variable_type;                                // Stores initial type of Variable


public:
    // Constructors
    DrVariable(DrProject *parent_project, std::string variable_name, Variant_Type variable_type, DrVariant initial_value);
    virtual ~DrVariable() override;

    // DrSettings Overrides
    virtual DrType      getType() override  { return DrType::Variable; }

    // Data Functions
    DrVariant           getCurrent();
    DrVariant           getBest();
    DrVariant           getTotal();
    void                setCurrent(DrVariant value);
    void                setBest(DrVariant value);
    void                setTotal(DrVariant value);

};

#endif // DR_VARIABLE_H






