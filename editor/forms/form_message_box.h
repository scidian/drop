//
//      Created by Stephens Nunnally on 2/20/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_MESSAGE_BOX_H
#define FORM_MESSAGE_BOX_H

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>


//####################################################################################
//##    FormMessageBox
//##        A popup message box
//############################
class FormMessageBox : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    QWidget     *m_parent           { nullptr };                                // Parent

    // Form Variables
    QWidget     *m_inner_widget     { nullptr };                                // Container widget, allows for a double form border
    QLabel      *m_pixmap           { nullptr };                                // Displays pixmap in message box

    // Result
    std::vector<QPushButton*>       m_button_list { };                          // Buttons on the form
    QMessageBox::StandardButton     m_button { QMessageBox::NoButton };         // Button returned


public:
    // Constructor / Destructor
    FormMessageBox(std::string message, QPixmap pixmap = QPixmap(), QWidget *parent = nullptr,
                   QMessageBox::StandardButtons buttons = QMessageBox::Ok);
    virtual ~FormMessageBox() override;


    // Event Overrides
    virtual void        resizeEvent(QResizeEvent *event) override;
    virtual void        showEvent(QShowEvent *event) override;

    // Form Building
    QPushButton*        addButton(QString text, QMessageBox::StandardButton button);

    // Form Actions
    QMessageBox::StandardButton     execute();

    // Getters / Setters
    QMessageBox::StandardButton     buttonPressed() { return m_button; }
    void                            setIconPixmap(QPixmap &pix) { m_pixmap->setPixmap(pix); }

};


#endif // FORM_MESSAGE_BOX_H












