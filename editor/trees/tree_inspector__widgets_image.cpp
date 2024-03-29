//
//      Created by Stephens Nunnally on 8/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Object Inspector Image Frame
//
#include <QApplication>
#include <QDebug>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QPainter>
#include <QToolButton>

#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_animation/form_animation.h"
#include "editor/forms/form_popup.h"
#include "editor/forms/form_progress_box.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Image Frame
//####################################################################################
QFrame* TreeInspector::createImageFrame(DrProperty *property, QFont &font, QSizePolicy size_policy, DrImageHolder *use_this_holder) {

    int frame_height = Dr::Scale(100);
    int frame_width =  Dr::Scale(100);
    bool rebuilding =  false;

    // Create new Image Frame
    DrImageHolder *image_frame;
    if (use_this_holder == nullptr) {
        image_frame = new DrImageHolder(m_editor_relay);
        image_frame->setAcceptDrops(true);
        image_frame->setSizePolicy(size_policy);
        image_frame->setMinimumHeight(frame_height);
        image_frame->setObjectName("inspectorImageFrame");
        getHoverHandler()->attachToHoverHandler(image_frame, property);

    // Rebuild existing Image Frame
    } else {
        rebuilding = true;
        image_frame = use_this_holder;
        Dr::ClearWidget(image_frame);
        image_frame->setProperty(User_Property::Header,  QString::fromStdString(property->getDisplayName()));
        image_frame->setProperty(User_Property::Body,    QString::fromStdString(property->getDescription()));
    }

    std::string property_name = property->getPropertyName();
    image_frame->setProperty(User_Property::Mouse_Over, false);                             // Initialize mouse user data, event filter updates this info
    image_frame->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));                       // Used to track when the mouse
    image_frame->setProperty(User_Property::CompKey,    QVariant::fromValue(    property->getParentComponent()->getComponentKey()) );
    image_frame->setProperty(User_Property::CompName,   QString::fromStdString( property->getParentComponent()->getComponentName()) );
    image_frame->setProperty(User_Property::PropKey,    QVariant::fromValue(    property->getPropertyKey()) );
    image_frame->setProperty(User_Property::PropName,   QString::fromStdString( property->getPropertyName()) );
    if (rebuilding == false) addToWidgetList(image_frame);

    // ***** Create the label that will display the image
    QBoxLayout *vertical_split = new QVBoxLayout(image_frame);
    vertical_split->setSpacing(0);
    vertical_split->setContentsMargins(0, 0, 0, 0);
        QLabel *asset_pix = new QLabel(image_frame);
        asset_pix->setObjectName(QStringLiteral("assetPixmap"));
        asset_pix->setFont(font);
        asset_pix->setSizePolicy(size_policy);
        asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);

        QPixmap pixmap;
        DrAnimation *animation = getParentProject()->findAnimationFromKey(property->getValue().toLong());
        if (animation == nullptr) {
            pixmap = QPixmap(":/assets/gui_misc/empty_image.png").scaled(QSize(64, 64), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pixmap = Dr::ApplySinglePixelFilter(Image_Filter_Type::Opacity, pixmap, -100);
        } else {
            pixmap = Dr::ToQPixmap(animation->getFirstFrameImage()->getBitmap());
            pixmap = pixmap.scaled(QSize(frame_width, frame_height - 20), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        asset_pix->setPixmap(pixmap);
        vertical_split->addWidget( asset_pix );

        if (animation != nullptr) {
            // ***** Delete Button
            QToolButton *delete_button = new QToolButton(image_frame);
            delete_button->setObjectName("buttonImageMiniButton");
            Dr::ApplyDropShadowByType(delete_button, Shadow_Types::Button_Shadow_Thin);
                QPixmap delete_icon(":/assets/gui_misc/image_delete.png");
                delete_icon = QPixmap::fromImage( Dr::ColorizeImage(delete_icon.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Text))) );
                delete_button->setIcon( QIcon(delete_icon.scaled(QSize(9, 9), Qt::KeepAspectRatio, Qt::SmoothTransformation)) );
            delete_button->setFixedSize(Dr::Scale(19), Dr::Scale(17));
            delete_button->setVisible(false);
            image_frame->setDeleteButton(delete_button);

            connect(delete_button, &QPushButton::clicked, [this, property, animation] () {
                property->setValue(c_no_key);
                QList<long> current_selected = getEditorRelay()->getInspector()->getSelectedKeys();
                this->getParentProject()->deleteAnimation(animation->getKey(), property->getParentSettings()->getKey());
                getEditorRelay()->buildScene( c_same_key );
                getEditorRelay()->buildAssetTree();
                getEditorRelay()->buildInspector( current_selected, true );
            });

            // ***** Edit Button
            QToolButton *edit_button = new QToolButton(image_frame);
            edit_button->setObjectName("buttonImageMiniButton");
            Dr::ApplyDropShadowByType(edit_button, Shadow_Types::Button_Shadow_Thin);
                QPixmap edit_icon(":/assets/gui_misc/image_edit.png");
                edit_icon = QPixmap::fromImage( Dr::ColorizeImage(edit_icon.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Text))) );
                edit_button->setIcon( QIcon(edit_icon.scaled(QSize(9, 9), Qt::KeepAspectRatio, Qt::SmoothTransformation)) );
            edit_button->setFixedSize(Dr::Scale(19), Dr::Scale(17));
            edit_button->setVisible(false);
            image_frame->setEditButton(edit_button);

            connect(edit_button, &QPushButton::clicked, [this, property, animation] () {

                FormAnimation *animation_editor = new FormAnimation(this->getParentProject(), animation->getKey(), nullptr );
                animation_editor->show();

            });
        }

    // Return DrImageHolder
    return image_frame;
}


