#ifndef DATAEXECIOLABEL_H
#define DATAEXECIOLABEL_H

#include "datainterface.h"

#include <QGraphicsItem>
#include <QFont>

class NodeInstance;
class PortInstance;
class InputPort;
class OutputPort;
class DataExecIOLabel : public QGraphicsItem
{
public:
    DataExecIOLabel(InputPort *parentInterfaceInput, NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance);
    DataExecIOLabel(OutputPort *parentInterfaceInput, NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance);
private: DataExecIOLabel(NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance);

public:

    QRectF boundingRect() const override;
    void computeShapePoints();

    InputPort *input_parentPort = nullptr;
    OutputPort *output_parentPort = nullptr;
    PortInstance *parentInterfaceInstance = nullptr;
    QString positionInNode;
    NodeInstance *parentNode;
    int width;
    int height;
    double scale;
    double penWidth;
    QFont font;
    //bool filled;

    QPointF A;  //  A--------B
    QPointF B;  //  |        |
    QPointF C;  //  |        E
    QPointF D;  //  |        |
    QPointF E;  //  C--------D

    QColor color;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // EXECUTIONINPUT_H
