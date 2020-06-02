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

#include "project/dr_project.h"

// Forward Declarations
class IProgressBar;


//####################################################################################
//##    Project handling from Editor, can use Qt
//############################
namespace Dr {


    // ***** Project Building
    DrAsset*    AddAsset(DrProject *project, DrAssetType asset_type, long source_key, QString base_name = "");
    void        AddBuiltInImages(DrProject *project);
    void        AddExternalImages(DrProject *project);
    DrImage*    AddImage(DrProject *project, QString filename, long key = c_no_key, bool outline = true, IProgressBar *progress = nullptr);
    DrAsset*    AddPrefab(DrProject *project, DrPrefabType prefab_type);
    QString     FindEmptyAssetName(AssetMap &asset_map, DrAssetType asset_type, QString base_name);
    void        InitializeNewProject(DrProject *project, std::string project_name, Orientation orientation, int width, int height, bool test = false);


    // ***** Asset Pixmaps
    QPixmap     GetAssetPixmapDevice(DrDeviceType device_type);
    QPixmap     GetAssetPixmapEffect(DrEffectType effect_type);
    QPixmap     GetAssetPixmapItem(DrItemType item_type);
    QPixmap     GetAssetPixmapMix(DrMix *mix);
    QPixmap     GetAssetPixmapPrefab(DrPrefabType prefab_type);


    // !!!!! #TEMP: Basic Font Drawing
    QPixmap     CreateText(DrFont *font, std::string text);


    // !!!!! #TEMP: Project Open / Save using QSettings
    bool        OpenProjectFromFile(DrProject *project, std::string open_file);
    void        SaveProjectToFile(DrProject *project);


}   // End namespace Dr

#endif // PROJECT_EDIT_H














