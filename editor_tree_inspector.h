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

class DrProject;
class DrProperty;

class InterfaceRelay;

class InspectorCategoryButton;
class TripleSpinBox;
class WidgetHoverHandler;


enum class Spin_Type {
    Integer,
    Double,
    Percent,
    Angle,
    Point,
    Size,
};


// Class constants
const int   SIZE_LEFT =  3;                             // Size policy width of left column
const int   SIZE_RIGHT = 5;                             // Size policy width of right column


//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject           *m_project;                     // Pointer to currently loaded project
    InterfaceRelay      *m_relay;                       // Pointer to InterfaceRelay class of parent form

    WidgetHoverHandler  *m_widget_hover;                // Pointer to a widget hover handler

public:
    explicit        TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *relay);

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Function Calls
    void            buildInspectorFromKeys(QList<long> key_list);
    InterfaceRelay* getRelay() { return m_relay; }

    // Property Builders
    void            applyHeaderBodyProperties(QWidget *widget, DrProperty *property);
    void            applyHeaderBodyProperties(QWidget *widget, QString header, QString body);
    QCheckBox*      createCheckBox(DrProperty *property, QFont &font);
    QComboBox*      createComboBox(DrProperty *property, QFont &font);
    QDoubleSpinBox* createDoubleSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type);
    QFrame*         createDoubleSpinBoxPair(DrProperty *property, QFont &font, Spin_Type spin_type);
    QSpinBox*       createIntSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type);
    QLineEdit*      createLineEdit(DrProperty *property, QFont &font);
    QFrame*         createVariableSpinBoxPair(DrProperty *property, QFont &font);
    TripleSpinBox*  initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value);

private slots:
    void            itemWasClicked(QTreeWidgetItem *item, int column);
    void            setAdvisorInfo(QString header, QString body);

};



//####################################################################################
//##    InspectorCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Object Inspector List
//############################
class InspectorCategoryButton : public QPushButton
{
    Q_OBJECT

private:
    TreeInspector       *m_parent_tree;
    QTreeWidgetItem     *m_parent_item,     *m_child_item;
    QFrame              *m_child_frame;
    int                  m_height;
    bool                 m_is_shrunk = false;
    QRect                start_rect, end_rect;
    QString              m_header, m_body;

public:
    InspectorCategoryButton(const QString &text, TreeInspector *parent_tree,
                            QTreeWidgetItem *parent_item, QTreeWidgetItem *child_item, QFrame *new_child);

    // Events
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and Setters
    void            setAdvisorHeaderText(QString header) { m_header = header; }
    void            setAdvisorBodyText(QString body) { m_body = body; }

private slots:
    void animationDone();
    void animationUpdate(const QVariant &value);
    void buttonPressed();
};


//####################################################################################
//##    WidgetHoverHandler
//##        Catches hover events for widgets on Object Inspector without needing subclassing
//############################
class WidgetHoverHandler : public QObject
{
    Q_OBJECT

public:
    WidgetHoverHandler(QObject *parent) : QObject(parent) {}
    virtual ~WidgetHoverHandler() {}

    void attach(QWidget *widget);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalMouseHover(QString header, QString body);
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













