//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Inspector List
//
//
#ifndef EDITOR_TREE_INSPECTOR_H
#define EDITOR_TREE_INSPECTOR_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"


// Forward declarations
class DrProject;
class DrThing;
class DrSettings;
class DrProperty;

class IEditorRelay;
class DrQTripleSpinBox;
class DrFilterHoverHandler;

// Class constants
constexpr int    c_inspector_size_left =   3;           // Size policy width of left column
constexpr int    c_inspector_size_right =  5;           // Size policy width of right column
constexpr int    c_minimum_grid_size =     1;//5;       // Minimum grid size
constexpr double c_minimum_grid_scale =   .1;           // Minimum grid sscale

constexpr int    c_spacer_item_key =     -10;           // Number is arbitrary below zero value, used to ignore spacer category while iterating


//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project               { nullptr };            // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay          { nullptr };            // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrFilterHoverHandler   *m_filter_hover          { nullptr };            // Pointer to an event filter hover handler

    QList<QWidget*>         m_widgets               { };                    // List of the widgets we built the last time we selected a new item

    long                    m_key_shown             { c_no_key };           // Key of Entity whose info is actually displayed in Inspector
    QList<long>             m_selected_keys         { };                    // Unique project keys of currently selected Entities
    DrType                  m_selected_type         { DrType::NotFound };   // Stores object type of currently selected Entities

    int                     m_last_scroll_position  { 0 };                  // Stores last known scroll position, keeps Inspector focused at similar position

public:
    QLabel      *temp_selected_label = nullptr;

public:
    explicit                TreeInspector(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Event Overrides
    virtual void            focusInEvent(QFocusEvent *event) override;

    // Function Calls
    void                    buildInspectorFromKeys(QList<long> key_list, bool force_rebuild = false);
    void                    expandCollapseComponents();
    QList<QTreeWidgetItem*> getListOfTopLevelItems();
    void                    forceUpdateOfItemSizes() { this->scheduleDelayedItemsLayout(); }                // #NOTE: Forces updating of QTreeWidget, yay!
    void                    updateInspectorPropertyBoxes(std::list<DrSettings*> changed_items, std::list<ComponentProperty> property_keys_to_update);
    void                    updateInspectorPropertyBoxesOfSelectedItem(std::list<ComponentProperty> property_keys_to_update);
    void                    updateLockedSettings();
    void                    updateSubProperties(bool called_from_build = false);
    void                    updateSettingsFromNewValue(ComponentProperty component_property_pair, DrVariant new_value, long sub_order = 0);

    // Getters and Setters
    DrProject*              getParentProject()  { return m_project; }
    IEditorRelay*           getEditorRelay()    { return m_editor_relay; }
    QList<long>&            getSelectedKeys()   { return m_selected_keys; }
    long                    getShownKey()       { return m_key_shown; }

    DrFilterHoverHandler*   getHoverHandler();
    void                    setHoverHandler(DrFilterHoverHandler *hover_handler) { m_filter_hover = hover_handler; }

    // Property Builders
    void                    addToWidgetList(QWidget *widget) { m_widgets.append(widget); }
    QCheckBox*              createCheckBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type check_type);
    QFrame*                 createCheckBoxIntBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QFrame*                 createCheckBoxSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QWidget*                createColorBox(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QFrame*                 createImageFrame(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QPushButton*            createListBox(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QDoubleSpinBox*         createDoubleSpinBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type);
    QFrame*                 createDoubleSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type);
    QFrame*                 createDoubleSpinBoxTrio(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type);
    QSpinBox*               createIntSpinBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type);
    QLineEdit*              createLineEdit(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QTextEdit*              createTextEdit(DrProperty *property, QFont &font, QSizePolicy size_policy);
    QWidget*                createSlider(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type);
    QFrame*                 createVariableSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy);
    DrQTripleSpinBox*       initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value);
    void                    setButtonColorFromSystemDialog(QPushButton *button);
    void                    updateColorButton(QPushButton *button, QColor color);

private slots:
    void        setAdvisorInfo(QString header, QString body);
    void        setButtonColor(QWidget *button, DrColor color);

public slots:
    void        handleCollapsed(QTreeWidgetItem *item);
    void        handleExpanded(QTreeWidgetItem *item);

};


//####################################################################################
//##    DrImageHolder
//##        Event filter handler for image properties
//####################################################################################
class DrImageHolder : public QFrame
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    IEditorRelay   *m_editor_relay;                     // Pointer to IEditorRelay class of parent form

    // Local Variables
    QPushButton    *m_delete_button = nullptr;          // Little "Delete" button
    QPushButton    *m_edit_button = nullptr;            // Little "Edit" button

public:
    explicit        DrImageHolder(IEditorRelay *editor_relay, QWidget *parent = nullptr);

    // Getters / Setters
    QPushButton*        getDeleteButton()       { return m_delete_button; }
    QPushButton*        getEditButton()         { return m_edit_button; }
    void                setDeleteButton(QPushButton *button)    { m_delete_button = button; }
    void                setEditButton(QPushButton *button)      { m_edit_button = button; }
};


//####################################################################################
//##    DrFilterInspectorImage
//##        Event filter handler for image properties
//####################################################################################
class DrFilterInspectorImage : public QObject
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    IEditorRelay   *m_editor_relay;                     // Pointer to IEditorRelay class of parent form

public:
    explicit        DrFilterInspectorImage(QObject *parent, IEditorRelay *editor_relay);

    // Event Overrides
    bool                eventFilter(QObject* object, QEvent* event) override;

    // Getters / Setters
    IEditorRelay*       getEditorRelay()        { return m_editor_relay; }
};




#endif // EDITOR_TREE_INSPECTOR_H













