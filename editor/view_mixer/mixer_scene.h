//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef MIXERSCENE_H
#define MIXERSCENE_H

#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QMap>

#include "editor/interface_editor_relay.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class DrTrack;
class IEditorRelay;
class MixerItem;


//####################################################################################
//##    MixerScene
//##        Displays a DrMix as a collection of DrTracks (MixerItem)
//############################
class MixerScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    long                    m_mixer_entity              { c_no_key };       // Entity (DrMix) to show DrTracks from

    // Selection Variables
    QList<QGraphicsItem*>   m_selection_items;                              // List of selected items

    // Timers for holding keys down
    QMap<Qt::Key, QElapsedTimer>    m_key_timers;
    QMap<Qt::Key, bool>             m_key_down;


public:
    // Constructor / Destructor
    explicit MixerScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, long mixer_entity_key = c_no_key);
    virtual ~MixerScene() override;

    // Event Overrides
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;

    // Construction
    void                    buildScene(long mix_scene);                     // Clears / builds node scene
    void                    clearSceneOverride();                           // Should call this instead of "clear()", prevents dangling pointers and resets local variables

    // Scene Functions
    MixerItem*              addItemToSceneFromTrack(DrTrack *track);
    QList<MixerItem*>       mixerItems();
    MixerItem*              mixerItemWithKey(QList<MixerItem*> &mixer_items, long entity_key);

    // Selection Functions
    QRectF                  totalSelectionSceneRect();

    // Other Widget Update Calls
    void                    updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);
    void                    updateSelectionFromKeyList(QList<long> key_list);

    // Getters / Setters
    IEditorRelay*           getEditorRelay()                        { return m_editor_relay; }
    DrProject*              getProject()                            { return m_project; }
    long                    getMixerEntityKey()                     { return m_mixer_entity; }


public slots:
    void            selectionChanged();


};


#endif // MIXERSCENE_H
















