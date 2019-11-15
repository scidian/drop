//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>

#include "engine.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_texture.h"
#include "engine_world.h"
#include "form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/thing_shape_list.h"


//####################################################################################
//##    Build Space
//####################################################################################
void DrEngineWorld::buildWorld(long world_id_to_build, Demo_Player player_to_use) {

    // ***** Basic World Proeprties
    m_background_color = QColor(0, 0, 0);
    m_ambient_light = 50;

    // ***** Create Physics World
    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity

    // Default gravity / damping settings
    m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector (aka 2D point) and cpv() is a shortcut for initializing them
    m_damping = 1;                                      // Kind of like air drag
    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);


    // ***** Custom Wildcard beginFunc CollisionHandlers: Damage / Health    
    QVector<Collision_Type> collide_types { Collision_Type::Damage_None,
                                            Collision_Type::Damage_Player,
                                            Collision_Type::Damage_Enemy,
                                            Collision_Type::Damage_All };
    for (Collision_Type collision : collide_types) {
        cpCollisionHandler *custom_collision_handler = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(collision));
        custom_collision_handler->beginFunc =    BeginFuncWildcard;
        custom_collision_handler->preSolveFunc = PreSolveFuncWildcard;
        custom_collision_handler->separateFunc = SeperateFuncWildcard;
    }


    // ***** Find current world, load Start Stage of that world
    DrWorld *world = m_project->getWorld(world_id_to_build);
    DrStage *stage = world->getStageFromKey(world->getFirstStageKey());

    // ***** World Settings
    m_game_direction =      world->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
    m_delete_threshold_x =  world->getComponentPropertyValue(Components::World_Settings, Properties::World_Deletion_Threshold).toInt();
    m_delete_threshold_y =  world->getComponentPropertyValue(Components::World_Settings, Properties::World_Deletion_Threshold).toInt();
    m_game_start = DrPointF(0, 0);                      // Set starting load position
    m_loaded_to = 0;                                    // Reset how far we've loaded


    if (world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool()) {
        m_background_color = QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());
    }

    // ***** World Physics Properties
    m_time_warp =           world->getComponentPropertyValue(Components::World_Physics, Properties::World_Time_Warp).toDouble();
    m_damping =             world->getComponentPropertyValue(Components::World_Physics, Properties::World_Drag).toDouble();
    m_friction =            world->getComponentPropertyValue(Components::World_Physics, Properties::World_Friction).toDouble();
    m_bounce =              world->getComponentPropertyValue(Components::World_Physics, Properties::World_Bounce).toDouble();
    QPointF get_gravity =   world->getComponentPropertyValue(Components::World_Physics, Properties::World_Gravity).toPointF();
    m_gravity = cpv(get_gravity.x(), get_gravity.y());

    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);

    // ***** World appearance settings
    int render =            world->getComponentPropertyValue(Components::World_Camera, Properties::World_Camera_Type).toInt();
    cam_switch_speed =      world->getComponentPropertyValue(Components::World_Camera, Properties::World_Camera_Switch_Speed).toDouble();
    render_type = static_cast<Render_Type>(render);

    m_ambient_light =       world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Ambient).toDouble();
    m_glow_light_z_order =  world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Layer).toDouble();
    m_glow_blend_mode =     world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Blend).toInt();

    bitrate =               world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Bitrate).toList().first().toInt();
    QPointF pixelation =    world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Pixelation).toPointF();
    pixel_x =               static_cast<float>(pixelation.x());
    pixel_y =               static_cast<float>(pixelation.y());
    brightness =        world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Brightness).toList().first().toInt() / 255.f;
    contrast =          world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Contrast).toList().first().toInt() / 255.f;
    saturation =        world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Saturation).toList().first().toInt() / 255.f;
    hue =               world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Hue).toList().first().toInt() / 360.f;
    grayscale =         world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Grayscale).toBool();
    negative =          world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Negative).toBool();

    bool draw_3D =      world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Convert_3D).toBool();
    render_mode =       draw_3D ? Render_Mode::Mode_3D : Render_Mode::Mode_2D;
    wireframe =         world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Wireframe).toBool();
    cartoon =           world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Cartoon).toList()[0].toBool();
    cartoon_width =     world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Cartoon).toList()[1].toFloat();
    cross_hatch =       world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Cross_Hatch).toList()[0].toBool();
    cross_hatch_width = world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Cross_Hatch).toList()[1].toFloat();
    wavy =              world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Wavy).toBool();


    // ***** Load Start Stage Players
    addPlayer( player_to_use );

    // ***** Load Current Stage to origin position
    loadStageToWorld(stage, 0, 0, true);


    // ********** Bouyancy Test
    /**
    cpBody *staticBody = cpSpaceGetStaticBody(m_space);

    // Add the sensor for the water
    cpBB bb = cpBBNew(-2000, -1000, 0, -10);
    cpShape *shape = cpSpaceAddShape(m_space, cpBoxShapeNew2(staticBody, bb, 0.0));
    cpShapeSetSensor(shape, cpTrue);
    cpShapeSetCollisionType(shape, 232323);     // 232323 is temp number for water

    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(m_space, 232323, 0);
    handler->preSolveFunc = static_cast<cpCollisionPreSolveFunc>(WaterPreSolve);
    for (Collision_Type collision : collide_types) {
        cpCollisionHandler *handler = cpSpaceAddCollisionHandler(m_space, 232323, static_cast<cpCollisionType>(collision));
        handler->preSolveFunc = static_cast<cpCollisionPreSolveFunc>(WaterPreSolve);
    }
    */


    // ********** Static Line Segments
    /**
    DrEngineObject *line1 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line1->addShapeSegment( DrPointF( -800,    0), DrPointF( 300, -250) );
    DrEngineObject *line2 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line2->addShapeSegment( DrPointF(  250,   50), DrPointF(1750,  350) );
    DrEngineObject *line3 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line3->addShapeSegment( DrPointF(-1100, -300), DrPointF(-900, -300) );
    addThings( { line1, line2, line3 } );
    */


    // ********** Bridge Test for Rocky Rover car
    /**
    DrEngineObject *anchor_a = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 2500, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_1 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2600, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_2 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2700, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_3 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2800, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_4 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2900, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_5 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3000, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_6 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3100, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_7 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3200, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *chain_8 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3300, -10, -1, DrPointF(1.5, .1));
    DrEngineObject *anchor_b = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 3400, -10, -1, DrPointF(1.5, .1));
    anchor_a->addShapeBoxFromTexture(Asset_Textures::Block);
    chain_1->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_2->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_3->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_4->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_5->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_6->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_7->addShapeBoxFromTexture( Asset_Textures::Block);
    chain_8->addShapeBoxFromTexture( Asset_Textures::Block);
    anchor_b->addShapeBoxFromTexture(Asset_Textures::Block);
    addThings( { anchor_a, chain_1, chain_2, chain_3, chain_4, chain_5, chain_6, chain_7, chain_8, anchor_b } );

    cpSpaceAddConstraint( m_space, cpPivotJointNew(anchor_a->body, chain_1->body, cpBodyGetPosition(chain_1->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_1->body,  chain_2->body, cpBodyGetPosition(chain_2->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_2->body,  chain_3->body, cpBodyGetPosition(chain_3->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_3->body,  chain_4->body, cpBodyGetPosition(chain_4->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_4->body,  chain_5->body, cpBodyGetPosition(chain_5->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_5->body,  chain_6->body, cpBodyGetPosition(chain_6->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_6->body,  chain_7->body, cpBodyGetPosition(chain_7->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_7->body,  chain_8->body, cpBodyGetPosition(chain_8->body)) );
    cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_8->body, anchor_b->body, cpBodyGetPosition(anchor_b->body)) );

    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group = 56;                      // Any int > 0, maybe use unique project id of parent? or keep a key generator when Engine starts
    filter.categories = all_categories;     // CP_ALL_CATEGORIES
    filter.mask =       all_categories;     // CP_ALL_CATEGORIES
    for (auto shape : anchor_a->shapes) cpShapeSetFilter( shape, filter);
    for (auto shape : anchor_b->shapes) cpShapeSetFilter( shape, filter);
    for (auto shape : chain_1->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_2->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_3->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_4->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_5->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_6->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_7->shapes)  cpShapeSetFilter( shape, filter);
    for (auto shape : chain_8->shapes)  cpShapeSetFilter( shape, filter);
    */

}














