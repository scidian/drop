//
//      Created by Stephens Nunnally on 8/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Object Inspector Image Frame
//
#include <QApplication>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QPainter>

#include "colors/colors.h"
#include "editor/tree_inspector.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "forms/form_popup.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings_component_property.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"


//####################################################################################
//##    Image Frame
//####################################################################################
QFrame* TreeInspector::createImageFrame(DrProperty *property, QFont &font, QSizePolicy size_policy) {

    int frame_height = 100;
    int frame_width =  100;

    // ***** Store current asset key in widget
    QFrame *image_frame = new QFrame();
    image_frame->setAcceptDrops(true);
    image_frame->setSizePolicy(size_policy);
    image_frame->setMinimumHeight(frame_height);
    image_frame->setObjectName("assetFrame");

    long property_key = property->getPropertyKey();
    image_frame->setProperty(User_Property::Mouse_Over, false);                                     // Initialize mouse user data, event filter updates this info
    image_frame->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));                               // Used to track when the mouse
    image_frame->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    m_filter_hover->attachToHoverHandler(image_frame, property);
    addToWidgetList(image_frame);

    // ***** Create the label that will display the image
    QBoxLayout *vertical_split = new QVBoxLayout(image_frame);
    vertical_split->setSpacing(0);
    vertical_split->setMargin(0);
    vertical_split->setContentsMargins(0, 0, 0, 0);
        QLabel *asset_pix = new QLabel(image_frame);
        asset_pix->setObjectName(QStringLiteral("assetPixmap"));
        asset_pix->setFont(font);
        asset_pix->setSizePolicy(size_policy);
        asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
        asset_pix->setPixmap(property->getValue().value<QPixmap>().scaled(QSize(frame_width, frame_height - 15), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        vertical_split->addWidget( asset_pix );

    return image_frame;
}


//####################################################################################
//##
//##    DrFilterInspectorImage Event Filter
//##
//####################################################################################
DrFilterInspectorImage::DrFilterInspectorImage(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay) { }

bool DrFilterInspectorImage::eventFilter(QObject *object, QEvent *event) {

    // ***** Grab properties from Image Frame
    QFrame *frame = dynamic_cast<QFrame*>(object);
    if (frame == nullptr) return QObject::eventFilter(object, event);
    long settings_key = m_editor_relay->getInspector()->getSelectedKey();
    long property_key = frame->property(User_Property::Key).toLongLong();
    if (settings_key <= 0 || property_key <= 0) return QObject::eventFilter(object, event);
    DrProject *project =    m_editor_relay->currentProject();
    DrSettings *settings =  project->findSettingsFromKey(settings_key);
    if (settings == nullptr) return QObject::eventFilter(object, event);

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

        if (drop_event->mimeData()->hasUrls() == false) {
            event->ignore();
            return false;
        }

        // Extract the local paths of the files
        QStringList path_list;
        QList<QUrl> url_list = drop_event->mimeData()->urls();
        for (auto url : url_list)
            path_list.append( url.toLocalFile() );

        // Try to load the first image, if it doesnt load, exit. If it does, make sure it is #AARRGGBB and convert to pixmap
        QString file_path = path_list[0];
        QImage image(file_path);
        if (image.isNull()) {
            event->ignore();
            return QObject::eventFilter(object, event);
        }
        if ( image.format() != QImage::Format::Format_ARGB32 )
            image = image.convertToFormat( QImage::Format_ARGB32 );
        QPixmap pixmap = QPixmap::fromImage( image );

        // ********** Dropped on to Asset Property
        if (settings->getType() == DrType::Asset) {
            DrAsset *asset = dynamic_cast<DrAsset*>(settings);

            // ***** Dropped on to Animation Property
            if (property_key == static_cast<int>(Properties::Asset_Animation_Default)) {
                long old_image_key = asset->getSourceKey();
                bool delete_after =  asset->canDeleteSource();
                long new_image_key = project->addImage(file_path);

                asset->updateAnimationProperty(new_image_key);

                // Update all Things, Thing_Size that use Asset
                for (auto world_pair : project->getWorldMap()) {
                    for (auto stage_pair : world_pair.second->getStageMap()) {
                        for (auto thing_pair : stage_pair.second->getThingMap()) {
                            DrThing *thing = thing_pair.second;
                            if (thing->getAssetKey() == settings_key) {
                                QPointF scale = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
                                QPointF new_size(pixmap.width() * scale.x(), pixmap.height() * scale.y());
                                thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size, new_size);
                            }
                        }
                    }

                }


                m_editor_relay->buildScene( c_same_key );
                m_editor_relay->buildAssetTree();
                m_editor_relay->buildInspector( { settings_key }, true );

                if (delete_after) asset->deleteSource(old_image_key);
                drop_event->acceptProposedAction();
            }
        }


    }

    return QObject::eventFilter(object, event);
}













