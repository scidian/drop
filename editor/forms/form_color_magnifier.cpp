//
//      Created by Stephens Nunnally on 3/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QGridLayout>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

#include "editor/form_main/form_main.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"


constexpr int c_text_box_height = 35;

constexpr int c_form_x_offset =    2;
constexpr int c_form_y_offset =   12;
constexpr int c_mouse_x_offset =   1;
constexpr int c_mouse_y_offset =   3;

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
FormColorMagnifier::FormColorMagnifier(QWidget *parent, QPoint mouse_pos, int width, int height, double zoom)
    : QWidget (parent), m_parent(parent), m_width(width), m_height(height), m_zoom(zoom) {

    this->setObjectName(QStringLiteral("colorMagForm"));

    // Make sure this form is deleted when it closes
    this->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);

    // Set up transparent background
    this->setStyleSheet("QWidget#colorMagForm { color: rgba(0, 0, 0, 0); background-color: rgba(0, 0, 0, 0); border: none; }");
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    this->setAttribute(Qt::WidgetAttribute::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, false);

    // Window properties
    this->setWindowFlag(Qt::WindowType::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowType::NoDropShadowWindowHint);
    this->setWindowFlag(Qt::WindowType::Popup);

    this->setFixedSize(width, height + c_text_box_height);
    this->move(mouse_pos.x() - width / 2 - c_form_x_offset, mouse_pos.y() - height / 2 - c_form_y_offset);

    // Take initial screenshot of desktop
    m_image = QImage(1024, 1024, QImage::Format::Format_ARGB32);
    QScreen *screen = QGuiApplication::screenAt(mouse_pos);
    if (screen != nullptr) grabScreen(screen);

    // Create a label to show the zoomies
    QGridLayout *layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
        m_label = new QLabel();
        m_label->setFixedSize(width, height + c_text_box_height);
        layout->addWidget(m_label);
        updateColor(mouse_pos);
}

FormColorMagnifier::~FormColorMagnifier() { }


//####################################################################################
//##    SLOT: Take a screenshot of the current screen
//####################################################################################
void FormColorMagnifier::grabScreen(QScreen *screen) {
    m_screen = screen;
    QRect sg  = m_screen->geometry();

    //QDesktopWidget *root_window = QApplication::desktop();
    //if (root_window != nullptr) {
        //m_capture = m_screen->grabWindow( root_window->winId(), sg.x(), sg.y(), sg.width(), sg.height());
        m_capture = m_screen->grabWindow( 0, sg.x(), sg.y(), sg.width(), sg.height());
        m_image   = m_capture.toImage();
    //}
}


//####################################################################################
//##    Mouse Events
//####################################################################################
void FormColorMagnifier::mousePressEvent(QMouseEvent *event) { QWidget::mousePressEvent(event); }
void FormColorMagnifier::mouseReleaseEvent(QMouseEvent *) {
    Dr::ClearCursor();
    emit colorGrabbed(m_parent, Dr::FromQColor(m_color));
    this->close();
}

void FormColorMagnifier::mouseMoveEvent(QMouseEvent *event) {
    // Get global mouse position, have form follow us, figure out screen position
    QPoint mouse_pos = event->globalPos();
    this->move(mouse_pos.x() - m_width / 2 - c_form_x_offset, mouse_pos.y() - m_height / 2 - c_form_y_offset);

    // Check if we are on a new screen
    QScreen *check_screen = QGuiApplication::screenAt(mouse_pos);
    if (m_screen != check_screen && check_screen != nullptr) grabScreen(check_screen);

    updateColor(mouse_pos);
}


