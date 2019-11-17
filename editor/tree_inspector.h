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
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>

#include "enums.h"
#include "enums_dr_settings.h"

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

//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                      // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrFilterHoverHandler   *m_filter_hover = nullptr;       // Pointer to an event filter hover handler

    QList<QWidget*>         m_widgets;                      // List of the widgets we built the last time we selected a new item

    long                    m_selected_key = c_no_key;      // Unique project key of currently selected item
    DrType                  m_selected_type;                // Stores object type of currently selected item

public:
    explicit                TreeInspector(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Event Overrides
    virtual void            focusInEvent(QFocusEvent *event) override;

    // Function Calls
    void                    buildInspectorFromKeys(QList<long> key_list, bool force_rebuild = false);
    void                    expandCollapseComponents();
    QList<QTreeWidgetItem*> getListOfTopLevelItems();
    void                    updateInspectorPropertyBoxes(QList<DrSettings*> changed_items, QList<long> property_keys_to_update);
    void                    updateInspectorPropertyBoxesOfSelectedItem(QList<long> property_keys_to_update);
    void                    updateSettingsFromNewValue(long property_key, QVariant new_value, long sub_order = 0);
    void                    updateLockedSettings();

    // Getters and Setters
    IEditorRelay*           getRelay()          { return m_editor_relay; }
    long                    getSelectedKey()    { return m_selected_key; }

    DrFilterHoverHandler*   getHoverHandler();
    void                    setHoverHandler(DrFilterHoverHandler *hover_handler) { m_filter_hover = hover_handler; }

    // Property Builders
    void                    addToWidgetList(QWidget *widget) { m_widgets.append(widget); }
    QCheckBox*              createCheckBox(DrProperty *property, QFont &font, QSizePolicy size_policy);
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
    void        setButtonColor(QWidget *button, QColor color);

public slots:
    void        handleCollapsed(QTreeWidgetItem *item);
    void        handleExpanded(QTreeWidgetItem *item);

};



//####################################################################################
//##    TripleSpinBox
//##        Allows us to control number of decimals being shown in spin box
//############################
class DrQTripleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DrQTripleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent) { }
    virtual ~DrQTripleSpinBox() override { }

protected:
    virtual QString textFromValue(double value) const override;
};


//####################################################################################
//##    DropDownComboBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class DrQComboBoxDropDown : public QComboBox
{
    Q_OBJECT

public:
    DrQComboBoxDropDown(QWidget *parent = nullptr) : QComboBox(parent) { }
    virtual ~DrQComboBoxDropDown() override { }

protected:
    virtual void showPopup() override;
};


//####################################################################################
//##    CheckBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class DrQCheckBox : public QCheckBox
{
    Q_OBJECT
private:
    int     m_draw_left = 5;
    int     m_draw_top =  1;

public:
    DrQCheckBox(QWidget *parent = nullptr) : QCheckBox (parent) { }
    virtual ~DrQCheckBox() override { }

    int         getDrawLeft() { return m_draw_left; }
    int         getDrawTop() { return m_draw_top; }
    void        setDrawLeft(int left) { m_draw_left = left; }
    void        setDrawTop(int top) { m_draw_top = top; }

protected:
    virtual void paintEvent(QPaintEvent *) override;
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
    IEditorRelay      *m_editor_relay;                  // Pointer to IEditorRelay class of parent form

public:
    explicit        DrFilterInspectorImage(QObject *parent, IEditorRelay *editor_relay);

    // Event Overrides
    bool            eventFilter(QObject* object, QEvent* event) override;
};




#endif // EDITOR_TREE_INSPECTOR_H













