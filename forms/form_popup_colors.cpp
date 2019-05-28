//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Subclassed FormPopup used to allow color picking with sweet options
//
//
#include <QButtonGroup>

#include "editor/editor_tree_inspector.h"
#include "form_main.h"
#include "form_popup.h"
#include "globals.h"
#include "helper.h"

//####################################################################################
//##        Constructor
//####################################################################################
FormPopupColor::FormPopupColor(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset, int y_offset)
    : FormPopup(parent, project, widget_to_use_for_mapToGlobal, x_offset, y_offset) { }


//####################################################################################
//##        Builds a popup with a lot of colors to choose from
//####################################################################################
void FormPopupColor::buildPopupColors(QWidget *wants_color, QColor start_color) {

    // ***** Initialize form variables and settings
    m_wants_return_variable = wants_color;
    m_start_color = start_color;
    m_current_palette = Color_Palettes::Material;            // !!!!! #TEMP:

    this->setFixedSize(210, 310);    

    // ********* Buttons for changing pages in the QStackWidget
    DrQPushButtonPageSelect *change0 = new DrQPushButtonPageSelect(getInnerWidget(), this, tr("Color Palette") );
    change0->setObjectName("buttonColorPalette");
    change0->setToolTip(tr("Color Palette"));
    change0->setGeometry( 159, 7, 41, 28);
    change0->setCheckable(true);

    DrQPushButtonPageSelect *change1 = new DrQPushButtonPageSelect(getInnerWidget(), this, tr("Color History") );
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
            QFont font;
            font.setPointSize(Dr::FontSize());
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
//##        Creates a block group of color buttons from palette colors
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
    QList<QVariant> history;
    if (type == Colors::History) {
        history = Dr::GetPreference(Preferences::Color_Popup_History).toList();
        if (history.count() < 1) return color_block;
    }

    // Loop through array and make a grid of color labels
    QColor color;
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
                            color.setAlphaF(1);
                        }
                        break;
                    case Colors::History:
                        color = QColor::fromRgba( history.at(color_index).toUInt() );
                        ++color_index;
                        break;
                }
            }

            // Skip this color if transparent
            if (color == Qt::transparent) continue;

            // Otherwise create a new color button
            DrQPushButtonColorSelect *button = new DrQPushButtonColorSelect(color_block, this, color, block_width, block_height);
            if (!horizontal) button->setGeometry(i * (block_width + x_spacing) + 2, j * (block_height + y_spacing) + 2, block_width, block_height);
            else             button->setGeometry(j * (block_width + x_spacing) + 2, i * (block_height + y_spacing) + 2, block_width, block_height);

            if (color.rgb() == m_start_color.rgb()) {
                button->move( button->pos() - QPoint(1, 1) );
                button->setFixedSize(block_width + 2, block_height + 2);
            }

            button->setStyleSheet( createButtonStyleSheet(color, border) );

            if (type == Colors::History) {  if (color_index >= history.count()) break;
            } else {                        if (color_index >= Dr::GetPaletteColorCount(m_current_palette)) break;  }
        }
        color_index += mid_step;

        if (type == Colors::History) {      if (color_index >= history.count()) break;
        } else {                            if (color_index >= Dr::GetPaletteColorCount(m_current_palette)) break;  }
    }

    return color_block;
}

// Creates initial style sheet text for little color boxes
QString FormPopupColor::createButtonStyleSheet(QColor color, int border) {
    QString style;
    if (color.rgb() == m_start_color.rgb()) {
        style = "QPushButton {  "
                "   border: " + QString::number(border + 2) + "px solid; border-radius: 2px; "
                "   border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
                "   background-color: " + color.name() + "; }";
    } else {
        style = "QPushButton {  "
                "   border: " + QString::number(border) + "px solid; border-radius: 0px; "
                "   border-color: " + color.darker(175).name() + "; "
                "   background-color: " + color.name() + "; }";
    }
    style +=
        "QPushButton::hover { "
        "   border: " + QString::number(border + 2) + "px solid; border-radius: 3px; "
        "   border-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } ";
    return style;
}


//####################################################################################
//##        Updates the info label color and text
//####################################################################################
void FormPopupColor::setInfoLabelColor(QColor color) {
    bool white = (color.red() < 160 && color.green() < 160 && color.blue() < 160);
    QString style =
        "   background-color: " + color.name() + "; color: ";
    style += white ? "white; " : "black; ";
    style +=
        "   border: 1px solid; border-radius: 4px;"
        "   border-color: " + color.darker(200).name() + "; ";
    m_color_label->setStyleSheet(style);
    QString text = color.name().toUpper();
    m_color_label->setText( text );
}


//####################################################################################
//##        On close, save current tab for later
//####################################################################################
void FormPopupColor::closeEvent(QCloseEvent *event) {
    Dr::SetPreference(Preferences::Color_Popup_Tab, m_palette_block->currentIndex());
    FormPopup::closeEvent(event);
}



//####################################################################################
//##        ColorLabel Class Functions
//####################################################################################
DrQPushButtonPageSelect::DrQPushButtonPageSelect(QWidget *parent, FormPopupColor *popup, QString description) :
    QPushButton(parent), m_popup(popup), m_description(description) {
    setAttribute(Qt::WA_Hover);
}
DrQPushButtonPageSelect::~DrQPushButtonPageSelect() { }

void DrQPushButtonPageSelect::enterEvent(QEvent *) {
    m_text_before = m_popup->getColorLabel()->text();
    m_popup->getColorLabel()->setText( m_description );
}
void DrQPushButtonPageSelect::leaveEvent(QEvent *) {
    m_popup->getColorLabel()->setText( m_text_before );
}


//####################################################################################
//##        ColorLabel Class Functions
//####################################################################################
DrQPushButtonColorSelect::DrQPushButtonColorSelect(QWidget *parent, FormPopupColor *popup, QColor my_color, int width, int height) :
    QPushButton(parent), m_popup(popup), m_color(my_color), m_width(width), m_height(height) {
    setAttribute(Qt::WA_Hover);
    ///setToolTip("R: " + QString::number(m_color.red()) + ", B: " + QString::number(m_color.blue()) + ", G: " + QString::number(m_color.green()));
}
DrQPushButtonColorSelect::~DrQPushButtonColorSelect() { }

void DrQPushButtonColorSelect::enterEvent(QEvent *) {
    if (m_color == m_popup->getStartColor()) {
        this->move( this->pos() - QPoint(1, 1) );
    } else {
        this->move( this->pos() - QPoint(2, 2) );
    }

    this->raise();
    this->setFixedSize(m_width + 4, m_height + 4);

    m_popup->setInfoLabelColor(m_color);
}

void DrQPushButtonColorSelect::leaveEvent(QEvent *) {
    if (m_color == m_popup->getStartColor()) {
        this->setFixedSize(m_width + 2, m_height + 2);
        this->move( this->pos() + QPoint(1, 1) );
    } else {
        this->setFixedSize(m_width, m_height);
        this->move( this->pos() + QPoint(2, 2) );
    }
}

void DrQPushButtonColorSelect::mouseReleaseEvent(QMouseEvent *) {
    emit m_popup->colorGrabbed( m_popup->getReturnWidget(), m_color );
    m_popup->close();
}














