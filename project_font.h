//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PROJECT_FONT_H
#define PROJECT_FONT_H

#include <QPixmap>
#include <QString>

#include "settings.h"

class DrProject;

class DrFont : public DrSettings
{
private:
    DrProject              *m_parent_project;               // holds reference to parent Project class that handles key generation for project

    QPixmap                 m_pixmap;                       // stored font pixmap
    std::map<char, QRect>   m_positions;                    // holds rects for each character in the pixmap
    std::map<char, QSize>   m_spacing;                      // horizontal and vertical spacing for each character

    QString                 m_name;                         // name of this font within the Project


    // Font properties - NEED TO INCORPORATE FOR EDITOR
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
    DrFont(DrProject *parent_project, long key, QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects = false);

    // Function Calls
    QPixmap         createText(QString text);

    // Getters and Setters
    virtual DrType  getType() override      { return DrType::Font; }

    DrProject*      getParentProject()      { return m_parent_project; }
    QString         getName()               { return m_name; }
    QPixmap         getPixmap()             { return m_pixmap; }

    QRect           getCharRect(char character)     { return m_positions[character]; }
    QSize           getCharSpacing(char character)  { return m_spacing[character]; }

    QString         getPropertyFontFamily() { return p_font_family; }
    int             getPropertyFontSize()   { return p_font_size; }
    void            setPropertyFontFamily(QString family)   { p_font_family = family; }
    void            setPropertyFontSize(int size)           { p_font_size = size; }

    void            setTestFontRects();

};

#endif // PROJECT_FONT_H









