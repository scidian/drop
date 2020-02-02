//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/opengl/opengl.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"


//####################################################################################
//##    Chipmunk Callbacks
//####################################################################################
// Iterators to get a list of all constraints, shapes, etc attached to a body
static void GetBodyJointList(cpBody *, cpConstraint *constraint, std::vector<cpConstraint*> *joint_list) { joint_list->push_back(constraint); }
static void GetBodyShapeList(cpBody *, cpShape *shape, std::vector<cpShape*> *shape_list) { shape_list->push_back(shape); }


//####################################################################################
//##    Object Constructor
//##        Pass -1 for friction and/or bounce to use default world friction and bounce settings
//####################################################################################
DrEngineObject::DrEngineObject(DrEngineWorld *world, long unique_key, long original_key) : DrEngineThing (world, unique_key, original_key) { }

DrEngineObject::DrEngineObject(DrEngineWorld *world, long unique_key, long original_key, Body_Type body_type, long asset_key,
                               double x, double y, double z, DrPointF scale, double friction, double bounce,
                               bool should_collide, bool can_rotate, double angle, float opacity) : DrEngineThing (world, unique_key, original_key) {
    // Initialize variables
    polygons.clear();

    // Thing Basics
    this->setOpacity(opacity);
    this->setScaleX(scale.x);                                                   // Save x scale for later
    this->setScaleY(scale.y);                                                   // Save y scale for later
    this->setZOrder(z);

    // Object Basics                 !!!!! #NOTE: texture_number == Asset Key
    long image_number = asset_key;
    if (asset_key < 0) {
        image_number = asset_key;
    } else if (asset_key > 0) {
        DrAsset *asset = world->getProject()->findAssetFromKey(asset_key);
        if (asset != nullptr) {
            image_number = asset->getIdleAnimationFirstFrameImageKey();
            DrAnimation *animation = world->getProject()->findAnimationFromKey(asset->getBaseKey());
            if (animation != nullptr) {
                for (auto frame : animation->getFrames()) {
                    animation_idle_keys.push_back(frame->getKey());
                }
            }

        }
    }

    DrEngineTexture *texture = world->getTexture(image_number);
    if (texture != nullptr) {
        this->setSize( DrPointF(texture->width(), texture->height()));
    } else {
        this->setSize( DrPointF(100, 100) );
    }
    this->setTextureNumber(image_number);                                       // Texture to render from
    this->updateBodyPosition( DrPointF(x, y), true );

    this->setCustomFriction(friction);
    this->setCustomBounce(bounce);
    this->setDoesCollide(should_collide);
    this->setCanRotate(can_rotate);

    // Create Body
    this->body_type = body_type;
    switch (body_type) {
        ///case Body_Type::Static:  this->body = cpSpaceGetStaticBody(getWorld()->getSpace());      break;
        case Body_Type::Static:     this->body = cpBodyNewStatic();                                 break;
        case Body_Type::Dynamic:    this->body = cpBodyNew( 0.0, 0.0 );                             break;
        case Body_Type::Kinematic:  this->body = cpBodyNewKinematic();                              break;
    }
    cpBodySetPosition( this->body, cpv(x, y));
    cpBodySetAngle(    this->body, Dr::DegreesToRadians(-angle) );
    setAngle( -angle );
    cpBodySetUserData( this->body, this);                                       // Set chipmunk User Data, store DrEngineObject* for use later

    if (body_type == Body_Type::Kinematic) {
        cpBodySetVelocityUpdateFunc(this->body, KinematicUpdateVelocity);
    } else if (body_type == Body_Type::Dynamic) {
        cpBodySetVelocityUpdateFunc(this->body, ObjectUpdateVelocity);
    }
}


//####################################################################################
//##    Destructor
//####################################################################################
DrEngineObject::~DrEngineObject() {
    // ***** Signal physics children to remove themselves
    if (body_style != Body_Style::Rigid_Body) {
        for (auto &ball_number : soft_balls) {
            if (getWorld() != nullptr) {
                DrEngineObject *ball = getWorld()->findObjectByKey(ball_number);
                if (ball != nullptr) {
                    ball->setPhysicsParent(nullptr);
                    ball->removeMe();
                }
            }
        }
    // Seperates physics body from parent
    } else if (isPhysicsChild()) {
        if (getPhysicsParent() != nullptr) {
            for (auto &ball_number : getPhysicsParent()->soft_balls) {
                if (ball_number == this->getKey()) ball_number = c_no_key;
            }
        }
        setPhysicsParent(nullptr);
    }

    // ***** Delete cpBody and all cpShapes / cpConstraints (joints) associated with it
    if (this->body != nullptr) {
        cpSpace *space = cpBodyGetSpace(body);

        std::vector<cpShape*> shape_list;
        cpBodyEachShape(body, cpBodyShapeIteratorFunc(GetBodyShapeList), &shape_list);
        for (auto shape : shape_list) {
            cpSpaceRemoveShape(space, shape);
            cpShapeFree(shape);
        }
        std::vector<cpConstraint*> joint_list;
        cpBodyEachConstraint(body, cpBodyConstraintIteratorFunc(GetBodyJointList), &joint_list);
        for (auto joint : joint_list) {
            // Check if constraint to remove is mouse joint (drag joint)
            if (getWorld() && getWorld()->getEngine()) {
                if (joint == getWorld()->getEngine()->mouse_joint) {
                    getWorld()->getEngine()->mouse_joint = nullptr;
                }
            }     
            cpSpaceRemoveConstraint(space, joint);
            cpConstraintFree(joint);
        }
        cpSpaceRemoveBody(space, body);
        cpBodyFree(body);
        body = nullptr;
    }
}


