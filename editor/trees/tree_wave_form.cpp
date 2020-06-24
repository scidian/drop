//
//      Created by Stephens Nunnally on 6/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/wave_form.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_wave_form.h"
#include "project/entities_sound/dr_sound.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
TreeWaveForm::TreeWaveForm(QWidget *parent, DrProject *project, IEditorRelay *editor_relay)
    : QTreeWidget(parent), m_project(project), m_editor_relay(editor_relay) {

    // ***** Initialize hover handler
    setHoverHandler(new DrFilterHoverHandler(this));
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    getHoverHandler()->attachToHoverHandler(this, Advisor_Info::Wave_Form);


    // ***** Build Tree
    this->clear();                                              // Clear / delete all from advisor tree
    this->setFixedHeight(100);

    // Insert top level item to act as widget for dock
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this);
    this->addTopLevelItem(topLevelItem);

    // Create Wave Form instance
    m_wave_form = new WaveForm(this, nullptr);
    m_wave_form->setContentsMargins(0, 0, 0, 0);

    // Apply wave form to tree, expand all
    this->setItemWidget(topLevelItem, 0, m_wave_form);
    this->expandAll();
}

TreeWaveForm::~TreeWaveForm() { }


//####################################################################################
//##    SLOTS
//####################################################################################
// SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
void TreeWaveForm::setAdvisorInfo(QString header, QString body) {
    m_editor_relay->setAdvisorInfo(header, body);
}


//####################################################################################
//##    Sound Selection
//####################################################################################
void TreeWaveForm::setSound(DrSound *sound) {
    if (sound == nullptr) {
        m_wave_form->setSound(nullptr);
        m_selected_key = c_no_key;
    } else {
        m_wave_form->setSound(sound->getAudioSource());
        m_selected_key = sound->getKey();
    }
    this->update();
}














