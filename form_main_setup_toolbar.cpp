//
//      Created by Stephens Nunnally on 3/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDockWidget>
#include <QToolBar>

#include "form_main.h"
#include "library.h"


PushButtonAction::PushButtonAction(const QIcon &icon, const QString &text, QObject *parent)
    : QWidgetAction(parent)
{
    QFont font;
    font.setPointSize(Dr::FontSize());
    setFont(font);

    setIcon(icon);
    setObjectName( QStringLiteral("toolbarButton") );
    setText( text );
}

PushButtonAction::~PushButtonAction() { }


QToolBar* FormMain::buildWindowModeEditStageToolbar()
{
    QFont font, fontLarger;
    font.setPointSize(Dr::FontSize());
    fontLarger.setPointSize(Dr::FontSize() + 2);

    QToolBar *toolbar = new QToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);




    toolbar->insertAction(nullptr, new PushButtonAction(QIcon(":/tree_icons/tree_action.png"), "Refresh"));
    toolbar->addAction(QIcon(":/tree_icons/tree_action.png"), "FirstAction");
    toolbar->addAction(QIcon(":/tree_icons/tree_action.png"), "LastAction");
    PushButtonAction *pb = new PushButtonAction(QIcon(":/tree_icons/tree_action.png"), "Search");
    toolbar->insertAction(nullptr, pb);

    QPushButton *button;
    button = new QPushButton();
    button->setObjectName(QStringLiteral("toolbarButton"));
    button->setFont(font);
    Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
    button->setText( tr("Atlases") );

    toolbar->insertWidget(pb, button);


    /*QDockWidget *toolbar = new QToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setMinimumSize(QSize(449, 50));
    toolbar->setMaximumSize(QSize(524287, 50));
    toolbar->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbar->setAllowedAreas(Qt::TopDockWidgetArea);
    toolbar->setOrientation(Qt::Orientation::Horizontal);
    toolbar->setWindowTitle(QStringLiteral(""));
    toolbar->setContentsMargins(0, 0, 0, 4);
        widgetToolbar = new QWidget();
        widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
        QHBoxLayout *toolbarLayout = new QHBoxLayout(widgetToolbar);
        toolbarLayout->setSpacing(0);
        toolbarLayout->setContentsMargins(0, 0, 0, 0);
            QPushButton *button;
            button = new QPushButton();
            button->setObjectName(QStringLiteral("toolbarButton"));
            button->setFont(font);
            Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
            button->setText( tr("Atlases") );
            toolbarLayout->addWidget(button);
            button = new QPushButton();
            button->setObjectName(QStringLiteral("toolbarButton"));
            button->setFont(font);
            Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
            button->setText( tr("Fonts") );
            toolbarLayout->addWidget(button);
            button = new QPushButton();
            button->setObjectName(QStringLiteral("toolbarButton"));
            button->setFont(font);
            Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
            button->setText( tr("Play") );
            toolbarLayout->addWidget(button);
            button = new QPushButton();
            button->setObjectName(QStringLiteral("toolbarButton"));
            button->setFont(font);
            Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
            button->setText( tr("App Settings") );
            toolbarLayout->addWidget(button);
            button = new QPushButton();
            button->setObjectName(QStringLiteral("toolbarButton"));
            button->setFont(font);
            Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
            button->setText( tr("Worlds / UI") );
            toolbarLayout->addWidget(button);
        toolbar->setWidget(widgetToolbar);
        toolbar->setTitleBarWidget(new QWidget());                               */       // Removes title bar from QDockWidget Toolbar


    return toolbar;
}















