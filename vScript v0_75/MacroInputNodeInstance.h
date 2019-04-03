#ifndef MACROINPUTNODEINSTANCE_H
#define MACROINPUTNODEINSTANCE_H

#include "NodeInstance.h"

class MacroNode;
class MainWindow;
class GraphWidget;
class OutputPort;
class MacroInputNode;

class MacroInputNodeInstance : public NodeInstance
{
public:
    MacroInputNodeInstance(MacroInputNode *parentInput, MacroNode *parentMacroNodeInput, MainWindow *mainWindowInput);

    QString getCode(PortInstGate *otherGate);
    QString getFinalOutputInternalName(int index, PortInstGate *otherGate);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    MacroNode *parentMacroNode;
};

#endif // MACROINPUTNODEINSTANCE_H
