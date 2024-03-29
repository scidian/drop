﻿//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Studios, All Rights Reserved
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

#include "core/types/dr_color.h"


//####################################################################################
//##    Local Enumerations
//############################
enum class Shadow_Types {
    Button_Shadow,
    Button_Shadow_Thin,
    Tool_Tip_Shadow,
};


//####################################################################################
//##    A library of helpful functions within the global Dr namespace
//############################
namespace Dr {

    // ***** Widget Help
    void        ClearWidget(QWidget *widget);
    void        DeleteLayout(QLayout *layout);

    // ***** Form / Widget Formatting
    void        ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
    void        ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
    void        ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method = 0);
    void        ApplyPopupMask(QWidget *widget, int x_radius, int y_radius, bool below = true);


    // ***** Form Location Handling
    void        CenterFormOnScreen(QWidget *parent_to_find_screen_from, QWidget *form_to_center,
                                   double width_percentage = -1, double height_percentage = -1);
    void        CenterFormOnParent(QWidget *parent_form, QWidget *form_to_center);
    QScreen*    FindScreenFromWidget(QWidget *widget);


    // ***** Style Sheets
    QString     CustomStyleSheetFormatting();
    std::string StyleSheetColorButton(DrColor color, DrColor text_color, DrColor highlight,
                                      int tl_radius, int tr_radius, int bl_radius, int br_radius,
                                      bool left_border, bool right_border, std::string name);
    std::string StyleSheetToolBar();
    std::string StyleSheetToolBarButtons(std::string button_name, std::string icon_unchecked, std::string icon_checked, std::string icon_disabled,
                                         int padding, int tl, int tr, int bl, int br, bool checkable, int custom_width = 0, int custom_height = 0);
    std::string StyleSheetToolBarModeButton(std::string button_name, std::string icon_name, std::string icon_faded_name,
                                            int padding = 6, int custom_width = 0, int custom_height = 0);
    std::string StyleSheetRecessedBackgroundBorder(int top_percent, int bottom_percent, bool highlight = false);
    std::string StyleSheetPoppedOutBackgroundBorder(DrColor background_color, DrColor border_color, int top_percent,
                                                    int bottom_percent, bool highlight = false);


}

#endif // STYLE_WIDGET_H













