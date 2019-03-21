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

    QString                 m_font_name;                    // name of this font
    QPixmap                 m_font_pixmap;                  // stored font pixmap
    std::map<char, QRect>   m_positions;                    // holds rects for each character in the pixmap
    std::map<char, QSize>   m_spacing;                      // horizontal and vertical spacing for each character

public:
    DrFont(DrProject *parent_project, long key, QString font_name, QPixmap font_pixmap);

    // Function Calls
    QPixmap         createText(QString text);

    // Getters and Setters
    virtual DrType  getType() override      { return DrType::Font; }

    DrProject*      getParentProject()      { return m_parent_project; }
    QString         getName()               { return m_font_name; }
    QPixmap         getFontPixmap()         { return m_font_pixmap; }

    QRect           getCharRect(char character)     { return m_positions[character]; }
    QSize           getCharSpacing(char character)  { return m_spacing[character]; }

    void            setTestFontRects();

};

#endif // PROJECT_FONT_H
