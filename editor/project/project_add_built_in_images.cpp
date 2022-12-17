//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>

#include "core/dr_debug.h"
#include "core/interface/dr_progress.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "project/dr_project.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/properties/property_collision.h"

namespace Dr {


//####################################################################################
//##    Adds Singular Image to Project
//##        During project->addImage, if exact image is already found within
//##        project, that existing image is returned
//####################################################################################
DrImage* AddImage(DrProject *project, QString filename, long key, bool outline, IProgressBar *progress) {

    QFileInfo file_info(filename);
        QString full_path =     file_info.path();
        QString file_name =     file_info.fileName();
        QString simple_name =   file_info.baseName();
                simple_name =   simple_name.replace("_", " ");      // Replace underscores with spaces
                simple_name =   simple_name.replace("-", " ");      // Replace hyphens with spaces

    QImage image = QImage(filename).convertToFormat(QImage::Format::Format_ARGB32);
    if (image.isNull()) {
        ///image = QImage(1, 1, QImage::Format::Format_ARGB32);
        return nullptr;
    }
    DrBitmap bitmap = DrBitmap(image.constBits(), static_cast<int>(image.sizeInBytes()), false, image.width(), image.height());
    ///qDebug() << "Bitmap - from size: " << image.sizeInBytes() << ", Width: " << bitmap.width << ", Height: " << bitmap.height;

    DrImage *dr_image = project->addImage(simple_name.toStdString(), bitmap, key, outline, progress);
    return dr_image;
}


//####################################################################################
//##    Adds Built In Images to Project
//####################################################################################
void AddBuiltInImages(DrProject *project) {

    AddImage(project, ":/assets/dr_images/empty.png",           c_key_image_empty);

    AddImage(project, ":/assets/dr_images/blob.png",            c_key_image_blob);
    AddImage(project, ":/assets/dr_images/character.png",       c_key_image_character);
    AddImage(project, ":/assets/dr_images/foliage.png",         c_key_image_foliage);
    AddImage(project, ":/assets/dr_images/ground.png",          c_key_image_ground);
    AddImage(project, ":/assets/dr_images/ladder.png",          c_key_image_ladder);
    AddImage(project, ":/assets/dr_images/object.png",          c_key_image_object);
    AddImage(project, ":/assets/dr_images/spike.png",           c_key_image_spike);
    AddImage(project, ":/assets/dr_images/spring.png",          c_key_image_spring);

}   // End AddBuiltInImages()


//####################################################################################
//##    Adds Built In Images to Project
//####################################################################################
void AddExternalImages(DrProject *project) {

    // Create directory object at current directory, filter for folders
    QDir dir("", "*", QDir::SortFlag::Name, QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    ///qDebug() << "Directory: " << dir.path();
    ///qDebug() << "Image folder exists? " << dir.exists("images");

    // If "images" folder is with executable (windows) or in Drop.app/Contents/macOS/images (mac)
    if (dir.exists("images")) {
        dir.cd("images");

        // Go through each folder and add Images
        for (auto sub_directory : dir.entryList()) {
            QString folder_name = sub_directory;
            QDir folder = dir;
                 folder.cd(folder_name);
                 folder.setFilter(QDir::Filter::Files);
            ///qDebug() << "Directory: " << folder.absolutePath();

            // Create category name from folder name
            if (folder_name.length() < 1) continue;
            QString category = folder_name;
                    category.replace("_", " ");
                    category.replace(0, 1, category.at(0).toUpper());

            // Go through each file and load image
            for (auto image_file : folder.entryInfoList()) {
                ///qDebug() << "Image: " << image_file.absoluteFilePath();
                if (image_file.baseName() == "_icon") {
                    QPixmap pix(image_file.absoluteFilePath());
                    Dr::SetAssetCategoryIcon(folder_name.toStdString(), pix);
                } else {
                    bool trace_outline = false;
                    DrImage *image = AddImage(project, image_file.absoluteFilePath(), c_no_key, trace_outline);
                    if (image != nullptr) image->setFolderName(folder_name.toStdString());
                }
            }

        }

    }
}   // End AddExternalImages()



}   // End namespace Dr















