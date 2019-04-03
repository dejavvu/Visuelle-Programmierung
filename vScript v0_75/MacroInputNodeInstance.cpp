#include "MacroInputNodeInstance.h"

#include "QGraphicsSceneMouseEvent"
#include "mainwindow.h"
#include "MacroNode.h"
#include "datainterface.h"
#include "graphwidget.h"
#include "MacroInputNode.h"
#include "MacroNodeInstance.h"

#include <QPainter>
#include <QDebug>

MacroInputNodeInstance::MacroInputNodeInstance(MacroInputNode *parentInput, MacroNode *parentMacroNodeInput, MainWindow *mainWindowInput)
    : NodeInstance(parentInput, 0, mainWindowInput, nullptr)
{
    parentMacroNode = parentMacroNodeInput;

    //cornerRadius = 25;

//    displayNameFont = QFont("Corbel");
//    displayNameFont.setBold(true);
//    displayNameFont.setItalic(true);
//    displayNameFont.setPointSize(25);
    scale = 2.0;

    setInpOutPositions();
}

QString MacroInputNodeInstance::getCode(PortInstGate *otherGate){
    /// INPORTANT INFO
    /// in the other getCode()s, I do NodeInstance::getCode() which
    /// adds myself to a list of mainWindow to be able to determine
    /// when I entered an infinite loop. But in this case, it doesn't
    /// make sence, because I can get called multiple times accuratly
    /// but from differen macroNodeInstances,
    /// so here no NodeInstance::getCode()

    qDebug() << "getting code from InputNodeInstance";
    // if this gets called, this is an active node, see MacroNodeInstance::getCode()
    for(int i=0; i<parentMacroNode->inputs.length(); i++){
        if(parentMacroNode->inputs.at(i)->type == "exec"){
            qDebug() << outputPortInstances;
            if(outputPortInstances.at(i)->deio->connectionsTo.length() > 0){
                return outputPortInstances.at(i)->deio->connectionsTo.first()->parentNode->getCode(otherGate);
            }
        }
    }
    return "ALERT: no connected execution in MacroInputNodeInstance";
}

QString MacroInputNodeInstance::getFinalOutputInternalName(int index, PortInstGate *otherGate){

    if(parentMacroNode->currentInstanceForGettingCode != nullptr){
        PortInstance *macroNI_ipi = parentMacroNode->currentInstanceForGettingCode->inputPortInstances.at(index);
        if(macroNI_ipi->deio->connectionsTo.length() > 0){
            // here, I go to the MacroNodeInstance, that the application is trying to get the code of
            // here, I look which nodeinstance and which output is connected to the corresponding input of the
            // MacroNodeInstance, to and get it's final internal output name
            if(macroNI_ipi->deio->connectionsTo.isEmpty()){
                return "";
            }
            PortInstGate *connectedPIGate = macroNI_ipi->deio->connectionsTo.first();
            PortInstance *connectedPI = connectedPIGate->parentInterfaceInstance;
            NodeInstance *connectedNodeInstance = connectedPIGate->parentNode;
            int connectedPI_outputIndex = connectedNodeInstance->outputPortInstances.indexOf(connectedPI);
            return connectedNodeInstance->getFinalOutputInternalName(connectedPI_outputIndex, macroNI_ipi->deio);
        }
    }

    return "(ALERT macro input is not connected)";
}

//void MacroInputNodeInstance::updateOutputs(){


//    // REMOVE OLD PORTS
//    for(int i=0; i < outputPortInstances.length(); i++){ // iterate over all outputInstances
//        PortInstGate *d = outputPortInstances[i]->deio;
//        DataExecIOLabel *l = outputPortInstances[i]->label;

//        // if the stored parentInterfaceInstance of the inputPortInstance cannot be found in the inputs of the parentMacroNode, delete it because it doesn't exist anymore
//        if(parentNode->inputs.indexOf(static_cast<MacroOutputPortInstance*>(d->parentInterfaceInstance)->parentInputPortOfMacroNode) == -1){
//            // remove all connections
//            for(PortInstGate *otherGate : d->connectionsTo){
//                otherGate->connectionsTo.removeOne(d);
//            }

//            graph->scene()->removeItem(d);
//            graph->scene()->removeItem(l);
//            outputPortInstances.removeAt(i);
//            i--;
//        }
//    }

