//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Subclassed FormPopup used to allow color picking with sweet options
//
//
#include <QButtonGroup>

#include "core/types/dr_color.h"
#include "editor/form_main/form_main.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/trees/tree_inspector.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormPopupColor::FormPopupColor(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset, int y_offset)
    : FormPopup(parent, project, widget_to_use_for_mapToGlobal, x_offset, y_offset) {

}


//####################################################################################
//##    Builds a popup with a lot of colors to choose from
//####################################################################################
void FormPopupColor::buildPopupColors(QWidget *wants_color, DrColor start_color) {

    // ***** Initialize form variables and settings
    m_wants_return_variable = wants_color;
    m_start_color = start_color;
    m_current_palette = Color_Palettes::Material;            // !!!!! #TEMP:

    this->setFixedSize(210, 310);

    // ********* Buttons for changing pages in the QStackWidget
    PopupPushButtonPageSelect *change0 = new PopupPushButtonPageSelect(getInnerWidget(), this, tr("Color Palette") );
    change0->setObjectName("buttonColorPalette");
    change0->setToolTip(tr("Color Palette"));
    change0->setGeometry( 159, 7, 41, 28);
    change0->setCheckable(true);

    PopupPushButtonPageSelect *change1 = new PopupPushButtonPageSelect(getInnerWidget(), this, tr("Color History") );
    change1->setObjectName("buttonColorHistory");
    change1->setToolTip(tr("Color History"));
    change1->setGeometry(159, 35, 41, 28);
    change1->setCheckable(true);

    QButtonGroup *color_pages = new QButtonGroup();
    color_pages->setExclusive(true);
    color_pages->addButton(change0, 0);
    color_pages->addButton(change1, 1);
    color_pages->button( Dr::GetPreference(Preferences::Color_Popup_Tab).toInt() )->setChecked(true);

    connect(change0, &QPushButton::clicked, this, [this, color_pages]() {
        m_palette_block->setCurrentIndex(0);
        for (auto button : color_pages->buttons()) button->raise();     });

    connect(change1, &QPushButton::clicked, this, [this, color_pages]() {
        m_palette_block->setCurrentIndex(1);
        for (auto button : color_pages->buttons()) button->raise();     });


    // ********** Widget for the first page, Material Palette
    QVBoxLayout *layout = new QVBoxLayout( this->getInnerWidget()) ;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(4);

        m_color_label = new QLabel();
        m_color_label->setAlignment(Qt::AlignCenter);
        m_color_label->setFixedSize(192, 20);
            QFont font = Dr::CustomFont();
            m_color_label->setFont( font );
        setInfoLabelColor(start_color);

        // ***** Basic
        QWidget *basic =    createColorBlock(Colors::Basic, 0, 12,  1, 0, 11, 11, 0, 1, 1);
        layout->addWidget(basic);

        // ***** Grays
        QWidget *grays =    createColorBlock(Colors::Grays, 2, 12,  3, 0, 11, 11, 0, 1, 1);
        layout->addWidget(grays);

        // ***** Main Block of Colors
        m_palette_block = new QStackedWidget();
        m_palette_block->setFixedSize(195, 195);

        QWidget *first_page =  createColorBlock(Colors::Main, 32, 16, 16, 4, 11, 11, 0, 1, 1);
        QWidget *second_page = createColorBlock(Colors::History, 0, 6, 6, 0, 27, 27, 0, 5, 5);

        m_palette_block->addWidget(first_page);
        m_palette_block->addWidget(second_page);
        m_palette_block->setCurrentIndex( Dr::GetPreference(Preferences::Color_Popup_Tab).toInt() );

        layout->addWidget(m_palette_block);

        // ********** Accent Colors
        ///QWidget *accent_colors = CreateColorBlock(popup, wants_color, info_label, 42, 16, 4, 10, 11, 11, 0, 1, Colors::Accent);
        ///layout->addWidget(accent_colors);

        layout->addWidget(m_color_label);
}


