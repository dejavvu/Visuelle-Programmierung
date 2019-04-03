#include "CodeNodeInstance.h"

#include "node.h"
#include "graphwidget.h"
#include "mainwindow.h"
#include "CodeNode.h"
#include "PortInstGate.h"
#include "DataExecIOLabel.h"
#include "InputNodeInstance.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QChar>
#include <QSpinBox>
#include <QLineEdit>

//QFont CodeNodeInstance::codeNodeInst_displaynameFont = QFont("K2D", 15, QFont::Bold, true);

CodeNodeInstance::CodeNodeInstance(CodeNode *parentInput, int instanceNumberInput, MainWindow *mainWindowInput, GraphWidget *graphInput)
    : NodeInstance(parentInput, instanceNumberInput, mainWindowInput, graphInput)
{
    parentNode = parentInput;

    displayNameFont = QFont("K2D", 15, QFont::Bold, true);

//    displayNameFont.setBold(true);
//    displayNameFont.setItalic(true);
//    displayNameFont.setPointSize(15);

    //cornerRadius = 15;

    updateFontSize();
    updateYourself();
}

//void CodeNodeInstance::changeSize(QString side, double mult){
//    computeWidthAndHeight();
//    scale *= mult;
//    setInpOutPositions();
//    update();
//}

QString CodeNodeInstance::getCode(PortInstGate *otherGate){
    //qDebug() << "getting code from CodeNodeInstance: " << parentNode->title + QString::number(this->instanceNumber);
    QString nodeInstParentCode = NodeInstance::getCode(otherGate);
    if(!nodeInstParentCode.isEmpty()){
        return nodeInstParentCode;
    }

    QString finalCode = "";
    QString mainCode = parentNode->metaCode;

    if(parentNode->nodeExecType == "data"){
        finalCode = replaceInputsFromCode(mainCode);
    }else{
        // prepare mainCode
        mainCode = replaceInputsFromCode(mainCode);
        mainCode = replaceOutputsFromCode(mainCode);

        finalCode += mainCode;

///  until now, data outputs are always passive. can't remember what this was for right now ...
/// DO NOT DELETE WITHOUT UNDERSTANDING
//        int dataInterfaceIndexCounter = 0;

//        // add data-output code to the final code
//        for(int i=0; i<outputPortInstances.length(); i++){
//            PortInstGate *deio = outputPortInstances[i]->deio;
//            OutputPort *o = deio->output_parentPort;
//            if(o->type == "data"){
//                QString finalOutputCode = "";
//                if(o->activationType != "passive" && parentNode->nodeExecType == "exec"){
//                    finalOutputCode = o->code;
//                    finalOutputCode.replace(o->internalName, finalOutputInternalNames.at(dataInterfaceIndexCounter));
//                    dataInterfaceIndexCounter++;
//                    finalOutputCode = replaceInputsFromCode(finalOutputCode);

//                    finalCode += "\n" + finalOutputCode;
//                }else{
//                    //finalOutputCode = o->internalName;
//                    //finalCode += finalOutputCode;
//                }
//            }else if(o->type == "exec"){

//            }
//        }
/// DO NOT DELETE WITHOUT UNDERSTANDING

    }


//    qDebug() << "Code of parentNode is: ";
//    qDebug() << parentNode->code;
//    qDebug() << "my final code is now: ";
//    qDebug() << finalCode;


    return finalCode;
}