//    // ADD NEW PORTS
//    QVector<OutputPort*> parentOutputInterfaces;
//    for(PortInstance *dii : outputPortInstances){
//        parentOutputInterfaces.append(dii->deio->output_parentPort);
//    }
//    for(InputPort *ip : parentNode->inputs){ // iterate over all dataInterfaceOutputs of the parentNode
//        bool referenceFound = false;
//        for(PortInstance *opi : outputPortInstances){
//            if(static_cast<MacroOutputPortInstance*>(opi)->parentInputPortOfMacroNode == ip){
//                referenceFound = true;
//                break;
//            }
//        }
//        if(!referenceFound){    // if the interface is not found in the instances, add a new
//            OutputPort *p = new OutputPort;
//            p->displayName = ip->displayName;
//            p->type = ip->type;
//            outputs.append(p); // see declaration of outputs in header file
//            MacroOutputPortInstance *dii = new MacroOutputPortInstance;
//            dii->deio = new PortInstGate(p, this, dii);
//            dii->label = new DataExecIOLabel(p, this, dii);
//            dii->parentInputPortOfMacroNode = ip;
//            outputPortInstances.append(dii);

//            graph->scene()->addItem(outputPortInstances.last()->deio);
//            graph->scene()->addItem(outputPortInstances.last()->label);
//        }
//    }

//    // BRING THEM ALL IN THE RIGHT SEQUENCE


//    // UPDATE EXISTING PORTS
//    for(int i=0; i<outputs.length(); i++){
//        OutputPort *op = outputs[i];
//        op->displayName = parentNode->inputs[i]->displayName;
//        if(op->type != parentNode->inputs[i]->type){
//            // remove all connections
//            PortInstGate *p = outputPortInstances[i]->deio;
//            for(int j=0; j<p->connectionsTo.length(); j++){
//                PortInstGate *otherGate = p->connectionsTo[j];
//                otherGate->connectionsTo.removeOne(p);
//                p->connectionsTo.removeOne(otherGate);
//            }
//        }
//        op->type = parentNode->inputs[i]->type;
//    }


//    // update widths
//    for(int i=0; i<outputPortInstances.length(); i++){
//        DataExecIOLabel *l = outputPortInstances[i]->label;
//        PortInstGate *deio = outputPortInstances[i]->deio;

//        QFontMetricsF labelFM = QFontMetricsF(l->font);
//        l->width = labelFM.width(l->output_parentPort->displayName);

//        l->output_parentPort->width = deio->width + l->width;
//    }

//}

//void MacroInputNodeInstance::updateInputs(){

//}

// DOES IT HAS TO BE HERE???
void MacroInputNodeInstance::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = true;
        mousePressPoint = event->pos();
    }
    QGraphicsItem::mousePressEvent(event);
}

void MacroInputNodeInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void MacroInputNodeInstance::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(leftMousePressed){
        QPointF newPos(mapToScene(event->pos() - mousePressPoint));
        this->setPos(newPos);
        setInpOutPositions();
    }
    QGraphicsItem::mouseMoveEvent(event);
}


//void MacroInputNodeInstance::computeShapePoints(){
//    //... TODO
//}

//void MacroInputNodeInstance::computeWidthAndHeight(){
//    //... TODO
//}

//QPainterPath MacroInputNodeInstance::getShapePath(QString pathType){
//    //... TODO
//}


//void MacroInputNodeInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

//    computeWidthAndHeight();
//    computeShapePoints();
//    setInpOutPositions();

//    painter->scale(scale, scale);

//    QRectF displayNameRect(B1, H2);

//    QPainterPath path = getShapePath("");


//    painter->setPen(QPen(QColor("black"), parentNode->penWidth));
//    painter->setBrush(designStyle != "light" ? QBrush(color) : QBrush(QColor(color.red(),
//                                                                                         color.green(),
//                                                                                         color.blue(),
//                                                                                         color.alpha())));
//    painter->drawPath(path);

//    int titleColorOffset = 0;
//    if(designStyle == "light"){
//        titleColorOffset = -100;
//    }else{
//        titleColorOffset = +100;
//    }
//    int red = color.red()+titleColorOffset;
//    int green = color.green()+titleColorOffset;
//    int blue = color.blue()+titleColorOffset;

//    red = red < 0 ? 0 : red;
//    red = red > 255 ? 255 : red;
//    green = green < 0 ? 0 : green;
//    green = green > 255 ? 255 : green;
//    blue = blue < 0 ? 0 : blue;
//    blue = blue > 255 ? 255 : blue;

//    QPen pen(QColor(red, green, blue, 255),
//             parentNode->penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
//    painter->setPen(pen);
//    painter->setFont(displayNameFont);
//    painter->drawText(displayNameRect, Qt::AlignHCenter , "input");
//}
