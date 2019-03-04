//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Object Inspector List
//
//
#ifndef EDITOR_TREE_INSPECTOR_H
#define EDITOR_TREE_INSPECTOR_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTreeWidget>

#include "enums.h"

class DrProject;
class DrObject;
class DrSettings;
class DrProperty;

class IEditorRelay;
class DrTripleSpinBox;
class WidgetHoverHandler;



// Class constants
const int    c_inspector_size_left =   3;               // Size policy width of left column
const int    c_inspector_size_right =  5;               // Size policy width of right column
const int    c_minimum_grid_size =     5;               // Minimum grid size
const double c_minimum_grid_scale =   .1;               // Minimum grid sscale

//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject          *m_project;                      // Pointer to currently loaded project
    IEditorRelay       *m_editor_relay;                 // Pointer to IEditorRelay class of parent form
    WidgetHoverHandler *m_widget_hover;                 // Pointer to a widget hover handler

    QList<QWidget*>     m_widgets;                      // List of the widgets we built the last time we selected a new item

    long                m_selected_key;                 // Unique project key of currently selected item
    DrType              m_selected_type;                // Stores object type of currently selected item

public:
    explicit        TreeInspector(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Function Calls
    void            buildInspectorFromKeys(QList<long> key_list);
    void            updateInspectorPropertyBoxes(QList<DrSettings*> changed_items, QList<long> property_keys_to_update);
    void            updateSettingsFromNewValue(long property_key, QVariant new_value, long sub_order = 0);

    // Getters and Setters
    IEditorRelay*   getRelay()          { return m_editor_relay; }
    long            getSelectedKey()    { return m_selected_key; }

    // Property Builders
    void                attachToHoverHandler(QWidget *widget, DrProperty *property);
    void                attachToHoverHandler(QWidget *widget, QString header, QString body);
    void                addToWidgetList(QWidget *widget) { m_widgets.append(widget); }
    QCheckBox*          createCheckBox(DrProperty *property, QFont &font);
    QPushButton*        createListBox(DrProperty *property, QFont &font);
    QDoubleSpinBox*     createDoubleSpinBox(DrProperty *property, QFont &font, Property_Type spin_type);
    QFrame*             createDoubleSpinBoxPair(DrProperty *property, QFont &font, Property_Type spin_type);
    QSpinBox*           createIntSpinBox(DrProperty *property, QFont &font, Property_Type spin_type);
    QLineEdit*          createLineEdit(DrProperty *property, QFont &font);
    QFrame*             createVariableSpinBoxPair(DrProperty *property, QFont &font);
    DrTripleSpinBox*    initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value);

private slots:
    void                setAdvisorInfo(QString header, QString body);

};



//####################################################################################
//##    TripleSpinBox
//##        Allows us to control number of decimals being shown in spin box
//############################
class DrTripleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DrTripleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent) {}
    virtual ~DrTripleSpinBox() override {}

protected:
    virtual QString textFromValue(double value) const override;

};


//####################################################################################
//##    DropDownComboBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class DrDropDownComboBox : public QComboBox
{
    Q_OBJECT

public:
    DrDropDownComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}
    virtual ~DrDropDownComboBox() override {}

protected:
    virtual void showPopup() override;

};


//####################################################################################
//##    CheckBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class DrCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    DrCheckBox(QWidget *parent = nullptr) : QCheckBox (parent) {}
    virtual ~DrCheckBox() override {}

protected:
    virtual void paintEvent(QPaintEvent *) override;

};



#endif // EDITOR_TREE_INSPECTOR_H













