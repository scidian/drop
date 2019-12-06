//
//      Created by Stephens Nunnally on 11/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRANIMATION_H
#define DRANIMATION_H

#include <QImage>
#include <QPixmap>
#include <QString>

#include "model/settings/settings.h"

// Forward declarations
class DrFrame;
class DrProject;
class DrWorld;

typedef std::vector<DrFrame*> Frames;


//####################################################################################
//##    DrAnimation
//##        Class to hold an Animation for use in a Project / Asset
//############################
class DrAnimation : public DrSettings
{
private:
    // Local Variables
    Frames      m_frames;                               // Holds frames of this animation
                                                        // !!!!! #NOTE: Frame numbers start at 1 !!!!!

public:
    // Constructor / Destructor
    DrAnimation(DrProject *parent_project, long new_animation_key, std::list<long> image_keys);
    virtual ~DrAnimation() override;

    // DrSettings Virtual Overrides
    virtual DrType  getType() override      { return DrType::Animation; }

    // Function Calls
    long        addFrame(long image_key);
    void        initializeAnimationSettings(QString new_name);

    // Getters / Setters
    QPixmap                 getPixmapFromFirstFrame();
    DrFrame*                getFrame(long frame_number) { return m_frames[static_cast<unsigned long>(frame_number - 1)]; }
    Frames&                 getFrames()                 { return m_frames; }
    long                    getFrameCount()             { return static_cast<long>(m_frames.size()); }
    int                     getStartFrameNumber();

};



//####################################################################################
//##    DrFrame
//##        Class to hold one Frame of an Animation
//############################
class DrFrame : public DrSettings
{
private:

public:
    // Constructor / Destructor
    DrFrame(DrProject *parent_project, std::list<long> image_keys);
    virtual ~DrFrame() override;

    // Function Calls
    void            addComponentTransform(double width, double height, double x, double y);

    // Getters / Setters
    virtual DrType  getType() override      { return DrType::Frame; }
};


#endif // DRANIMATION_H