void FormColorMagnifier::updateColor(QPoint mouse_pos) {
    // If position out of bounds, exit out
    QPoint screen_pos;
    QRect  sg =  m_screen->geometry();
    screen_pos.setX( static_cast<int>( (mouse_pos.x() - sg.x() - c_mouse_x_offset) * m_screen->devicePixelRatio()) );
    screen_pos.setY( static_cast<int>( (mouse_pos.y() - sg.y() - c_mouse_y_offset) * m_screen->devicePixelRatio()) );
    if (screen_pos.x() < 0 || screen_pos.x() > (m_image.width() - 1) || screen_pos.y() < 0 || screen_pos.y() > (m_image.height() - 1)) return;

    // Get pixel color
    m_color = m_image.pixelColor(screen_pos.x(), screen_pos.y());

    // Apply new pixmap to label
    QPixmap small = drawCursor(screen_pos, m_width, m_height, m_zoom);
    m_label->setPixmap( small );

    ///// Originally tried to change the cursor, couldnt get the cursor to stay application / desktop wide
    ///QCursor cursor( small, small.width() / 2, small.height() / 2);
    ///Dr::ClearCursor();
    ///Dr::SetCursor( cursor );

    ///// Also tried to hide cursor, but only seems to hide over active widget
    Dr::ClearCursor();
    Dr::HideCursor();
}



//####################################################################################
//##    Creates the Color Magnifier pixmap
//####################################################################################
QPixmap FormColorMagnifier::drawCursor(QPoint screen_pos, int width, int height, double zoom) {
    // Set round clipping path for pixmap
    QPixmap small(width, height + c_text_box_height);
    small.fill( Qt::transparent );
    QPainter paint(&small);
    QPainterPath circle_path, square_path;
    square_path.addRect(   0, 0, small.width(), small.height());
    circle_path.addEllipse(2, 2, width - 4, height - 4);
    paint.setClipPath(circle_path);

    // Draw zoomed in portion and square around selected pixel
    paint.drawPixmap(QRectF(-zoom / 2, -zoom / 2, width, height),
                     m_capture,
                     QRectF(screen_pos.x() - (width  / 2 / zoom), screen_pos.y() - (height / 2 / zoom), width / zoom, height / zoom) );
    paint.setClipPath(square_path);

    paint.setBrush( Qt::NoBrush );
    paint.setPen( Qt::white );
    paint.drawRect( static_cast<int>(width / 2 -zoom / 2),
                    static_cast<int>(height / 2 - zoom / 2),
                    static_cast<int>(zoom), static_cast<int>(zoom) );
    paint.setPen( Qt::black );
    paint.drawRect( static_cast<int>(width / 2 -zoom / 2) - 1,
                    static_cast<int>(height / 2 - zoom / 2) - 1,
                    static_cast<int>(zoom) + 2, static_cast<int>(zoom) + 2);

    // Draw rings around zoom
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen( QPen( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)), 4 ) );
    paint.drawEllipse(2, 2, width- 4, height - 4);
    paint.setPen( QPen( Dr::ToQColor(Dr::GetColor(Window_Colors::Midlight)), 2 ) );
    paint.drawEllipse(2, 2, width - 4, height - 4);
    paint.setPen( QPen( m_color, 4 ) );
    paint.drawEllipse(5, 5, width - 10, height - 10);

    // Draw bottom text box area
    paint.setPen( QPen( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)), 4 ) );
    QColor back_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
           back_color.setAlpha(128);
    paint.setBrush( back_color );
    QRectF text_rect = QRectF(5, height + 5, width - 10, c_text_box_height - 10);
    paint.drawRoundedRect( text_rect, 10, 10);
    paint.setPen( QPen( Dr::ToQColor(Dr::GetColor(Window_Colors::Midlight)), 2 ) );
    paint.drawRoundedRect( text_rect.adjusted(0, 0, 0, 0), 10, 10);
    paint.setPen( QPen( m_color, 3 ) );
    paint.drawRoundedRect( text_rect.adjusted(2, 2, -2, -2), 8, 8);

    // Draw color name text
    paint.setPen( Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light)) );
    ///QString color_string = "R: " + QString::number(m_color.red()) +
    ///                      " G: " + QString::number(m_color.green()) +
    ///                      " B: " + QString::number(m_color.blue());
    QFont font = Dr::CustomFont();
    paint.setFont( font );
    paint.drawText( text_rect, Qt::AlignCenter, m_color.name().toUpper() );
    paint.end();

    return small;
}





















