#include "MacroNodeInstance.h"

#include "MacroNode.h"
#include "MacroInputNodeInstance.h"
#include "MacroOutputNodeInstance.h"
#include "QGraphicsSceneMouseEvent"
#include "mainwindow.h"
#include "graphwidget.h"
#include "PortInstGate.h"
#include "DataExecIOLabel.h"

#include <QDebug>

MacroNodeInstance::MacroNodeInstance(MacroNode *parentInput, int instanceNumberInput, MainWindow *mainWindowInput, GraphWidget *graphInput)
    : NodeInstance(parentInput, instanceNumberInput, mainWindowInput, graphInput)
{
    parentNode = parentInput;

    //cornerRadius = 15;

    displayNameFont = QFont("Corbel");
    displayNameFont.setBold(true);
    displayNameFont.setItalic(true);
    displayNameFont.setPointSize(15);
}

QString MacroNodeInstance::getCode(PortInstGate *otherGate){
    QString nodeInstParentCode = NodeInstance::getCode(otherGate);
    if(!nodeInstParentCode.isEmpty()){
        return nodeInstParentCode;
    }

    parentNode->currentInstanceForGettingCode = this;

    // if getCode gets called here, we already know, that this is an active node
    // because this gets only called on nodes with execution inputs
    QString code = parentNode->inputNodeInstance->getCode(otherGate);
    for(int i=0; i<outputPortInstances.length(); i++){
        PortInstance *o = outputPortInstances.at(i);
        if(o->deio->connectionsTo.length() > 0){
            code += o->deio->connectionsTo.first()->parentNode->getCode(o->deio);
        }
    }

    return code;
}

QString MacroNodeInstance::getFinalOutputInternalName(int index, PortInstGate *otherGate){
    parentNode->currentInstanceForGettingCode = this;
    qDebug() << "currentInstanceForGettingCode is now me " << this;

    if(parentNode->outputNodeInstance->inputPortInstances.at(index)->deio->connectionsTo.length() > 0){
        // here, I go to the OutputNodeInstance of my parentMacroNode, look what nodeinstance and which
        // output is connected to the corresponding input of the OutputNodeInstance, to and get it's
        // final internal output name
        PortInstance *output_ipi = parentNode->outputNodeInstance->inputPortInstances.at(index);
        PortInstGate *connectedPIGate = output_ipi->deio->connectionsTo.first();
        PortInstance *connectedPI = connectedPIGate->parentInterfaceInstance;
        NodeInstance *connectedNodeInstance = connectedPIGate->parentNode;
        int connectedPI_outputIndex = connectedNodeInstance->outputPortInstances.indexOf(connectedPI);
        return connectedNodeInstance->getFinalOutputInternalName(connectedPI_outputIndex, otherGate);
    }
    return "(ALERT outputnode is not connected)";
}

// DOES IT HAS TO BE HERE???
void MacroNodeInstance::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = true;
        mousePressPoint = event->pos();
    }
    QGraphicsItem::mousePressEvent(event);
}
void MacroNodeInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}
void MacroNodeInstance::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(leftMousePressed){
        QPointF newPos(mapToScene(event->pos() - mousePressPoint));
        this->setPos(newPos);
        setInpOutPositions();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

//void MacroInputNodeInstance::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
//    mainWindow->addGraphTab(this);
//}


//void MacroNodeInstance::computeShapePoints(){
//    //... TODO?
//    int r = cornerRadius;
//    A1.setX(-width/2); A1.setY(-height/2);
//    A2.setX(width/2); A2.setY(-height/2);
//    B1.setX(-width/2); B1.setY(-height/2 + r);
//    B2.setX(width/2); B2.setY(-height/2 + r);
//    C1.setX(-width/2); C1.setY(height/2 - r);
//    C2.setX(width/2); C2.setY(height/2 - r);
//    D1.setX(-width/2); D1.setY(height/2);
//    D2.setX(width/2); D2.setY(height/2);
//    E1.setX(0); E1.setY(-height/2 + r);
//    E2.setX(0); E2.setY(height/2 - r);
//}

//void MacroNodeInstance::computeWidthAndHeight(){

//    QFontMetricsF *displayNameFM = new QFontMetricsF(displayNameFont);
//    qreal displayNameFontHeight = displayNameFM->height();
//    qreal displayNameFontWidth = displayNameFM->width(parentNode->title);

//    int edgeHeight = 0;
//    int leftEdgeHeight = 0;
//    int rightEdgeHeight = 0;
//    int leftLargestWidth = 0;
//    int rightLargestWidth = 0;
//    for(InputPort *di : parentNode->inputs){
//        if(di->width > leftLargestWidth)
//            leftLargestWidth = di->width;
//        leftEdgeHeight += di->height;
//    }
//    for(OutputPort *di : parentNode->outputs){
//        if(di->width > rightLargestWidth)
//            rightLargestWidth = di->width;
//        rightEdgeHeight += di->height;
//    }
//    edgeHeight = leftEdgeHeight > rightEdgeHeight ? leftEdgeHeight : rightEdgeHeight;

//    edgeHeight += cornerRadius;

//    int minPossibleHeight = displayNameFontHeight > edgeHeight ? displayNameFontHeight : edgeHeight; // + 2*cornerRadius;

//    int minPossibleWidth = displayNameFontWidth > (leftLargestWidth + spaceBetweenInputAndOutput + rightLargestWidth) ? displayNameFontWidth : (leftLargestWidth + spaceBetweenInputAndOutput + rightLargestWidth);

//    width = minPossibleWidth;
//    height = minPossibleHeight;

//    if(width < minWidth){
//        width = minWidth;
//    }
//    if(height < minHeight){
//        height = minHeight;
//    }
//}

//QPainterPath MacroNodeInstance::getShapePath(QString pathType){
//    QPainterPath path;
//    path.moveTo(A1);
//    path.lineTo(A2);
//    path.lineTo(D2);
//    path.lineTo(D1);
//    path.closeSubpath();
//    return path;
//}


/// moved to NodeInstance.cpp
//void MacroNodeInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

//    //painter->drawText(macroRect, Qt::AlignLeft, "MACRO");
//}
