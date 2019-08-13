//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "image_filter.h"
#include "project.h"
#include "project_effect.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrEffect::DrEffect(DrProject *parent_project, long key, QString effect_name, DrEffectType effect_type) {
    setKey(key);

    m_parent_project = parent_project;
    m_effect_name = effect_name;
    m_effect_type = effect_type;
}



//####################################################################################
//##    Creates a Pixmap for this Effect
//####################################################################################
QPixmap DrEffect::getPixmap() {

    switch (m_effect_type) {
        case DrEffectType::Light:   return DrImaging::drawLight(Qt::white, 200, 0, 360, 50, 5);
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
