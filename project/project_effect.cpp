//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "imaging/imaging.h"
#include "project.h"
#include "project_effect.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrEffect::DrEffect(DrProject *parent_project, long key, DrEffectType effect_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_effect_type = effect_type;
}


//####################################################################################
//##    Creates a Pixmap for this Effect
//####################################################################################
QPixmap DrEffect::getPixmap() {
    switch (m_effect_type) {
        case DrEffectType::Light:   return QPixmap(":/assets/asset_types/light.png");
        case DrEffectType::Water:   return QPixmap(":/assets/asset_types/water.png");
        case DrEffectType::Fire:    return QPixmap(":/assets/asset_types/fire.png");
        case DrEffectType::Mirror:  return QPixmap(":/assets/asset_types/mirror.png");
        case DrEffectType::Fisheye: return QPixmap(":/assets/asset_types/fisheye.png");
        case DrEffectType::Swirl:   return QPixmap(":/assets/asset_types/swirl.png");
        case DrEffectType::Flag:    return QPixmap(":/assets/asset_types/flag.png");
        case DrEffectType::Rain:    return QPixmap(":/assets/asset_types/rain.png");
        case DrEffectType::Snow:    return QPixmap(":/assets/asset_types/snow.png");
        case DrEffectType::Clouds:  return QPixmap(":/assets/asset_types/clouds.png");
        case DrEffectType::Fog:     return QPixmap(":/assets/asset_types/fog.png");
    }
}











