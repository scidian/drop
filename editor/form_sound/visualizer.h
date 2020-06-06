//
//      Created by Stephens Nunnally on 5/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QFrame>

#include "3rd_party/soloud/soloud.h"
#include "project/enums_entity_types.h"


//####################################################################################
//##    VisualFrame
//##        Allows us to draw visualizer for sound played by SoLoud
//############################
class VisualFrame : public QFrame
{
    Q_OBJECT

private:

public:
    VisualFrame(QWidget *parent = nullptr);
    virtual ~VisualFrame() override;

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;
};

#endif // VISUALIZER_H