QString CodeNodeInstance::replaceInputsFromCode(QString code){

    qDebug() << "replacing inputs from code in " << parentNode->title << " with " << code;

    QRegularExpression input_re("%INPUT\\d+%");
    while(code.indexOf(input_re) != -1){

        ///get input number
        int index = code.indexOf(input_re);
        int numberIndex = code.indexOf(QRegularExpression("\\d+"), index);
        int numberEndIndex = code.indexOf("%", numberIndex);
        int len = (numberEndIndex+1) - index;
        QString inputNumberStr = "";
        for(int i=numberIndex; i<numberEndIndex; i++){
            inputNumberStr.append(code.at(i));
        }
        int inputNumber = inputNumberStr.toInt();
        qDebug() << "inputNumber: " << inputNumber;
        int inputIndex = inputNumber-1;

        QString s = "";

        //qDebug() << "1";
        if(inputPortInstances[inputIndex]->deio->connectionsTo.length() == 1){ /// if the input is connected
            qDebug() << "input is connected";
            /// get connected nodeInstance
            NodeInstance *connectedNodeInstance = inputPortInstances.at(inputIndex)->deio->connectionsTo[0]->parentNode;
//            if(connectedNodeInstance->parentNode->nodeExecType == "exec"){ // if the connected instance is exec type (like "if"), get the final internal output name of the connected output
//                int deioIndexAtOtherCodeNode = connectedNodeInstance->outputPortInstances.indexOf(inputPortInstances.at(inputNumber-1)->deio->connectionsTo[0]->parentInterfaceInstance);
//                s = connectedNodeInstance->getFinalOutputInternalName(deioIndexAtOtherCodeNode); // old: finalOutputInternalNames.at(deioIndexAtOtherCodeNode);
//            }else if(connectedNodeInstance->parentNode->nodeExecType == "data"){ // if the connected instance is data type (like "=="), just get it's whole code
//                s = connectedNodeInstance->getCode();
//            }
            int deioIndexAtOtherCodeNode = connectedNodeInstance->outputPortInstances.indexOf(inputPortInstances.at(inputNumber-1)->deio->connectionsTo[0]->parentInterfaceInstance);
            qDebug() << "deioIndexAtOtherCodeNode: " << deioIndexAtOtherCodeNode;
            s = connectedNodeInstance->getFinalOutputInternalName(deioIndexAtOtherCodeNode, inputPortInstances[inputIndex]->deio); // old: finalOutputInternalNames.at(deioIndexAtOtherCodeNode);

            qDebug() << "s: " << s;

//            qDebug() << "final internal name of OutputDataInterface of " << this->parentNode->title + QString::number(this->instanceNumber) << " is: " << s;
//            qDebug() << "index: " << index;
//            qDebug() << "numberIndex: " << numberIndex;
//            qDebug() << "numberEndIndex: " << numberEndIndex;
//            qDebug() << "length: " << len;

        }else{ /// if the input is not connected, just get the data from the widget
            qDebug() << "input isn't connected";
            if(inputPortInstances[inputIndex]->wdgt != nullptr){
                //qDebug() << "5";
                QSpinBox *spinBoxWdgt = dynamic_cast<QSpinBox*>(inputPortInstances[inputIndex]->wdgt);
                QLineEdit *lineEditWdgt = dynamic_cast<QLineEdit*>(inputPortInstances[inputIndex]->wdgt);
                if(spinBoxWdgt){
                    s = QString::number(spinBoxWdgt->value());
                }else if(lineEditWdgt){
                    s = lineEditWdgt->text();
                }
                //qDebug() << "6";
            }
        }
        //qDebug() << "7";
        /// replace the %INPUTX%-notation with the generated code
        code.replace(index, len, s);
        qDebug() << "code is now: " << code << " in " << parentNode->title;

    }

    return code;
}

QString CodeNodeInstance::replaceOutputsFromCode(QString code){

    QRegularExpression output_re("%OUTPUT\\d+%");
    while(code.indexOf(output_re) != -1){

        //get output number
        int index = code.indexOf(output_re);
        int numberIndex = code.indexOf(QRegularExpression("\\d+"), index);
        int numberEndIndex = code.indexOf("%", numberIndex);
        int len = (numberEndIndex+1) - index;
        QString outputNumberStr = "";
        for(int i=numberIndex; i<numberEndIndex; i++){
            outputNumberStr.append(code.at(i));
        }
        int outputNumber = outputNumberStr.toInt();
        int outputIndex = outputNumber-1;

        QString s = "";

        if(outputPortInstances[outputIndex]->deio->connectionsTo.length() == 1){ // if output is connected
            NodeInstance *otherNodeInstance = outputPortInstances.at(outputIndex)->deio->connectionsTo[0]->parentNode;
            s = otherNodeInstance->getCode(outputPortInstances[outputIndex]->deio);
        }else{ // if output is not connected
            s = ""; //"_NO_NODE_CONNECTED_";
        }

        // compute indent
        QString indent = "";
        for(int i=index-1; i>=0; i--){
            if(code[i] == " "){
                indent.append(" ");
            }else if(code[i] == "\n"){
                break;
            }else{
                indent = "";
                break;
            }
        }
        // add the indent to every newline
        s.replace('\n', '\n'+indent);
        // replace the notation with the generated code
        code.replace(index, len, s);
    }

    return code;
}

