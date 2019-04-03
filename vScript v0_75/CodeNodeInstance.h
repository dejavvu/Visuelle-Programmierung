#ifndef CODENODEINSTANCE_H
#define CODENODEINSTANCE_H

#include "nodeinstance.h"

class FunctionNode;
class GraphWidget;
class MainWindow;
class CodeNode;
class PortInstGate;

class CodeNodeInstance : public NodeInstance
{
public:
    CodeNodeInstance(CodeNode *parentInput, int instanceNumberInput, MainWindow *mainWindowInput, GraphWidget *graphInput);


    void computeShapePoints();
    void updateFontSize();
    //void setInpOutPositions();
    //void changeSize(QString side, double mult);
    QPainterPath getShapePath(QString pathType);
    QString getCode(PortInstGate *otherGate);
    QString getFinalOutputInternalName(int index, PortInstGate *otherGate);
    void replaceInputsFromOutputNames();
    QString replaceInputsFromCode(QString code);
    QString replaceOutputsFromCode(QString code);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    CodeNode *parentNode;

    bool leftMousePressed;
    QPointF mousePressPoint;

};

#endif // CODENODEINSTANCE_H
