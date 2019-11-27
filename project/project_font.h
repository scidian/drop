//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRFONT_H
#define DRFONT_H

#include <QPixmap>
#include <QString>

#include "settings/settings.h"

// Forward declarations
class DrProject;

//####################################################################################
//##    DrFont
//##        Class to hold a Font for use in a Project
//############################
class DrFont : public DrSettings
{
private:
    // Local Variables
    QPixmap                 m_pixmap;                       // Stored font pixmap
    std::map<char, QRect>   m_positions;                    // Holds rects for each character in the pixmap
    std::map<char, QSize>   m_spacing;                      // Horizontal and vertical spacing for each character

    QString                 m_name;                         // Name of this font within the Project


    // Font Properties - NEED TO INCORPORATE FOR EDITOR
    QString     p_font_family;              // Font to use
    int         p_font_size;                // Point size
    int         p_font_padding;             // Extra letter spacing
    QColor      p_main_color;               // Main font color

    bool        p_outline;
    QColor      p_outline_color;
    int         p_outline_width;

    bool        p_shadow;
    QColor      p_shadow_color;
    QPoint      p_shadow_offset;

    bool        p_gradient;
    QColor      p_gradient_color;


public:
    // Constructor
    DrFont(DrProject *parent_project, long key, QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects = false);

    // DrSettings Overrides
    virtual DrType      getType() override      { return DrType::Font; }

    // Function Calls
    QPixmap         createText(QString text);

    // Getters / Setters
    QPixmap         getPixmap()                     { return m_pixmap; }

    QRect           getCharRect(char character)     { return m_positions[character]; }
    QSize           getCharSpacing(char character)  { return m_spacing[character]; }

    QString         getPropertyFontFamily()         { return p_font_family; }
    int             getPropertyFontSize()           { return p_font_size; }
    void            setPropertyFontFamily(QString family)   { p_font_family = family; }
    void            setPropertyFontSize(int size)           { p_font_size = size; }

    void            setTestFontRects();

};

#endif // DRFONT_H