//####################################################################################
//##
//##    DrImageHolder
//##        Handles Image Holder
//##
//####################################################################################
DrImageHolder::DrImageHolder(IEditorRelay *editor_relay, QWidget *parent) : QFrame(parent) {
    m_editor_relay = editor_relay;
};

void DrImageHolder::setChildrenGeometry() {
    if (this->getDeleteButton() != nullptr) this->getDeleteButton()->setGeometry( this->width() - Dr::Scale(26), 10, Dr::Scale(19), Dr::Scale(17));
    if (this->getEditButton()   != nullptr) this->getEditButton()->setGeometry(   this->width() - Dr::Scale(26), 10 + Dr::Scale(21), Dr::Scale(19), Dr::Scale(17));
}


//####################################################################################
//##
//##    DrFilterInspectorImage Event Filter
//##
//####################################################################################
DrFilterInspectorImage::DrFilterInspectorImage(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay) { }

bool DrFilterInspectorImage::eventFilter(QObject *object, QEvent *event) {

    // ***** Grab properties from Image Frame
    DrImageHolder *frame = dynamic_cast<DrImageHolder*>(object);
    if (frame == nullptr) return QObject::eventFilter(object, event);
    long settings_key = getEditorRelay()->getInspector()->getShownKey();
    std::string component_name = frame->property(User_Property::CompName).toString().toStdString();
    std::string property_name =  frame->property(User_Property::PropName).toString().toStdString();

    if (settings_key <= 0 || property_name == "") return QObject::eventFilter(object, event);
    DrProject  *project =   m_editor_relay->currentProject();               if (project == nullptr)  return QObject::eventFilter(object, event);
    DrSettings *settings =  project->findSettingsFromKey(settings_key);     if (settings == nullptr) return QObject::eventFilter(object, event);

    if (event->type() == QEvent::Resize) {
        frame->setChildrenGeometry();

    } else if (event->type() == QEvent::HoverMove || event->type() == QEvent::HoverEnter) {
        if (frame->getDeleteButton() != nullptr) frame->getDeleteButton()->setVisible(true);
        if (frame->getEditButton() !=   nullptr) frame->getEditButton()->setVisible(true);

    } else if (event->type() == QEvent::HoverLeave) {
        if (frame->getDeleteButton() != nullptr) frame->getDeleteButton()->setVisible(false);
        if (frame->getEditButton() !=   nullptr) frame->getEditButton()->setVisible(false);
    }

    // ***** Event Debugging
    /**
    if (event->type() != QEvent::MouseButtonPress &&        //   2
        event->type() != QEvent::MouseButtonRelease &&      //   3
        event->type() != QEvent::MouseMove &&               //   5
        event->type() != QEvent::Enter &&                   //  10
        event->type() != QEvent::Leave &&                   //  11
        event->type() != QEvent::Paint &&                   //  12
        event->type() != QEvent::Move &&                    //  13
        event->type() != QEvent::Resize &&                  //  14
        event->type() != QEvent::Show &&                    //  17
        event->type() != QEvent::Hide &&                    //  18
        event->type() != QEvent::Wheel &&                   //  31
        event->type() != QEvent::HoverEnter &&              // 127
        event->type() != QEvent::HoverLeave &&              // 128
        event->type() != QEvent::HoverMove &&               // 129
        event->type() != QEvent::DynamicPropertyChange      // 170
        ) {
        Dr::SetLabelText(Label_Names::Label_1, "Event: " + QString::number(event->type()) + ", " + Dr::CurrentTimeAsString());
    } */
    // QEvent::DragEnter =  60,                             // drag moves into widget
    // QEvent::DragMove =   61,                             // drag moves in widget
    // QEvent::DragLeave =  62,                             // drag leaves or is cancelled
    // QEvent::Drop =       63,                             // actual drop

    if (event->type() == QEvent::DragEnter) {
        event->accept();
    }

    // ***** Handle Drop Event
    if (event->type() == QEvent::Drop) {
        QDropEvent *drop_event = dynamic_cast<QDropEvent*>(event);

        // ********** Check if Image Format
        if (drop_event->mimeData()->hasUrls() == false) {
            event->ignore();
            return false;
        }

        // ***** Extract the local paths of the files
        QStringList path_list;
        QList<QUrl> url_list = drop_event->mimeData()->urls();
        for (auto url : url_list) {
            path_list.append( url.toLocalFile() );
        }

        // ***** Try to load the images, if they dont load, exit. If they do, make sure they are #AARRGGBB and convert to pixmap
        QList<QString> file_paths;
        double width = 0, height = 0;
        int count = 0;
        for (auto path : path_list) {
            QImage image(path);

            if (image.isNull() == false) {
                if (count == 0) {
                    QPixmap pixmap = QPixmap::fromImage( image.convertToFormat( QImage::Format_ARGB32 ));
                    width =  pixmap.width();
                    height = pixmap.height();
                }
                file_paths.append(path);
                count++;
            }
        }
        DrProperty *property = settings->findPropertyFromPropertyName(property_name);
        if (file_paths.isEmpty() || property == nullptr) {
            event->ignore();
            return QObject::eventFilter(object, event);
        }

        // ***** Add Images, Update Animation
        FormProgressBox *progress = new FormProgressBox("...", "Cancel", file_paths.size() * 2, frame->window());
        progress->setPrefix(" Importing Images: \n");
        std::list<long> image_keys;
        for (auto file_path : file_paths) {
            DrImage *image = Dr::AddImage(project, file_path, c_no_key, true, progress);
            image_keys.push_back(image->getKey());
        }
        progress->stopProgress();
        settings->updateAnimationProperty( image_keys, property->getCompPropPair() );

        // ***** Update all Things, Thing_Size that use Asset
        for (auto &world_pair : project->getWorldMap()) {
            for (auto &stage_pair : world_pair.second->getStageMap()) {
                for (auto &thing_pair : stage_pair.second->getThingMap()) {
                    DrThing *thing = thing_pair.second;
                    if (thing->getAssetKey() == settings_key) {
                        DrPointF scale = thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale).toPointF();
                        DrPointF new_size(width * scale.x, height * scale.y);
                        thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size, new_size);
                    }
                }
            }
        }

        // ***** Update UI
        getEditorRelay()->buildScene( c_same_key );
        getEditorRelay()->buildAssetTree();
        getEditorRelay()->buildInspector( { settings_key }, true );
        drop_event->acceptProposedAction();
    }

    return QObject::eventFilter(object, event);
}














