//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QFontDatabase>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTime>

#include <cmath>

#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/types/dr_variant.h"
#include "editor/helper_library.h"

namespace Dr {


// Local Variables (internal file linkage)
static int      l_font_id;                      // Keeps id of our custom font
static int      l_font_id_bold;                 // Keeps id of our custom font
static QString  l_font_family;                  // Keeps family name of our custom font
static QString  l_font_family_bold;             // Keeps family name of our custom font


//####################################################################################
//##
//##    Time Functions
//##
//####################################################################################
// Returns System time as string
QString CurrentTimeAsString() {
    return QTime().currentTime().toString() + "." + QString::number(QTime().currentTime().msec());
}



//####################################################################################
//##
//##    Comparison Functions
//##
//####################################################################################
// Makes sure scale is not zero so we don't mess up QGraphicsItem transform
double CheckScaleNotZero(double scale_to_check) {
    if (scale_to_check <  .001 && scale_to_check >= 0) scale_to_check =  .001;
    if (scale_to_check > -.001 && scale_to_check <= 0) scale_to_check = -.001;
    return scale_to_check;
}

// Trims double to max_decimal_places, and then removes any trailing zeros
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
    l_font_id =          QFontDatabase::addApplicationFont(":/assets/fonts/Aileron-Regular.otf");
    l_font_id_bold =     QFontDatabase::addApplicationFont(":/assets/fonts/Aileron-Bold.otf");

    if (l_font_id >= 0) {
        l_font_family =      QFontDatabase::applicationFontFamilies(l_font_id).at(0);
        l_font_family_bold = QFontDatabase::applicationFontFamilies(l_font_id_bold).at(0);
        QFont font = CustomFont();
        qApp->setFont(font);
    }
}

// Returns custom font family
QString FontFamily() { return (l_font_id >= 0) ? l_font_family : "Arial"; }

// Project wide font size
int FontSize()  { return 11; }

QFont CustomFont() {
    QFont font(FontFamily());
    font.setPointSize(FontSize());
    return font;
}

QFont CustomFontLarger() {
    QFont font(FontFamily());
    font.setPointSize(FontSize() + 2);
    return font;
}


//####################################################################################
//##
//##    String Functions
//##
//####################################################################################
// Check the width of text that would be drawn with a particular font
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

QString StringFromBool(bool boolean)        { return boolean? "True" : "False"; }

// Finds a new name for a copy
QString FindCopyName(QString original, std::vector<QString> others) {
    QString new_name = original;
    if (new_name.indexOf(" copy") != -1) new_name = new_name.left(new_name.indexOf(" copy"));
    int same_count = 0;
    for (auto &one : others) {
        QString check_name = one;
        if (check_name.indexOf(" copy") != -1) check_name = check_name.left(check_name.indexOf(" copy"));
        if (check_name == new_name) same_count++;
    }

    // First copy
    if (same_count > 0) new_name += " copy";

    // Multiple copies, find name with unused copy number
    if (same_count > 1) {
        QString name_check = "";
        bool has_name = false;
        int  i = 1;
        do {
            has_name = false;
            name_check = new_name;
            if (i > 1) name_check += " (" + QString::number(i) + ")";
            for (auto &one : others) {
                if (one == name_check) {
                    has_name = true;
                    break;
                }
            }
            i++;
        } while (has_name == true);
        new_name = name_check;
    }
    return new_name;
}



//####################################################################################
//##
//##    DrLibrary Misc Conversion
//##
//####################################################################################
DrColor     FromQColor(const QColor &color)     { return DrColor(color.red(), color.green(), color.blue(), color.alpha()); }
DrPoint     FromQPoint(const QPoint &point)     { return DrPoint(point.x(), point.y()); }
DrPointF    FromQPointF(const QPointF &pointf)  { return DrPointF(pointf.x(), pointf.y()); }
QColor      ToQColor(const DrColor &color)      { return QColor(color.red(), color.green(), color.blue(), color.alpha()); }
QPoint      ToQPoint(const DrPoint &point)      { return QPoint(point.x, point.y); }
QPointF     ToQPointF(const DrPointF &pointf)   { return QPointF(pointf.x, pointf.y); }

//####################################################################################
//##    DrLibrary Image Conversion
//####################################################################################
DrBitmap    FromQImage(const QImage &image)     { return DrBitmap(image.constBits(), int(image.sizeInBytes()), false, image.width(), image.height()); }
DrBitmap    FromQPixmap(const QPixmap &pix)     { return FromQImage(pix.toImage()); }
QImage      ToQImage(const DrBitmap &bitmap)    { return QImage(bitmap.data.data(), bitmap.width, bitmap.height, QImage::Format::Format_ARGB32); }
QPixmap     ToQPixmap(const DrBitmap &bitmap)   { return QPixmap::fromImage(ToQImage(bitmap)); }


//####################################################################################
//##
//##    Qt Comparison
//##
//####################################################################################
// Compares 2 colors, returns true if they are the same
bool SameQColor(QColor color1, QColor color2, double tolerance) {
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   tolerance) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), tolerance) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  tolerance) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), tolerance) );
}

//####################################################################################
//##
//##    User Interaction
//##
//####################################################################################
// Shows a modal Error Message
void ShowErrorMessage(std::string function_name, std::string error_message, QWidget *parent) {
    std::string error = "Error from " + function_name + "(): " + error_message;
    QMessageBox msg_box(QMessageBox::Icon::Critical, "Error!", QString::fromStdString(error), QMessageBox::Ok, parent);
    msg_box.exec();
}

// Show a Modal Message Box with Icon or Pixmap
QMessageBox::StandardButton ShowMessageBox(std::string message, QPixmap pixmap, std::string title, QWidget *parent,
                                           QMessageBox::StandardButtons buttons) {
    QMessageBox msg_box(QMessageBox::Icon::NoIcon, QString::fromStdString(title), QString::fromStdString(message), buttons, parent);
    msg_box.setIconPixmap(pixmap);
    return static_cast<QMessageBox::StandardButton>(msg_box.exec());
}

QMessageBox::StandardButton ShowMessageBox(std::string message, QMessageBox::Icon icon, std::string title, QWidget *parent,
                                           QMessageBox::StandardButtons buttons) {
    QMessageBox msg_box(icon, QString::fromStdString(title), QString::fromStdString(message), buttons, parent);
    return static_cast<QMessageBox::StandardButton>(msg_box.exec());
}



}   // namespace Dr















