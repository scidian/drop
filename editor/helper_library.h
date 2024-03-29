//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      To be used with Qt Library types
//
//
#ifndef HELPER_EDITOR_H
#define HELPER_EDITOR_H

#include <QBitmap>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QScreen>
#include <QWidget>

// Forward Declarations
class DrBitmap;
class DrColor;
class DrPoint;
class DrPointF;
class DrVariant;


//####################################################################################
//##    A library of helpful Qt based functions within the global Dr namespace
//############################
namespace Dr {


    // ***** Time Functions
    QString     CurrentTimeAsString();


    // ***** Comparison Functions
    double      CheckScaleNotZero(double scale_to_check);
    QString     RemoveTrailingDecimals(double value, int max_decimal_places);


    // ***** Custom Font Functions
    QFont       CustomFont(int add_points = 0, bool scale = true);
    QFont       CustomFontLarger(bool scale = true);
    QString     FontFamily();
    int         FontSize(bool scale = true);
    void        LoadCustomFonts();


    // ***** String Functions
    int         CheckFontWidth(QFont font, QString text_to_check);
    QString     FitStringToWidth(QFont font, QString text_to_check, int max_width = 80, bool use_dots = true);
    QString     StringFromBool(bool boolean);
    QString     FindCopyName(QString original, std::vector<QString> others);


    // ***** DrLibary Misc Conversion
    DrColor     FromQColor(const QColor &color);
    DrPoint     FromQPoint(const QPoint &point);
    DrPointF    FromQPointF(const QPointF &pointf);
    QColor      ToQColor(const DrColor &color);
    QPoint      ToQPoint(const DrPoint &point);
    QPointF     ToQPointF(const DrPointF &pointf);

    // ***** DrLibrary Image Conversion
    DrBitmap    FromQImage(const QImage &image);
    DrBitmap    FromQPixmap(const QPixmap &pix);
    QImage      ToQImage(const DrBitmap &bitmap);
    QPixmap     ToQPixmap(const DrBitmap &bitmap);


    // ***** Qt Comparison
    bool        SameQColor(QColor color1, QColor color2, double tolerance);


    // ***** Qt Math
    QTransform  CreateRotatedQTransform(QPointF  offset, double angle_in_degrees);
    QTransform  CreateRotatedQTransform(DrPointF offset, double angle_in_degrees);
    QRect       RectAtPoint(QPoint center,   int rect_size);
    QRectF      RectFAtPoint(QPointF center, double rect_size);


    // ***** User Interaction
    void                        ShowErrorMessage(std::string function_name, std::string error_message, QWidget *parent = nullptr);
    QMessageBox::StandardButton ShowMessageBox(std::string message, QPixmap pixmap, QWidget *parent = nullptr,
                                               QMessageBox::StandardButtons buttons = QMessageBox::StandardButton::Ok);
    QMessageBox::StandardButton ShowMessageBox(std::string message, QMessageBox::Icon icon = QMessageBox::Icon::NoIcon, QWidget *parent = nullptr,
                                               QMessageBox::StandardButtons buttons = QMessageBox::StandardButton::Ok);




}   // End namespace Dr

#endif // HELPER_EDITOR_H


















