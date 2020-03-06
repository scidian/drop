//
//      Created by Stephens Nunnally on 10/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QLabel>
#include <QRadioButton>

#include "editor/form_main/form_main.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Builds out FormPop's inner widget to show options for Adding an Entity to Project
//####################################################################################
void FormPopup::buildPopupAddEntity() {
    QFont font = Dr::CustomFont();
    QFontMetrics font_metrics { font };

    // Figure out best size of popup
    QStringList options;
    options << tr("Add New World")
            << tr("Add New Stage")
            << tr("Add Character Asset")
            << tr("Add Object Asset");
    QList<QRect> rects;
    int width = 0;
    for (auto option : options) {
        rects.append( font_metrics.boundingRect(option));
        width = qMax(width, font_metrics.boundingRect(option).width() + 50);
    }
    this->setMinimumWidth(width);
    this->setMinimumHeight( (rects.first().height() * options.count()) + 50);

    // Layout popup
    QVBoxLayout *layout = new QVBoxLayout(m_inner_widget);
    layout->setContentsMargins(8, 0, 5, 10);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(6);

        QRadioButton *buttonWorld =  new QRadioButton(options[0]);      buttonWorld->setObjectName( QStringLiteral("popupRadio"));
        QRadioButton *buttonStage =  new QRadioButton(options[1]);      buttonStage->setObjectName( QStringLiteral("popupRadio"));
        QRadioButton *buttonChar =   new QRadioButton(options[2]);      buttonChar->setObjectName(  QStringLiteral("popupRadio"));
        QRadioButton *buttonObject = new QRadioButton(options[3]);      buttonObject->setObjectName(QStringLiteral("popupRadio"));

        buttonWorld->setFont(font);     buttonWorld->setFixedHeight(    rects[0].height() + 4 );
        buttonStage->setFont(font);     buttonStage->setFixedHeight(    rects[1].height() + 4 );
        buttonChar->setFont(font);      buttonChar->setFixedHeight(     rects[2].height() + 4 );
        buttonObject->setFont(font);    buttonObject->setFixedHeight(   rects[3].height() + 4 );

        buttonWorld->setCheckable(false);
        buttonStage->setCheckable(false);
        buttonChar->setCheckable(false);
        buttonObject->setCheckable(false);

        // Adds World to Project
        connect(buttonWorld, &QRadioButton::released, [this]() {
            DrWorld *world = nullptr;
            IEditorRelay *editor = Dr::GetActiveEditorRelay();
            if (editor) {
                editor->buildInspector( { } );      // Clear inspector to stop Inspector signals
                world = m_project->addWorld();
                editor->buildProjectTree();
                editor->buildInspector( { world->getKey() } );
                editor->updateItemSelection(Editor_Widgets::Stage_View, { world->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
                editor->buildScene( world->getFirstStageKey() );
            }
            this->close();
            if (editor && world != nullptr) {
                editor->updateItemSelection(Editor_Widgets::Stage_View, { world->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
            }
        });

        // Adds Stage to Project
        connect(buttonStage, &QRadioButton::released, [this]() {
            DrStage *new_stage = nullptr;
            IEditorRelay *editor = Dr::GetActiveEditorRelay();
            if (editor) {
                editor->buildInspector( { } );      // Clear inspector to stop Inspector signals
                new_stage = editor->getStageView()->getDrScene()->getCurrentStageShown()->getParentWorld()->addStage();
                editor->buildProjectTree();
                editor->buildInspector( { new_stage->getKey() } );
                editor->updateItemSelection(Editor_Widgets::Stage_View, { new_stage->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
                editor->buildScene(new_stage->getKey());
            }
            this->close();
            if (editor && new_stage != nullptr) {
                editor->updateItemSelection(Editor_Widgets::Stage_View, { new_stage->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
            }
        });

        // Adds Character / Object Asset to Project
        connect(buttonChar,   &QRadioButton::released, [this]() { this->addAssetFromPopup(DrAssetType::Character, c_key_image_character); });
        connect(buttonObject, &QRadioButton::released, [this]() { this->addAssetFromPopup(DrAssetType::Object,    c_key_image_object); });

        layout->addSpacing(4);
        layout->addWidget(buttonWorld);
        layout->addWidget(buttonStage);
        layout->addSpacing(3);

        // Divider line looks nice
        QLabel *label = new QLabel();
        label->setMaximumHeight(1);
        label->setStyleSheet(QString::fromStdString(
                             "color: " +      Dr::GetColor(Window_Colors::Midlight).name() + "; "
                             "background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; "
                             "border: none; margin-left: 5px; margin-right: 5px;"));
        layout->addWidget(label);

        layout->addWidget(buttonChar);
        layout->addWidget(buttonObject);
        layout->addSpacing(4);
}


//####################################################################################
//##    Adds Asset to Project
//####################################################################################
void FormPopup::addAssetFromPopup(DrAssetType asset_type, long source_key) {
    IEditorRelay *editor = Dr::GetActiveEditorRelay();
    if (editor == nullptr) { this->close(); return; }
    editor->buildInspector( { } );                                          // Clear inspector to stop Inspector signals

    DrAsset *asset = Dr::AddAsset(m_project, asset_type, source_key);

    // Update EditorRelay widgets
    editor->buildAssetTree();
    editor->buildInspector( { asset->getKey() } );
    editor->updateItemSelection(Editor_Widgets::Asset_Tree);
    editor->getAssetTree()->setSelectedKey(asset->getKey());
    editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);

    // Close this popup
    this->close();

    // Make sure we leave with Asset Tree highlighted and active
    editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
}














