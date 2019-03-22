//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "project_font.h"


DrFont::DrFont(DrProject *parent_project, long key, QString font_name, QPixmap font_pixmap, bool use_test_rects) :
    m_parent_project(parent_project)
{
    setKey(key);

    m_name =  font_name;
    m_pixmap = font_pixmap;

    if (use_test_rects)
        setTestFontRects();
}

// !!! Need to incorporate letter spacing
QPixmap DrFont::createText(QString text)
{
    int width = 0;
    int height = m_positions['A'].height();
    for (int i = 0; i < text.length(); i++) {
        width += m_positions[ text.at(i).toLatin1() ].width();
    }
    if (width ==  0) width =  1;
    if (height == 0) height = 1;
    QPixmap text_image(width, height);
    text_image.fill(Qt::transparent);

    QPainter painter(&text_image);
    painter.setPen(Qt::NoPen);

    int x = 0;
    for (int i = 0; i < text.length(); i++) {
        char letter = text.at(i).toLatin1();
        int  letter_width = m_positions[letter].width();

        painter.drawPixmap( QRect(x, 0, letter_width, height), m_pixmap, m_positions[letter] );
        x += letter_width;
    }

    return text_image;
}


void DrFont::setTestFontRects()
{
    m_positions['A'] = QRect(  0,   0, 32, 32);
    m_positions['B'] = QRect( 32,   0, 32, 32);
    m_positions['C'] = QRect( 64,   0, 32, 32);
    m_positions['D'] = QRect( 96,   0, 32, 32);
    m_positions['E'] = QRect(128,   0, 32, 32);
    m_positions['F'] = QRect(160,   0, 32, 32);
    m_positions['G'] = QRect(192,   0, 32, 32);
    m_positions['H'] = QRect(224,   0, 32, 32);
    m_positions['I'] = QRect(256,   0, 32, 32);
    m_positions['J'] = QRect(288,   0, 32, 32);
    m_positions['K'] = QRect(320,   0, 32, 32);
    m_positions['L'] = QRect(352,   0, 32, 32);
    m_positions['M'] = QRect(384,   0, 32, 32);
    m_positions['N'] = QRect(416,   0, 32, 32);
    m_positions['O'] = QRect(448,   0, 32, 32);
    m_positions['P'] = QRect(480,   0, 32, 32);

    m_positions['Q'] = QRect(  0,  32, 32, 32);
    m_positions['R'] = QRect( 32,  32, 32, 32);
    m_positions['S'] = QRect( 64,  32, 32, 32);
    m_positions['T'] = QRect( 96,  32, 32, 32);
    m_positions['U'] = QRect(128,  32, 32, 32);
    m_positions['V'] = QRect(160,  32, 32, 32);
    m_positions['W'] = QRect(192,  32, 32, 32);
    m_positions['X'] = QRect(224,  32, 32, 32);
    m_positions['Y'] = QRect(256,  32, 32, 32);
    m_positions['Z'] = QRect(288,  32, 32, 32);
    m_positions['a'] = QRect(320,  32, 32, 32);
    m_positions['b'] = QRect(352,  32, 32, 32);
    m_positions['c'] = QRect(384,  32, 32, 32);
    m_positions['d'] = QRect(416,  32, 32, 32);
    m_positions['e'] = QRect(448,  32, 32, 32);
    m_positions['f'] = QRect(480,  32, 32, 32);

    m_positions['g'] = QRect(  0,  64, 32, 32);
    m_positions['h'] = QRect( 32,  64, 32, 32);
    m_positions['i'] = QRect( 64,  64, 32, 32);
    m_positions['j'] = QRect( 96,  64, 32, 32);
    m_positions['k'] = QRect(128,  64, 32, 32);
    m_positions['l'] = QRect(160,  64, 32, 32);
    m_positions['m'] = QRect(192,  64, 32, 32);
    m_positions['n'] = QRect(224,  64, 32, 32);
    m_positions['o'] = QRect(256,  64, 32, 32);
    m_positions['p'] = QRect(288,  64, 32, 32);
    m_positions['q'] = QRect(320,  64, 32, 32);
    m_positions['r'] = QRect(352,  64, 32, 32);
    m_positions['s'] = QRect(384,  64, 32, 32);
    m_positions['t'] = QRect(416,  64, 32, 32);
    m_positions['u'] = QRect(448,  64, 32, 32);
    m_positions['v'] = QRect(480,  64, 32, 32);

    m_positions['w'] = QRect(  0,  96, 32, 32);
    m_positions['x'] = QRect( 32,  96, 32, 32);
    m_positions['y'] = QRect( 64,  96, 32, 32);
    m_positions['z'] = QRect( 96,  96, 32, 32);
    m_positions['0'] = QRect(128,  96, 32, 32);
    m_positions['1'] = QRect(160,  96, 32, 32);
    m_positions['2'] = QRect(192,  96, 32, 32);
    m_positions['3'] = QRect(224,  96, 32, 32);
    m_positions['4'] = QRect(256,  96, 32, 32);
    m_positions['5'] = QRect(288,  96, 32, 32);
    m_positions['6'] = QRect(320,  96, 32, 32);
    m_positions['7'] = QRect(352,  96, 32, 32);
    m_positions['8'] = QRect(384,  96, 32, 32);
    m_positions['9'] = QRect(416,  96, 32, 32);
    m_positions['.'] = QRect(448,  96, 32, 32);
    m_positions[' '] = QRect(480,  96, 32, 32);
}





















