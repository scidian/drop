//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PROJECT_EDIT_H
#define PROJECT_EDIT_H

#include <QPixmap>
#include <QString>

#include "model/project/project.h"


//####################################################################################
//##    Project handling from Editor, can use Qt
//############################
namespace Dr {


    // ***** Project Building
    DrImage*    AddImage(DrProject *project, QString filename, long key = c_no_key, Asset_Category category = Asset_Category::Image);
    void        AddBuiltInImages(DrProject *project);
    void        InitializeNewProject(DrProject *project, std::string project_name, Orientation orientation, int width, int height, bool test = false);


    // ***** Asset Pixmaps
    QPixmap     GetAssetPixmapDevice(DrDeviceType device_type);
    QPixmap     GetAssetPixmapEffect(DrEffectType effect_type);



    // !!!!! #TEMP: Basic Font Drawing
    QPixmap     CreateText(DrFont *font, std::string text);


    // !!!!! #TEMP: Project Open / Save using QSettings
    bool        OpenProjectFromFile(DrProject *project, std::string open_file);
    void        SaveProjectToFile(DrProject *project);


}   // end namespace Dr

#endif // PROJECT_EDIT_H














