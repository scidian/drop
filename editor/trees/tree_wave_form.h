//
//      Created by Stephens Nunnally on 6/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef TREE_WAVE_FORM_H
#define TREE_WAVE_FORM_H

#include <QTreeWidget>
#include <QMutex>

#include "project/enums_entity_types.h"

// Forward Declarations
class DrFilterHoverHandler;
class DrProject;
class DrSound;
class IEditorRelay;
class WaveForm;


//####################################################################################
//##    TreeWaveForm
//##        A Tree List to show selected DrSound wave form in a dock for Sound Creator
//############################
class TreeWaveForm: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                              // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrFilterHoverHandler   *m_filter_hover      { nullptr };        // Pointer to an event filter hover handler
    WaveForm               *m_wave_form         { nullptr };        // Pointer to a WaveForm QFrame instance that draws currently selected DrSound

    long                    m_selected_key      { c_no_key };       // Project key of selected sound

public:
    // Constructor / Destructor
    TreeWaveForm(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~TreeWaveForm();

    // Event Overrides


    // Getters / Setters
    IEditorRelay*           getEditorRelay()            { return m_editor_relay; }
    DrProject*              getParentProject()          { return m_project; }

    DrFilterHoverHandler*   getHoverHandler()           { return m_filter_hover; };
    WaveForm*               getWaveForm()               { return m_wave_form; }

    void                    setHoverHandler(DrFilterHoverHandler* hover_handler) { m_filter_hover = hover_handler; }
    void                    setWaveForm(WaveForm *wave_form) { m_wave_form = wave_form; }


    // Sound Selection
    long                    getSelectedKey()            { return m_selected_key; }
    void                    setSelectedKey(long key)    { m_selected_key = key; }
    void                    setSound(DrSound *sound);

private slots:
    void            setAdvisorInfo(QString header, QString body);

};


#endif // TREE_WAVE_FORM_H