//####################################################################################
//##    Creates a block group of color buttons from palette colors
//####################################################################################
QWidget* FormPopupColor::createColorBlock(Colors type, int start_index, int columns, int rows, int mid_step,
                                          int block_width, int block_height, int border, int x_spacing, int y_spacing) {
    QWidget *color_block = new QWidget();

    int width =  (columns * block_width)  + (x_spacing * (columns - 1)) + 4;
    int height = (rows    * block_height) + (y_spacing * (rows    - 1)) + 4;
    color_block->setFixedSize(width, height);
    color_block->setContentsMargins(0, 0, 0, 0);

    // If normal palette load, start index at 0, 0 extra color skipping on each loop
    if (m_current_palette != Color_Palettes::Material && type == Colors::Main) {
        start_index = 0;
        mid_step =    0;
    }
    int color_index = start_index;

    // Figure out if we're drawing these top to bottom, or left to right
    bool horizontal = (type == Colors::Basic || type == Colors::Grays || type == Colors::History);
    int x = (horizontal) ? rows : columns;
    int y = (horizontal) ? columns : rows;

    // Find color history if necessary
    std::vector<DrVariant> history;
    if (type == Colors::History) {
        history = Dr::GetPreference(Preferences::Color_Popup_History).toVector();
        if (history.size() < 1) return color_block;
    }

    // Loop through array and make a grid of color labels
    DrColor color;
    for (int i = 0; i < x; i++)  {
        for (int j = 0; j < y; j++) {

            if (m_current_palette != Color_Palettes::Material && type == Colors::Main) {
                color = Dr::GetColorFromPalette(m_current_palette, color_index);
                ++color_index;
            } else {
                switch (type) {
                    case Colors::Basic:
                            color = Dr::GetColorFromPalette(Color_Palettes::Basic, color_index);
                            ++color_index;
                        break;
                    case Colors::Accent:
                    case Colors::Grays:
                    case Colors::Main:
                        if (j < 10) {
                            color = Dr::GetColorFromPalette(Color_Palettes::Material, color_index);
                            ++color_index;
                        } else {
                            color = color.darker( 150 );
                            color.setAlphaF(1.0);
                        }
                        break;
                    case Colors::History:
                        color = history.at(color_index).toColor();
                        ++color_index;
                        break;
                }
            }

            // Skip this color if transparent
            if (color == Qt::transparent) continue;

            // Otherwise create a new color button
            PopupPushButtonColorSelect *button = new PopupPushButtonColorSelect(color_block, this, color, block_width, block_height);
            if (!horizontal) button->setGeometry(i * (block_width + x_spacing) + 2, j * (block_height + y_spacing) + 2, block_width, block_height);
            else             button->setGeometry(j * (block_width + x_spacing) + 2, i * (block_height + y_spacing) + 2, block_width, block_height);

            if (color.rgb() == m_start_color.rgb()) {
                button->move( button->pos() - QPoint(1, 1) );
                button->setFixedSize(block_width + 2, block_height + 2);
            }

            QString color_as_string = "R: " + QString::number(color.red()) + ", G: " + QString::number(color.green()) + ", B: " + QString::number(color.blue());
            button->setToolTip( color_as_string );
            button->setStyleSheet( QString::fromStdString(createButtonStyleSheet(color, border)) );

            if (type == Colors::History) {  if (color_index >= static_cast<int>(history.size())) break;
            } else {                        if (color_index >= Dr::GetPaletteColorCount(m_current_palette)) break;  }
        }
        color_index += mid_step;

        if (type == Colors::History) {      if (color_index >= static_cast<int>(history.size())) break;
        } else {                            if (color_index >= Dr::GetPaletteColorCount(m_current_palette)) break;  }
    }

    return color_block;
}

