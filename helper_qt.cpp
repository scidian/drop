//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "helper_qt.h"

#include <QApplication>
#include <QFontDatabase>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTime>
#include <cmath>

#include "colors/colors.h"
#include "helper.h"
#include "widgets/widgets.h"

namespace Dr {


// Internal file linkage variables
static int      g_font_id;                      // Keeps id of our custom font
static int      g_font_id_bold;                 // Keeps id of our custom font
static QString  g_font_family;                  // Keeps family name of our custom font
static QString  g_font_family_bold;             // Keeps family name of our custom font


//####################################################################################
//##    Returns System time as string
//####################################################################################
QString CurrentTimeAsString() {
    return QTime().currentTime().toString() + "." + QString::number(QTime().currentTime().msec());
}



//####################################################################################
//##    Makes sure scale is not zero so we don't mess up QGraphicsItem transform
//####################################################################################
double CheckScaleNotZero(double scale_to_check) {
    if (scale_to_check <  .001 && scale_to_check >= 0) scale_to_check =  .001;
    if (scale_to_check > -.001 && scale_to_check <= 0) scale_to_check = -.001;
    return scale_to_check;
}



//####################################################################################
//##    Trims double to max_decimal_places, and then removes any trailing zeros
//####################################################################################
QString RemoveTrailingDecimals(double value, int max_decimal_places) {
    double int_part, decimal_part = 0;

    // Remove any extra starting decimal places
    value = QString::number(value, 'f', max_decimal_places).toDouble();

    int count = 0;
    while (count < max_decimal_places) {
        decimal_part = std::modf(value * pow(10, count), &int_part);
        if (decimal_part == 0.0) break;
        ++count;
    }

    return QString::number(value, 'f', count);
}



//####################################################################################
//##
//##    Custom Font Functions
//##
//####################################################################################
void LoadCustomFonts() {
    g_font_id =          QFontDatabase::addApplicationFont(":/assets/fonts/Aileron-Regular.otf");
    g_font_id_bold =     QFontDatabase::addApplicationFont(":/assets/fonts/Aileron-Bold.otf");

    if (g_font_id >= 0) {
        g_font_family =      QFontDatabase::applicationFontFamilies(g_font_id).at(0);
        g_font_family_bold = QFontDatabase::applicationFontFamilies(g_font_id_bold).at(0);
        QFont font = CustomFont();
        qApp->setFont(font);
    }
}

// Returns custom font family
QString FontFamily() { return (g_font_id >= 0) ? g_font_family : "Arial"; }

// Project wide font size
int FontSize()  { return 11; }

QFont CustomFont() {
    QFont font(FontFamily());
    font.setPointSize(FontSize());
    return font;
}


//####################################################################################
//##    These functions check the width of text that would be drawn
//##    with a particular font
//####################################################################################
int CheckFontWidth(QFont font, QString text_to_check) {
    QFontMetrics font_metrics { font };
    return font_metrics.horizontalAdvance( text_to_check );
}

// Shortens label text to fit within a widget (max_width), used in editor_tree_assets frames
QString FitStringToWidth(QFont font, QString text_to_check, int max_width, bool use_dots) {
    QString text = text_to_check;
    QFontMetrics font_metrics { font };
    int width = font_metrics.horizontalAdvance( text_to_check );

    int length = text.length();
    while (width > max_width && length >= 1) {
        --length;
        text = text_to_check.left(length);
        if (use_dots) text += "...";
        width = font_metrics.horizontalAdvance( text );
    }
    return text;
}

QString StringFromBool(bool boolean) { return boolean? "True" : "False"; }


//####################################################################################
//##    Shows a modal Error Message
//####################################################################################
void ShowErrorMessage(QString function_name, QString error_message, QWidget *parent) {
    QString error = "Error from " + function_name + "(): " + error_message;
    QMessageBox msg_box(QMessageBox::Icon::Critical, "Error!", error, QMessageBox::Ok, parent);
    msg_box.exec();
}


//####################################################################################
//##    Show a Modal Message Box with Icon or Pixmap
//####################################################################################
QMessageBox::StandardButton ShowMessageBox(QString message, QPixmap pixmap, QString title, QWidget *parent,
                                           QMessageBox::StandardButtons buttons) {
    QMessageBox msg_box(QMessageBox::Icon::NoIcon, title, message, buttons, parent);
    msg_box.setIconPixmap(pixmap);
    return static_cast<QMessageBox::StandardButton>(msg_box.exec());
}

QMessageBox::StandardButton ShowMessageBox(QString message, QMessageBox::Icon icon, QString title, QWidget *parent,
                                           QMessageBox::StandardButtons buttons) {
    QMessageBox msg_box(icon, title, message, buttons, parent);
    return static_cast<QMessageBox::StandardButton>(msg_box.exec());
}





}   // namespace Dr















