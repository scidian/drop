//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_tree_inspector.h"
#include "form_main.h"
#include "form_popup.h"
#include "globals.h"

namespace Dr {

enum class Colors {
    Grays,
    Main,
    Accent,
    Bit,
};

QWidget* CreateColorBlock(FormPopup *popup, QWidget *wants_color, QLabel *info_label, int start_index, int columns, int rows,
                          int mid_step, int block_width, int block_height, int border, int spacing, Colors type);
QString  CreateButtonStyleSheet(QColor color, int border);
void     SetInfoLabelColor(QLabel *label, QColor color);


//####################################################################################
//##        Builds a popup with a lot of colors to choose from
//####################################################################################
void BuildPopupColors(QWidget *parent_form, FormPopup *popup, QWidget *wants_color, QColor start_color)
{
    Q_UNUSED (parent_form)

    QFont font;  font.setPointSize(Dr::FontSize());

    popup->setFixedSize(210, 344);

    // ***** Widget for the whole popup form
    QWidget *widget = popup->getWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(8, 5, 10, 10);
    layout->setAlignment(Qt::AlignCenter);

        QLabel *info_label = new QLabel();
        info_label->setAlignment(Qt::AlignCenter);
        info_label->setFixedSize(192, 20);
        info_label->setFont( font );
        SetInfoLabelColor(info_label, start_color);

        // ***** Main Block of Colors  2-11, 12-21, 22-31
        QWidget *grays = CreateColorBlock(popup, wants_color, info_label, 2, 12, 3, 0, 11, 11, 0, 1, Colors::Grays);
        layout->addWidget(grays);

        // ***** Main Block of Colors
        QWidget *main_colors = CreateColorBlock(popup, wants_color, info_label, 32, 16, 15, 4, 11, 11, 0, 1, Colors::Main);
        layout->addWidget(main_colors);

        // ********** Accent Colors
        QWidget *accent_colors = CreateColorBlock(popup, wants_color, info_label, 42, 16, 4, 10, 11, 11, 0, 1, Colors::Accent);
        layout->addWidget(accent_colors);

        // ********** 4 bit colors
        ///QWidget *bit_colors = CreateColorBlock(popup, wants_color, info_label, 0, 16, 1, 0, 11, 11, 0, 1, Colors::Bit);
        ///layout->addWidget(bit_colors);

        layout->addWidget(info_label);
}


// Creates a group of color labels from Material Palette colors
QWidget* CreateColorBlock(FormPopup *popup, QWidget *wants_color, QLabel *info_label, int start_index, int columns, int rows,
                          int mid_step, int block_width, int block_height, int border, int spacing, Colors type)
{
    QWidget *color_block = new QWidget();

    int width =  (columns * block_width)  + (spacing * (columns - 1));
    int height = (rows    * block_height) + (spacing * (rows    - 1));
    if (type == Colors::Grays || type == Colors::Bit) width = ((columns + 6) * block_width)  + (spacing * ((columns + 6) - 1));

    color_block->setFixedSize(width, height);
    color_block->setContentsMargins(0, 0, 0, 0);

    // Figure out if we're drawing these top to bottom, or left to right
    int color_index = start_index;
    int x, y;
    if (type == Colors::Grays || type == Colors::Bit) {
        x = rows;       y = columns;
    } else {
        x = columns;    y = rows;
    }

    // Loop through array and make a grid of color labels
    QColor color;
    for (int i = 0; i < x; i++)  {
        for (int j = 0; j < y; j++) {
            if (type == Colors::Bit) {
                color = Dr::GetColorEGA(color_index);
            } else {
                if (j < 10) color = Dr::GetColorMaterial(color_index);
                else        color = color.darker( 150 );
            }

            ColorSelecterButton *button = new ColorSelecterButton(color_block, popup, wants_color, info_label, color);
            button->setStyleSheet( CreateButtonStyleSheet(color, border) );

            if (type == Colors::Main || type == Colors::Accent)
                button->setGeometry(i * (block_width + spacing), j * (block_height + spacing), block_width, block_height);
            else
                button->setGeometry(j * (block_width + spacing), i * (block_height + spacing), block_width, block_height);

            if (type == Colors::Bit || j < 10) ++color_index;
        }
        color_index += mid_step;
    }

    // Add black and white and 2 bit gray scale
    if (type == Colors::Grays) {
        ColorSelecterButton *white_button = new ColorSelecterButton(color_block, popup, wants_color, info_label, Qt::white);
        white_button->setStyleSheet( CreateButtonStyleSheet(Qt::white, border) );
        white_button->setGeometry((y + 0) * (block_width + spacing), 0, block_width * 2 + spacing * 1, int(block_height * 1.5) + spacing * 1);

        ColorSelecterButton *black_button = new ColorSelecterButton(color_block, popup, wants_color, info_label, Qt::black);
        black_button->setStyleSheet( CreateButtonStyleSheet(Qt::black, border) );
        black_button->setGeometry((y + 2) * (block_width + spacing), 0, block_width * 2 + spacing * 1, int(block_height * 1.5) + spacing * 1);

        ColorSelecterButton *gray1_button = new ColorSelecterButton(color_block, popup, wants_color, info_label, QColor(170, 170, 170));
        gray1_button->setStyleSheet( CreateButtonStyleSheet(QColor(170, 170, 170), border) );
        gray1_button->setGeometry((y + 0) * (block_width + spacing), int(block_height * 1.5) + spacing,
                                  block_width * 2 + spacing * 1,     int(block_height * 1.5) + spacing * 1);

        ColorSelecterButton *gray2_button = new ColorSelecterButton(color_block, popup, wants_color, info_label, QColor(85, 85, 85));
        gray2_button->setStyleSheet( CreateButtonStyleSheet(QColor(85, 85, 85), border) );
        gray2_button->setGeometry((y + 2) * (block_width + spacing), int(block_height * 1.5) + spacing,
                                  block_width * 2 + spacing * 1,     int(block_height * 1.5) + spacing * 1);
    }

    return color_block;
}

// Creates initial style sheet text for little color boxes
QString CreateButtonStyleSheet(QColor color, int border)
{
    return
    "QPushButton {  "
    "   border: " + QString::number(border) + "px solid; border-radius: 0px; "
    "   border-color: " + color.darker(150).name() + "; "
    "   background-color: " + color.name() + "; }"
    "QPushButton::hover { "
    "   border: " + QString::number(border + 1) + "px solid;"
    "   border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; } ";
}

// Updates the info label color and text
void SetInfoLabelColor(QLabel *label, QColor color)
{
    bool white = (color.red() < 160 && color.green() < 160 && color.blue() < 160);
    QString style =
        "   background-color: " + color.name() + "; color: ";
    style += white ? "white; " : "black; ";
    style +=
        "   border: 1px solid; border-radius: 4px;"
        "   border-color: " + color.darker(200).name() + "; ";
    label->setStyleSheet(style);
    //label->setText( "Red: " + QString::number(color.red()) + ", Blue: " + QString::number(color.blue()) + ", Green: " + QString::number(color.green()) );
    label->setText( color.name().toUpper() );
}


}   // namespace Dr



//####################################################################################
//##        ColorLabel Class Functions
//####################################################################################
ColorSelecterButton::ColorSelecterButton(QWidget *parent, FormPopup *popup, QWidget *wants_color, QLabel *info_label, QColor my_color) :
    QPushButton(parent), m_popup(popup), m_widget(wants_color), m_info_label(info_label), m_color(my_color)
{
    setAttribute(Qt::WA_Hover);
}
ColorSelecterButton::~ColorSelecterButton() { }

void ColorSelecterButton::enterEvent(QEvent *)              { Dr::SetInfoLabelColor(m_info_label, m_color); }
void ColorSelecterButton::mouseReleaseEvent(QMouseEvent *)  {
    emit m_popup->colorGrabbed(m_widget, m_color);
    m_popup->close();
}














