//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_LAYOUT_H
#define WIDGETS_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

// Forward declarations
class DrProject;

//####################################################################################
//##    DrQLayoutFlow
//##        A changing GridLayout that adjusts to the size of the parent widget
//############################
class DrQLayoutFlow : public QLayout
{
private:
    int     doLayout(const QRect &rect, int &row_width, bool testOnly) const;
    int     smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem*>  item_list;                         // Holds a list of items inserted into the layout, so we can delete them in the destuctor
    int                  m_hSpace;
    int                  m_vSpace;
    QSize                m_last_size;

    int                  m_row_width;                       // Holds the largest width (in item count) of the layout

public:
    // Constructors / Destructor
    explicit DrQLayoutFlow(QWidget *parent, int margin_left = -1, int margin_right = -1, int margin_top = -1, int margin_bottom = -1,
                          int hSpacing = -1, int vSpacing = -1);
    explicit DrQLayoutFlow(int margin_left = -1, int margin_right = -1, int margin_top = -1, int margin_bottom = -1,
                          int hSpacing = -1, int vSpacing = -1);
    ~DrQLayoutFlow() override;

    // QLayout Overrides
    void                addItem(QLayoutItem *item) override;
    Qt::Orientations    expandingDirections() const override;
    QLayoutItem*        itemAt(int index) const override;
    void                setGeometry(const QRect &rect) override;
    QLayoutItem*        takeAt(int index) override;

    int     count() const override;
    bool    hasHeightForWidth() const override;
    int     heightForWidth(int) const override;
    QSize   minimumSize() const override;
    QSize   sizeHint() const override;

    // Function Calls
    int     horizontalSpacing() const;
    int     verticalSpacing() const;
    QSize   lastSize() { return m_last_size; }
    int     rowWidth() { return m_row_width; }

};



#endif // WIDGETS_LAYOUT_H








