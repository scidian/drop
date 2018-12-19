//
//
//
//
//

#ifndef FORM_STYLING_H
#define FORM_STYLING_H

#include "form_main.h"

//#######################################################################
//##
//##    SceneTreeHighlightStyle - A sub classed QProxyStyle so we can overwrite
//##                              drawing of TreeWidget list divider
//##
//#######################################################################
class SceneTreeHighlightProxy : public QProxyStyle
{
private:
    TreeSceneView  *m_parent_tree;

public:
    explicit SceneTreeHighlightProxy(QStyle *baseStyle, TreeSceneView *parent_tree) : QProxyStyle(baseStyle), m_parent_tree(parent_tree) { }
    virtual ~SceneTreeHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};





#endif // FORM_STYLING_H
