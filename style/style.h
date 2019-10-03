//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful Style Functions
//
//
#ifndef STYLE_WIDGET_H
#define STYLE_WIDGET_H

#include <QBitmap>
#include <QLabel>
#include <QPainter>
#include <QScreen>
#include <QWidget>


//####################################################################################
//##    Local Enumerations
//############################
enum class Shadow_Types {
    Button_Shadow,
    Tool_Tip_Shadow,
};


//####################################################################################
//##    A library of helpful functions within the global Dr namespace
//############################
namespace Dr {

// ***** Form / Widget Functions
QString     CustomStyleSheetFormatting();

void        ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
void        ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
void        ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method = 0);
void        ApplyPopupMask(QWidget *widget, int x_radius, int y_radius, bool below = true);
void        CenterFormOnScreen(QWidget *parent_to_find_screen_from, QWidget *form_to_center,
                               double width_percentage = -1, double height_percentage = -1);
QScreen*    FindScreenFromWidget(QWidget *widget);

QString     StyleSheetColorButton(QColor color, QColor text_color, QColor highlight,
                                  int tl_radius, int tr_radius, int bl_radius, int br_radius,
                                  bool left_border, bool right_border, QString name);
QString     StyleSheetToolBar();
QString     StyleSheetToolBarButtons(QString button_name, QString icon_unchecked, QString icon_checked, QString icon_disabled,
                                     int padding, int tl, int tr, int bl, int br, bool checkable);
QString     StyleSheetToolBarModeButton(QString button_name, QString icon_name, QString icon_faded_name);
QString     StyleSheetRecessedBackgroundBorder(int top_percent, int bottom_percent, bool highlight = false);
QString     StyleSheetPoppedOutBackgroundBorder(QColor background_color, QColor border_color, int top_percent,
                                                int bottom_percent, bool highlight = false);

// ***** QPixmap Functions
QImage      ImageMask(const QImage& from_image, QColor mask_color, int max_alpha);
void        SetMaskCircleShape (QPixmap &pix);


}

#endif // STYLE_WIDGET_H













