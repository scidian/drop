//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful misc functions
//
//
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
//##        Returns System time as string
//####################################################################################
QString CurrentTimeAsString() {
    return QTime().currentTime().toString() + "." + QString::number(QTime().currentTime().msec());
}


//####################################################################################
//##        High-Res Timer Functions
//####################################################################################
double MillisecondsElapsed(const DrTime &timer) {
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - timer).count() /  1000000.0);
}
void ResetTimer(DrTime &timer) { timer = Clock::now(); }


//####################################################################################
//##        Makes sure scale is not zero so we don't mess up QGraphicsItem transform
//####################################################################################
double CheckScaleNotZero(double scale_to_check) {
    if (scale_to_check <  .001 && scale_to_check >= 0) scale_to_check =  .001;
    if (scale_to_check > -.001 && scale_to_check <= 0) scale_to_check = -.001;
    return scale_to_check;
}


//####################################################################################
//##        Returns true if 'number_desired' is within +-'tolerance' of 'number_to_check'
//####################################################################################
bool IsCloseTo(double number_desired, double number_to_check, double tolerance) {
    return ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) );
}


//####################################################################################
//##        Angle Comparision Functions
//####################################################################################
// Returns true of the two angles are parrallel or perpendicular
bool IsSimilarAngle(double angle1, double angle2, double tolerance) {
    while (angle1 >= 90) { angle1 -= 90; }
    while (angle1 <   0) { angle1 += 90; }
    while (angle2 >= 90) { angle2 -= 90; }
    while (angle2 <   0) { angle2 += 90; }
    return IsCloseTo(angle1, angle2, tolerance);
}

// Returns true is 'check_angle' in equal to 0, 90, 180, or 270, i.e. "square" angle
bool IsSquare(double check_angle) {
    check_angle = abs(check_angle);
    while (check_angle >= 360) check_angle -= 360;
    if (qFuzzyCompare(check_angle, 0))   return true;
    if (qFuzzyCompare(check_angle, 90))  return true;
    if (qFuzzyCompare(check_angle, 180)) return true;
    if (qFuzzyCompare(check_angle, 270)) return true;
    return false;
}

// Returns 'angle' rounded to the nearest 90 multiple of 'angle_to_find'
double Closest90DegreeAngle(double angle, double angle_to_find) {
    double closest_angle =    0;
    double distance_apart = 180;
    for (double d = angle_to_find - 1080; d < angle_to_find + 1080; d += 90.0) {
        if (abs(d - angle) < distance_apart) {
            closest_angle = d;
            distance_apart = abs(d - angle);
        }
    }
    return closest_angle;
}


//####################################################################################
//##        Trimms double to max_decimal_places, and then removes any trailing zeros
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
//##        Custom Font Functions
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
//##        These functions check the width of text that would be drawn
//##        with a particular font
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
//##        Used to show a modal error message
//####################################################################################
void ShowErrorMessage(QString function_name, QString error_message) {
    QMessageBox::warning(nullptr, "Error", "Error from " + function_name + "(): " + error_message);
}


//####################################################################################
//##        Used to show a modal messagebox
//####################################################################################
void ShowMessageBox(QString new_message, QPixmap pixmap) {
    QMessageBox *msg_box = new QMessageBox(nullptr);

    msg_box->setText(new_message);
    msg_box->setIconPixmap(pixmap);
    msg_box->exec();

    delete msg_box;
    ///QMessageBox::about(nullptr, "Sample Box", "Shows a message box");
}


}   // namespace Dr













