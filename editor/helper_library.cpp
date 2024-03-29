//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QFontDatabase>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTime>

#include <cmath>

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/types/dr_variant.h"
#include "editor/forms/form_message_box.h"
#include "editor/helper_library.h"
#include "editor/pixmap/pixmap.h"
#include "editor/style/style.h"

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
QString FontFamily()        { return (l_font_id >= 0) ? l_font_family : "Arial"; }

// Project wide font size
int FontSize(bool scale)    { return (scale ? Dr::Scale(11) : 11); }

QFont CustomFont(int add_points, bool scale) {
    QFont font(FontFamily());
    if (scale)  font.setPointSize(FontSize(scale) + Dr::Scale(add_points));
    else        font.setPointSize(FontSize(scale) + add_points);
    return font;
}

QFont CustomFontLarger(bool scale) {
    QFont font(FontFamily());
    if (scale)  font.setPointSize(FontSize(scale) + Dr::Scale(2));
    else        font.setPointSize(FontSize(scale) + 2);
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
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   static_cast<float>(tolerance)) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), static_cast<float>(tolerance)) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  static_cast<float>(tolerance)) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), static_cast<float>(tolerance)) );
}


//####################################################################################
//##
//##    Qt Math
//##
//####################################################################################
QTransform CreateRotatedQTransform(QPointF offset, double angle_in_degrees) {
    return QTransform().translate(offset.x(), offset.y()).rotate(angle_in_degrees).translate(-offset.x(), -offset.y());
}
QTransform CreateRotatedQTransform(DrPointF offset, double angle_in_degrees) {
    return CreateRotatedQTransform(ToQPointF(offset), angle_in_degrees);
}
// Returns a Rect around center point with sides length of rect_size
QRect RectAtPoint(QPoint center, int rect_size) {
    return QRect(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}
// Returns a RectF around center point with sides length of rect_size
QRectF RectFAtPoint(QPointF center, double rect_size) {
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}

//####################################################################################
//##
//##    User Interaction
//##
//####################################################################################
// Shows a modal Error Message
void ShowErrorMessage(std::string function_name, std::string error_message, QWidget *parent) {
    std::string error = "Error from " + function_name + "(): " + error_message;
    QMessageBox msg_box(QMessageBox::Icon::Critical, "Error!", QString::fromStdString(error), QMessageBox::StandardButton::Ok, parent);
    msg_box.exec();
}

// Show a Modal Message Box with Icon or Pixmap
QMessageBox::StandardButton ShowMessageBox(std::string message, QPixmap pixmap, QWidget *parent, QMessageBox::StandardButtons buttons) {
    ///QMessageBox msg_box(QMessageBox::Icon::NoIcon, "", QString::fromStdString(message), buttons, parent);
    ///msg_box.setIconPixmap(pixmap);
    ///QMessageBox::StandardButton result = static_cast<QMessageBox::StandardButton>(msg_box.exec());v

    FormMessageBox msg_box(message, pixmap, parent, buttons);
    QMessageBox::StandardButton result = msg_box.execute();

    // Return Button Pressed from msg_box
    return result;
}

QMessageBox::StandardButton ShowMessageBox(std::string message, QMessageBox::Icon icon, QWidget *parent, QMessageBox::StandardButtons buttons) {
    // Select Appropriate Icon
    QString icon_string = "";
    switch (icon) {
        case QMessageBox::Icon::NoIcon:         break;
        case QMessageBox::Icon::Warning:        icon_string = ":/assets/gui_misc/msg_warning";      break;
        case QMessageBox::Icon::Critical:       icon_string = ":/assets/gui_misc/msg_critical";     break;
        case QMessageBox::Icon::Question:       icon_string = ":/assets/gui_misc/msg_question";     break;
        case QMessageBox::Icon::Information:    icon_string = ":/assets/gui_misc/msg_info";         break;
    }

    // Style Icon
    QPixmap pix;
    if (icon_string != "") {
        // Replace All Colors Effect
        ///pix = QPixmap::fromImage( Dr::ColorizeImage(pix.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator))) );
        // Overlay Top Image Icon
        ///QPixmap top = QPixmap(icon_string + "_top.png").scaled(52, 52, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ///pix = QPixmap::fromImage( Dr::OverlayImage(pix.toImage(), top.toImage(), 1.0) );
        // Outer Glow Effect
        ///pix = QPixmap::fromImage( Dr::ApplyBorderToImage(pix.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)), 2, 3) );

        // Simple Icon
        pix = QPixmap(icon_string + ".png").scaled(Dr::Scale(54), Dr::Scale(54), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Colorize Effect
        if (icon == QMessageBox::Icon::Information) {
            QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect();
            colorize->setStrength(1.0);
            colorize->setColor( Dr::ToQColor( Dr::GetColor(Window_Colors::Icon_Dark) ));
            pix = QPixmap::fromImage( Dr::ApplyEffectToImage(pix.toImage(), colorize, 0) );
        }

        // Drop Shadow Effect
        QGraphicsDropShadowEffect *drop_shadow = new QGraphicsDropShadowEffect();
        drop_shadow->setOffset(Dr::Scale(-1), Dr::Scale(2));
        drop_shadow->setBlurRadius(3);
        if (Dr::GetColorScheme() == Color_Scheme::Light) {
            drop_shadow->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Text)) );
        } else {
            drop_shadow->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)) );
        }
        pix = QPixmap::fromImage( Dr::ApplyEffectToImage(pix.toImage(), drop_shadow, 3) );
    }

    ///QMessageBox msg_box(icon, QString::fromStdString(title), QString::fromStdString(message), buttons, parent);
    ///QMessageBox msg_box(QMessageBox::Icon::NoIcon, "", QString::fromStdString(message), buttons, parent);
    ///msg_box.setIconPixmap(pix);
    ///QMessageBox::StandardButton result = static_cast<QMessageBox::StandardButton>(msg_box.exec());

    FormMessageBox msg_box(message, pix, parent, buttons);
    QMessageBox::StandardButton result = msg_box.execute();

    // Return Button Pressed from msg_box
    return result;
}



}   // namespace Dr















