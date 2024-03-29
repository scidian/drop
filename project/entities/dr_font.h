//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRFONT_H
#define DRFONT_H

#include "core/types/dr_color.h"
#include "core/types/dr_point.h"
#include "core/types/dr_pointf.h"
#include "core/types/dr_rect.h"
#include "project/settings/settings.h"

// Forward Declarations
class DrProject;


//####################################################################################
//##    DrFont
//##        Class to hold a Font for use in a Project
//############################
class DrFont : public DrSettings
{
private:
    // Local Variables
    DrBitmap                m_bitmap;                       // Stored font bitmap
    std::map<char, DrRect>  m_positions;                    // Holds rects for each character in the pixmap
    std::map<char, DrPoint> m_spacing;                      // Horizontal and vertical spacing for each character

    std::string             m_name;                         // Name of this font within the Project

    // Font Properties - NEED TO INCORPORATE FOR EDITOR
    std::string p_font_family;              // Font to use
    int         p_font_size;                // Point size
    int         p_font_padding;             // Extra letter spacing
    DrColor     p_main_color;               // Main font color

    bool        p_outline;
    DrColor     p_outline_color;
    int         p_outline_width;

    bool        p_shadow;
    DrColor     p_shadow_color;
    DrPoint     p_shadow_offset;

    bool        p_gradient;
    DrColor     p_gradient_color;


public:
    // Constructor
    DrFont(DrProject *parent_project, long key, std::string font_name, DrBitmap font_pixmap, std::string font_family, int font_size, bool use_test_rects = false);

    // DrSettings Overrides
    virtual DrType      getType() override              { return DrType::Font; }

    // Getters / Setters
    DrBitmap            getBitmap()                     { return m_bitmap; }

    DrRect              getCharRect(char character)     { return m_positions[character]; }
    DrPoint             getCharSpacing(char character)  { return m_spacing[character]; }

    std::string         getPropertyFontFamily()         { return p_font_family; }
    int                 getPropertyFontSize()           { return p_font_size; }

    void                setTestFontRects();

};

#endif // DRFONT_H









