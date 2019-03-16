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


class FlowLayout : public QLayout
{
private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;                          // Holds a list of items inserted into the layout, so we can delete them in the destuctor
    int m_hSpace;
    int m_vSpace;


public:
    explicit FlowLayout(QWidget *parent, int margin_left = -1, int margin_right = -1, int margin_top = -1, int margin_bottom = -1,
                        int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin_left = -1, int margin_right = -1, int margin_top = -1, int margin_bottom = -1,
                        int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout() override;

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
};



#endif // WIDGETS_LAYOUT_H