//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineObject::addToWorld() {
    // Add Body and Shapes to Space
    cpSpaceAddBody(getWorld()->getSpace(), this->body);
    for (auto shape : shapes)
        cpSpaceAddShape( getWorld()->getSpace(), shape );

    // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
    if (this->canRotate() == false) {
        cpBodySetMoment( this->body, static_cast<double>(INFINITY) );
    }

    // Set initial velocity to forced velocity
    cpVect forced_velocity = cpv( getForcedSpeedX(), getForcedSpeedY() );
    cpBodySetVelocity( body, forced_velocity );

    // Reset Update Timer
    Dr::ResetTimer( update_timer );
}

DrPointF DrEngineObject::mapPositionToScreen() {
    return getWorld()->getEngine()->getOpenGL()->mapToScreen( getPosition().x, getPosition().y, getZOrder() );
}


//####################################################################################
//##    Collision Type of Object
//####################################################################################
void DrEngineObject::setCollisionType(Collision_Type what_should_collide) {
    m_collision_type = what_should_collide;
    for (auto shape : shapes)
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(what_should_collide));
}

// Sets Surface Velocity
void DrEngineObject::setSurfaceVelocity(cpVect surface_vel) {
    m_surface_velocity = surface_vel;
    for (auto shape : shapes)
        cpShapeSetSurfaceVelocity(shape, surface_vel);
}

bool DrEngineObject::doesDamage() {
    return (m_damage > c_epsilon || m_damage < -c_epsilon || m_death_touch);
}

bool DrEngineObject::shouldDamage(Collision_Type check_damage) {
    if (m_collision_type == Collision_Type::Damage_Enemy  && (check_damage == Collision_Type::Damage_Player || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_Player && (check_damage == Collision_Type::Damage_Enemy  || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_All)
        return true;
    return false;
}

// Returns true if two objects should collide
bool DrEngineObject::shouldCollide(DrEngineObject *b) {
    switch (this->getCollidesWith()) {
        case Collision_Groups::None:                return false;
        case Collision_Groups::Active_Players_Only: return (b->hasKeyControls() && (b->hasLostControl() == false));
        case Collision_Groups::Players:             return (b->hasKeyControls() || (b->getCollisionType() == Collision_Type::Damage_Enemy));
        case Collision_Groups::Enemies:             return (b->getCollisionType() == Collision_Type::Damage_Player);
        default: ;
    }
    return true;
}


//####################################################################################
//##    Take Damage / Heal,
//##        - If force_death is true, object dies no matter what
//##        - Returns
//##            TRUE:  just killed this object / object dead,
//##            FALSE: did not kill this object / object alive
//####################################################################################
bool DrEngineObject::takeDamage(double damage_to_take, bool reset_damage_timer, bool death_touch, bool force_death) {
    // If dying or dead exit now
    if (!m_alive || m_dying) return true;
    if (force_death) {
        setHealth(0.0, true);
        return true;
    }

    // Check if unlimited health
    bool unlimited_health = (getHealth() < 0.0);

    // HEALING: If damage was negative (healing), add health, check max_health and exit
    if (damage_to_take < 0.0 && !unlimited_health) {
        setHealth(getHealth() + abs(damage_to_take), true);
        if (reset_damage_timer) Dr::ResetTimer(m_damage_timer);
        if (getHealth() > m_max_health && m_max_health > c_epsilon) setHealth(m_max_health, true);
        return false;
    }

    // INVINCIBLE: If can't be hurt exit
    if (m_invincible)
        return false;
    else if (reset_damage_timer)
        Dr::ResetTimer(m_damage_timer);

    // TAKE DAMAGE: If not unlimited health, apply damage, if killed exit
    if (!unlimited_health) {
        setHealth(getHealth() - damage_to_take, true);
        if (getHealth() <= c_epsilon) {
            setHealth(0.0, true);
            return true;
        }
    }

    // DEATH TOUCH: If object was death touched, kill it and exit
    if (death_touch) {
        setHealth(0.0, true);
        return true;
    }

    return false;
}


//####################################################################################
//##    Sets Health of Object
//####################################################################################
void DrEngineObject::setHealth(double new_health, bool taking_damage) {
    m_health = new_health;
    if (taking_damage == true) updateRelativeHealth();
}

//####################################################################################
//##    Updates Health of related Physics Parent / Physics Children
//####################################################################################
void DrEngineObject::updateRelativeHealth() {
    if (body_style == Body_Style::Circular_Blob || body_style == Body_Style::Square_Blob || body_style == Body_Style::Mesh_Blob) {
        updateChildrenHealth();
    } else {
        if (getPhysicsParent() == nullptr) return;
        getPhysicsParent()->setHealth(this->getHealth());
        getPhysicsParent()->setDamageTimer(this->getDamageTimer());
        getPhysicsParent()->updateChildrenHealth();
    }
}

//####################################################################################
//##    Updates Health of Physics Children to Match Parent
//####################################################################################
void DrEngineObject::updateChildrenHealth() {
    if (body_style == Body_Style::Rigid_Body) return;
    if (getWorld() == nullptr) return;
    for (auto &ball_number : soft_balls) {
        DrEngineObject *ball = getWorld()->findObjectByKey(ball_number);
        if (ball != nullptr) {
            ball->setHealth(this->getHealth());
            ball->setDamageTimer(this->getDamageTimer());
        }
    }
}