// Creates initial style sheet text for little color boxes
std::string FormPopupColor::createButtonStyleSheet(DrColor color, int border) {
    std::string style;
    if (color.rgb() == m_start_color.rgb()) {
        style = "QPushButton {  "
                "   border: " + std::to_string(border + 2) + "px solid; border-radius: 2px; "
                "   border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
                "   background-color: " + color.name() + "; }";
    } else {
        style = "QPushButton {  "
                "   border: " + std::to_string(border) + "px solid; border-radius: 0px; "
                "   border-color: " + color.darker(175).name() + "; "
                "   background-color: " + color.name() + "; }";
    }
    style +=
        "QPushButton::hover { "
        "   border: " + std::to_string(border + 2) + "px solid; border-radius: 3px; "
        "   border-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } ";
    return style;
}


//####################################################################################
//##    Updates the info label color and text
//####################################################################################
void FormPopupColor::setInfoLabelColor(DrColor color) {
    bool white = (color.red() < 160 && color.green() < 160 && color.blue() < 160);
    std::string style =
                    "   background-color: " + color.name() + "; color: ";
                style += white ? "white; " : "black; ";
                style +=
                    "   border: 1px solid; border-radius: 4px;"
                    "   border-color: " + color.darker(200).name() + "; ";
    m_color_label->setStyleSheet( QString::fromStdString(style) );
    QString text = QString::fromStdString(color.name()).toUpper();
    m_color_label->setText( text );
}


//####################################################################################
//##    On close, save current tab for later
//####################################################################################
void FormPopupColor::closeEvent(QCloseEvent *event) {
    Dr::SetPreference(Preferences::Color_Popup_Tab, m_palette_block->currentIndex());
    FormPopup::closeEvent(event);
}



//####################################################################################
//##    ColorLabel Class Functions
//####################################################################################
PopupPushButtonPageSelect::PopupPushButtonPageSelect(QWidget *parent, FormPopupColor *popup, QString description) :
    QPushButton(parent), m_popup(popup), m_description(description) {
    setAttribute(Qt::WA_Hover);
}
PopupPushButtonPageSelect::~PopupPushButtonPageSelect() { }

void PopupPushButtonPageSelect::enterEvent(QEnterEvent *) {
    m_text_before = m_popup->getColorLabel()->text();
    m_popup->getColorLabel()->setText( m_description );
}
void PopupPushButtonPageSelect::leaveEvent(QEvent *) {
    m_popup->getColorLabel()->setText( m_text_before );
}


//####################################################################################
//##    ColorLabel Class Functions
//####################################################################################
PopupPushButtonColorSelect::PopupPushButtonColorSelect(QWidget *parent, FormPopupColor *popup, DrColor my_color, int width, int height) :
    QPushButton(parent), m_popup(popup), m_color(my_color), m_width(width), m_height(height) {
    setAttribute(Qt::WA_Hover);
    ///setToolTip("R: " + QString::number(m_color.red()) + ", B: " + QString::number(m_color.blue()) + ", G: " + QString::number(m_color.green()));
}
PopupPushButtonColorSelect::~PopupPushButtonColorSelect() { }

void PopupPushButtonColorSelect::enterEvent(QEnterEvent *) {
    if (m_color == m_popup->getStartColor()) {
        this->move( this->pos() - QPoint(1, 1) );
    } else {
        this->move( this->pos() - QPoint(2, 2) );
    }

    this->raise();
    this->setFixedSize(m_width + 4, m_height + 4);

    m_popup->setInfoLabelColor(m_color);
}

void PopupPushButtonColorSelect::leaveEvent(QEvent *) {
    if (m_color == m_popup->getStartColor()) {
        this->setFixedSize(m_width + 2, m_height + 2);
        this->move( this->pos() + QPoint(1, 1) );
    } else {
        this->setFixedSize(m_width, m_height);
        this->move( this->pos() + QPoint(2, 2) );
    }
}

void PopupPushButtonColorSelect::mouseReleaseEvent(QMouseEvent *) {
    emit m_popup->colorGrabbed( m_popup->getReturnWidget(), m_color );
    m_popup->close();
}














