#include "DataExecIOLabel.h"

#include <QPainter>
#include <qDebug>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

DataExecIOLabel::DataExecIOLabel(InputPort *parentInterfaceInput, NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance) : DataExecIOLabel(parentNodeInput, _parentInterfaceInstance)
{
    positionInNode = "input";
    input_parentPort = parentInterfaceInput;
    height = input_parentPort->height/1.2;
}
DataExecIOLabel::DataExecIOLabel(OutputPort *parentInterfaceInput, NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance) : DataExecIOLabel(parentNodeInput, _parentInterfaceInstance)
{
    positionInNode = "output";
    output_parentPort = parentInterfaceInput;
    height = output_parentPort->height/1.2;
}
DataExecIOLabel::DataExecIOLabel(NodeInstance *parentNodeInput, PortInstance *_parentInterfaceInstance){
    parentNode = parentNodeInput;
    width = 0;
    penWidth = 0.6;
    scale = 1.0;

    font = QFont("Dubai");
    font.setBold(false);
    font.setPointSizeF(5);

    A = QPointF(0, 0);
    C = QPointF(0, 0);
    B = QPointF(0, 0);
    computeShapePoints();
}

void DataExecIOLabel::computeShapePoints(){
    double space = 0.0;
    A.setX( - width / 2.0 + space); A.setY( - height / 2.0 + space);
    B.setX( + width / 2.0 - space); B.setY( - height / 2.0 + space);
    C.setX( - width / 2.0 + space); C.setY( + height / 2.0 - space);
    D.setX( + width / 2.0 - space); D.setY( + height / 2.0 - space);
    E.setX( + width / 2.0 - space); E.setY( 0.0 );
}

QRectF DataExecIOLabel::boundingRect() const
{
    auto const penWidth = 1.0 * scale;
    //return QRectF(-width/2 - penWidth/2, -height/2 - penWidth/2, width + penWidth, height + penWidth);
    //return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
    return QRectF(-(width*scale)/2 - penWidth/2, -(height*scale)/2 - penWidth/2, (width*scale) + penWidth, (height*scale) + penWidth);
}

void DataExecIOLabel::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsItem::mousePressEvent(event);
}

void DataExecIOLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    computeShapePoints();

    painter->scale(scale, scale);

    if((positionInNode == "input" && input_parentPort->type == "exec") ||
       (positionInNode == "output" && output_parentPort->type == "exec")){
        color = Qt::white;
        if(positionInNode == "input"){
            input_parentPort->color = color;
        }else if(positionInNode == "output"){
            output_parentPort->color = color;
        }
    }else if((positionInNode == "input" && input_parentPort->type == "data") ||
             (positionInNode == "output" && output_parentPort->type == "data")){
        color = QColor(255,165,0); // orange
        if(positionInNode == "input"){
            input_parentPort->color = color;
        }else if(positionInNode == "output"){
            output_parentPort->color = color;
        }
    }
    QPen pen = QPen(color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setFont(font);
    QFontMetrics fontFM = QFontMetrics(font);

    QRectF textRect = QRectF(A, D);
    if(positionInNode == "input"){
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, input_parentPort->displayName);
    }else if(positionInNode == "output"){
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, output_parentPort->displayName);
    }
}
