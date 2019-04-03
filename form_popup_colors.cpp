//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Subclassed FormPopup used to allow color picking with sweet options
//
//
#include "editor_tree_inspector.h"
#include "form_main.h"
#include "form_popup.h"
#include "globals.h"


//####################################################################################
//##        Constructor
//####################################################################################
ColorPopup::ColorPopup(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset, int y_offset)
    : FormPopup(project, widget_to_use_for_mapToGlobal, parent, x_offset, y_offset) { }


//####################################################################################
//##        Builds a popup with a lot of colors to choose from
//####################################################################################
void ColorPopup::buildPopupColors(QWidget *wants_color, QColor start_color)
{
    QFont font;  font.setPointSize(Dr::FontSize());
    m_wants_return_variable = wants_color;

    this->setFixedSize(210, 310);

    // ********** Widget for the first page, Material Palette
    QWidget *first_page_widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(first_page_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(4);

        QLabel *info_label = new QLabel();
        info_label->setAlignment(Qt::AlignCenter);
        info_label->setFixedSize(192, 20);
        info_label->setFont( font );
        setInfoLabelColor(info_label, start_color);

        // ***** Basic
        QWidget *basic =       createColorBlock(info_label,  0, 12,  1, 0, 11, 11, 0, 1, 1, Colors::Basic);
        layout->addWidget(basic);

        // ***** Grays
        QWidget *grays =       createColorBlock(info_label,  2, 12,  3, 0, 11, 11, 0, 1, 1, Colors::Grays);
        layout->addWidget(grays);

        // ***** Main Block of Colors
        QWidget *main_colors = createColorBlock(info_label, 32, 16, 16, 4, 11, 11, 0, 1, 1, Colors::Main);
        layout->addWidget(main_colors);

        // ********** Accent Colors
        ///QWidget *accent_colors = CreateColorBlock(popup, wants_color, info_label, 42, 16, 4, 10, 11, 11, 0, 1, Colors::Accent);
        ///layout->addWidget(accent_colors);

        layout->addWidget(info_label);


    // ********** Widget for the second page, Material Palette
    QWidget *second_page_widget = new QWidget();
    layout = new QVBoxLayout(second_page_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(4);

        QLabel *test = new QLabel("test");
        layout->addWidget(test);


    getInnerStackedWidget()->addWidget(first_page_widget);
    getInnerStackedWidget()->addWidget(second_page_widget);



    // ********* Buttons for changing pages in the QStackWidget
    QPushButton *change0 =  new QPushButton(getInnerStackedWidget());
    QPushButton *change1 = new QPushButton(getInnerStackedWidget());

    change0->setGeometry( 159, 7, 41, 25);
    change0->setObjectName("buttonColorMaterial");
    connect(change0, &QPushButton::clicked,  this, [this, change0, change1]() {
        this->getInnerStackedWidget()->setCurrentIndex(0);
        change0->raise();
        change1->raise();
    });

    change1->setGeometry(159, 38, 41, 25);
    change1->setObjectName("buttonColorMaterial");
    connect(change1, &QPushButton::clicked, this, [this, change0, change1]() {
        this->getInnerStackedWidget()->setCurrentIndex(1);
        change0->raise();
        change1->raise();
    });

}


//####################################################################################
//##        Creates a block group of color buttons from palette colors
//####################################################################################
QWidget* ColorPopup::createColorBlock(QLabel *info_label, int start_index, int columns, int rows, int mid_step,
                                     int block_width, int block_height, int border, int x_spacing, int y_spacing, Colors type)
{
    QWidget *color_block = new QWidget();

    int width =  (columns * block_width)  + (x_spacing * (columns - 1)) + 4;
    int height = (rows    * block_height) + (y_spacing * (rows    - 1)) + 4;
    color_block->setFixedSize(width, height);
    color_block->setContentsMargins(0, 0, 0, 0);

    // Figure out if we're drawing these top to bottom, or left to right
    int color_index = start_index;
    bool horizontal = (type == Colors::Basic || type == Colors::Grays);
    int x = (horizontal) ? rows : columns;
    int y = (horizontal) ? columns : rows;

    // Loop through array and make a grid of color labels
    QColor color;
    for (int i = 0; i < x; i++)  {
        for (int j = 0; j < y; j++) {

            if (type == Colors::Basic) {
                color = Dr::GetColorBasic(color_index);
            } else {
                if (j < 10) color = Dr::GetColorMaterial(color_index);
                else        color = color.darker( 150 );
            }

            ColorSelecterButton *button = new ColorSelecterButton(color_block, this, m_wants_return_variable, info_label, color);
            button->setStyleSheet( createButtonStyleSheet(color, border) );

            if (!horizontal) button->setGeometry(i * (block_width + x_spacing) + 2, j * (block_height + y_spacing) + 2, block_width, block_height);
            else             button->setGeometry(j * (block_width + x_spacing) + 2, i * (block_height + y_spacing) + 2, block_width, block_height);

            if (j < 10 || type == Colors::Basic) ++color_index;
        }
        color_index += mid_step;
    }

    return color_block;
}

// Creates initial style sheet text for little color boxes
QString ColorPopup::createButtonStyleSheet(QColor color, int border)
{
    return
    "QPushButton {  "
    "   border: " + QString::number(border) + "px solid; border-radius: 0px; "
    "   border-color: " + color.darker(150).name() + "; "
    "   background-color: " + color.name() + "; }"
    "QPushButton::hover { "
    "   border: " + QString::number(border + 2) + "px solid; border-radius: 3px; "
    "   border-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } ";
}


//####################################################################################
//##        Updates the info label color and text
//####################################################################################
void ColorPopup::setInfoLabelColor(QLabel *label, QColor color)
{
    bool white = (color.red() < 160 && color.green() < 160 && color.blue() < 160);
    QString style =
        "   background-color: " + color.name() + "; color: ";
    style += white ? "white; " : "black; ";
    style +=
        "   border: 1px solid; border-radius: 4px;"
        "   border-color: " + color.darker(200).name() + "; ";
    label->setStyleSheet(style);
    label->setText( color.name().toUpper() );
    ///label->setText( "Red: " + QString::number(color.red()) +
    ///             ", Blue: " + QString::number(color.blue()) +
    ///            ", Green: " + QString::number(color.green()) );
}



//####################################################################################
//##        ColorLabel Class Functions
//####################################################################################
ColorSelecterButton::ColorSelecterButton(QWidget *parent, ColorPopup *popup, QWidget *wants_color, QLabel *info_label, QColor my_color) :
    QPushButton(parent), m_popup(popup), m_widget(wants_color), m_info_label(info_label), m_color(my_color)
{
    setAttribute(Qt::WA_Hover);
}
ColorSelecterButton::~ColorSelecterButton() { }

void ColorSelecterButton::enterEvent(QEvent *)
{
    this->raise();
    this->move( this->pos() - QPoint(2, 2) );
    this->setFixedSize(15, 15);

    m_popup->setInfoLabelColor(m_info_label, m_color);
}

void ColorSelecterButton::leaveEvent(QEvent *)
{
    this->setFixedSize(11, 11);
    this->move( this->pos() + QPoint(2, 2) );
}

void ColorSelecterButton::mouseReleaseEvent(QMouseEvent *)
{
    emit m_popup->colorGrabbed(m_widget, m_color);
    m_popup->close();
}














