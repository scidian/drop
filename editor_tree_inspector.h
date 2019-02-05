//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Object Inspector List
//
//
#ifndef EDITOR_TREE_INSPECTOR_H
#define EDITOR_TREE_INSPECTOR_H

#include <QtWidgets>

#include "enums.h"

class DrProject;
class DrObject;
class DrProperty;

class InterfaceRelay;
class TripleSpinBox;
class WidgetHoverHandler;


enum class Spin_Type {
    Integer,                // Any integer
    Positive,               // Integer >= 0
    Float,                  // Any floating point number
    Percent,                // Floating point between 0.0 and 100.0
    Angle,                  // Show degrees sign
    Point,                  // Has X and Y
    Size,                   // Has W and H
};


// Class constants
const int   INSPECTOR_SIZE_LEFT =  3;                             // Size policy width of left column
const int   INSPECTOR_SIZE_RIGHT = 5;                             // Size policy width of right column


//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject          *m_project;                      // Pointer to currently loaded project
    InterfaceRelay     *m_relay;                        // Pointer to InterfaceRelay class of parent form
    WidgetHoverHandler *m_widget_hover;                 // Pointer to a widget hover handler

    QList<QWidget*>     m_widgets;                      // List of the widgets we built the last time we selected a new item

    long                m_selected_key;                 // Unique project key of currently selected item
    DrType              m_selected_type;                // Stores object type of currently selected item

public:
    explicit        TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *relay);

    // Function Calls
    void            buildInspectorFromKeys(QList<long> key_list);
    void            updateObjectProperty(DrObject* object, Object_Properties property);

    InterfaceRelay* getRelay() { return m_relay; }

    // Property Builders
    void            applyHeaderBodyProperties(QWidget *widget, DrProperty *property);
    void            applyHeaderBodyProperties(QWidget *widget, QString header, QString body);
    void            addToWidgetList(QWidget *widget) { m_widgets.append(widget); }
    QCheckBox*      createCheckBox(DrProperty *property, QFont &font);
    QComboBox*      createComboBox(DrProperty *property, QFont &font);
    QDoubleSpinBox* createDoubleSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type);
    QFrame*         createDoubleSpinBoxPair(DrProperty *property, QFont &font, Spin_Type spin_type);
    QSpinBox*       createIntSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type);
    QLineEdit*      createLineEdit(DrProperty *property, QFont &font);
    QFrame*         createVariableSpinBoxPair(DrProperty *property, QFont &font);
    TripleSpinBox*  initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value);

private slots:
    void            setAdvisorInfo(QString header, QString body);

};



//####################################################################################
//##    TripleSpinBox
//##        Allows us to control number of decimals being shown in spin box
//############################
class TripleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    TripleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent) {}
    virtual ~TripleSpinBox() override {}

protected:
    virtual QString textFromValue(double value) const override;

};


//####################################################################################
//##    DropDownComboBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class DropDownComboBox : public QComboBox
{
    Q_OBJECT

public:
    DropDownComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}
    virtual ~DropDownComboBox() override {}

protected:
    virtual void showPopup() override;

};




#endif // EDITOR_TREE_INSPECTOR_H













