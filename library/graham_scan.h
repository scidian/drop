//
//      Created by Stephens Nunnally on 5/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef GRAHAM_SCAN_H
#define GRAHAM_SCAN_H

#include <QPointF>
#include <QVector>

struct gsPoint {
    double x, y;
};

extern QVector<QPointF> convexHull(QVector<gsPoint> points, int np);


#endif // GRAHAM_SCAN_H