QString CodeNodeInstance::getFinalOutputInternalName(int index, PortInstGate *otherGate){
    qDebug() << "getting final output internal name in " << parentNode->title;

    // it happens, that a NodeInstance tries to get the output data of a CodeNodeInstance, but
    // this CodeNodeInstance hasn't created it's output names yet. So, here I need to do this,
    // if it is not already done before (here or in mainWindow)
    // OR
    // if I am currently inside a macro, because, then, I have to create all internal output
    // names new, because mainWindow could not create them correctly, because of the 1:N
    // relationship between MacroNode and MacroNodeInstance
    if(finalOutputInternalNames.isEmpty() || (graph != nullptr && graph->parentMacroNode != nullptr /*-> I'm inside a macro*/)){
        finalOutputInternalNames.clear();
        replaceInputsFromOutputNames();
    }
    return NodeInstance::getFinalOutputInternalName(index, otherGate);
}

void CodeNodeInstance::replaceInputsFromOutputNames(){
    qDebug() << "replacing inputs from code in getFinalOutputInternalNames in " << parentNode->title;
    for(int i=0; i<outputPortInstances.length(); i++){
        QString internalOutputName = outputPortInstances[i]->deio->output_parentPort->internalName;
        QRegularExpression input_re("%INPUT\\d+%");
        if(internalOutputName.indexOf(input_re) != -1){
            finalOutputInternalNames.append(replaceInputsFromCode(internalOutputName));
        }
    }
}

void CodeNodeInstance::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = true;
        mousePressPoint = event->pos();
    }
    QGraphicsItem::mousePressEvent(event);
}

void CodeNodeInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        leftMousePressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void CodeNodeInstance::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(leftMousePressed && (graph->touchMode == "edit" || graph->touchMode == "")){
        QPointF newPos(mapToScene(event->pos() - mousePressPoint));
        this->setPos(newPos);
        setInpOutPositions();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void CodeNodeInstance::updateFontSize(){
    if(parentNode->nodeExecType == "exec"){
        displayNameFont.setPointSize(15);
    }else if(parentNode->nodeExecType == "data"){
        displayNameFont.setPointSize(10);
    }
}

void CodeNodeInstance::computeShapePoints(){
    int r = CodeNodeInstance::cornerRadius;
    A1.setX( - width/2.0 ); A1.setY( - height/2.0 );
    A2.setX( + width/2.0 ); A2.setY( - height/2.0 );
    D1.setX( - width/2.0 ); D1.setY( + height/2.0 );
    D2.setX( + width/2.0 ); D2.setY( + height/2.0 );
    E1.setX( - width/2.0 + r ); E1.setY( - height/2.0 );
    E2.setX( + width/2.0 - r ); E2.setY( - height/2.0 );
    L1.setX( - width/2.0 ); L1.setY( - height/2.0 + r );
    L2.setX( + width/2.0 ); L2.setY( - height/2.0 + r );
    C1.setX( - width/2.0 ); C1.setY( + height/2.0 - r );
    C2.setX( + width/2.0 ); C2.setY( + height/2.0 - r );
    M1.setX( - width/2.0 + r ); M1.setY( + height/2.0 );
    M2.setX( + width/2.0 - r ); M2.setY( + height/2.0 );

    B1.setX( 0.0 - displayNameFontWidth/2.0 ); B1.setY( 0.0 - displayNameFontHeight/2.0 );
    B2.setX( 0.0 + displayNameFontWidth/2.0 ); B2.setY( 0.0 - displayNameFontHeight/2.0 );
    H1.setX( 0.0 - displayNameFontWidth/2.0 ); H1.setY( 0.0 + displayNameFontHeight/2.0 );
    H2.setX( 0.0 + displayNameFontWidth/2.0 ); H2.setY( 0.0 + displayNameFontHeight/2.0 );
}

/// void omputeWidthAndHeight() moved to NodeInstance.cpp

QPainterPath CodeNodeInstance::getShapePath(QString pathType){
    QPainterPath path;
    if(pathType == "normal rounded"){

        QPointF middleLeft(-width/2, 0);
        QPointF middleRight(+width/2, 0);
        QPointF middleTop(0, -height/2);
        QPointF middleBottom(0, height/2);
        path.moveTo( L1.y() < middleLeft.y() ? L1 : middleLeft );
        path.cubicTo(A1, A1, E1.x() < middleTop.x() ? E1 : middleTop );
        path.lineTo(E2.x() > middleTop.x() ? E2 : middleTop);
        path.cubicTo(A2, A2, L2.y() < middleRight.y() ? L2 : middleRight);
        path.lineTo(C2.y() > middleRight.y() ? C2 : middleRight);
        path.cubicTo(D2, D2, M2.x() > middleBottom.x() ? M2 : middleBottom);
        path.lineTo(M1.x() < middleBottom.x() ? M1 : middleBottom);
        path.cubicTo(D1, D1, C1.y() > middleLeft.y() ? C1 : middleLeft);
        path.closeSubpath();

//        if(L1.y() < C2.y()){
//            path.moveTo(L1);
//            path.cubicTo(A1, A1, E1);
//            path.lineTo(E2);
//            path.cubicTo(A2, A2, L2);
//            path.lineTo(C2);
//            path.cubicTo(D2, D2, M2);
//            path.lineTo(M1);
//            path.cubicTo(D1, D1, C1);
//            path.closeSubpath();
//        }else{
//            QPointF middleLeft(-width/2, 0);
//            QPointF middleRight(+width/2, 0);
//            QPointF middleTop(0, -height/2);
//            QPointF middleBottom(0, height/2);
//            path.moveTo(middleLeft);
//            path.cubicTo(A1, A1, E1);
//            path.lineTo(E2);
//            path.cubicTo(A2, A2, middleRight);
//            path.cubicTo(D2, D2, M2);
//            path.lineTo(M1);
//            path.cubicTo(D1, D1, middleLeft);
//            path.closeSubpath();
//        }
    }else if(pathType == "corner round"){

    }
    return path;
}

void CodeNodeInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qDebug() << "repainting CodeNodeInstance";
    setInpOutPositions();
    applyStylesheetToContent(graph->graphContentStyleSheet);

    painter->scale(scale, scale);

    QRectF displayNameRect(B1, H2);

    QPainterPath path = getShapePath("normal rounded");


    painter->setPen(QPen(QColor(borderColor), NodeInstance::borderPenWidth));
    painter->setBrush(designStyle != "light" ? QBrush(parentNode->color) : QBrush(QColor(parentNode->color.red(),
                                                                                         parentNode->color.green(),
                                                                                         parentNode->color.blue(),
                                                                                         (parentNode->color.alpha()*2 > 255 ? 255 : parentNode->color.alpha()*2))));
    painter->drawPath(path);

    int titleColorOffset = 0;
    if(designStyle == "light"){
        titleColorOffset = -100;
    }else{
        titleColorOffset = +100;

    }
    int red = parentNode->color.red()+titleColorOffset;
    int green = parentNode->color.green()+titleColorOffset;
    int blue = parentNode->color.blue()+titleColorOffset;

    red = red < 0 ? 0 : red;
    red = red > 255 ? 255 : red;
    green = green < 0 ? 0 : green;
    green = green > 255 ? 255 : green;
    blue = blue < 0 ? 0 : blue;
    blue = blue > 255 ? 255 : blue;

    QPen pen(QColor(red, green, blue, 50),
             NodeInstance::borderPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    painter->setFont(displayNameFont);
    painter->drawText(displayNameRect, Qt::AlignHCenter , parentNode->title);

}
