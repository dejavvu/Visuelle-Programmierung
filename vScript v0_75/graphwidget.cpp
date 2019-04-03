#include <QDebug>
#include <vector>
#include <list>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QTimeLine>
#include <QWheelEvent>
#include <QScrollBar>
#include <QColor>
#include <QRgb>
#include <QImage>
#include <QPointF>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QMainWindow>
#include <QWidget>
#include <QVector>
#include <QColorDialog>
#include <QtMath>
#include <QGraphicsProxyWidget>
#include <QGestureEvent>
#include <QCoreApplication>
#include <QCheckBox>

#include "graphwidget.h"
#include "mainwindow.h"

#include "node.h"
#include "FunctionNode.h"
#include "CodeNode.h"
#include "NodeInstance.h"
#include "FunctionNodeInstance.h"
#include "CodeNodeInstance.h"
#include "InputNodeInstance.h"
#include "InputNode.h"
#include "OutputNodeInstance.h"
#include "OutputNode.h"
#include "PortInstGate.h"
#include "datainterface.h"
#include "FunctionNodeDetailsForm.h"
#include "ZoomSlider.h"
#include "MacroNode.h"
#include "MacroNodeInstance.h"
#include "MacroInputNodeInstance.h"
#include "MacroOutputNodeInstance.h"
//#include "ui_graphwidget.h"

static const int GRID_STEP = 30;
QString GraphWidget::designStyle = "dark";
double GraphWidget::connectionsWidth = 1.0;
QColor GraphWidget::bgColor = QColor(32, 32, 32, 255);
QColor GraphWidget::gridColor = QColor(52, 52, 52, 255);
QString GraphWidget::gridType = "+";

GraphWidget::GraphWidget(QWidget *parent, MainWindow *mainWindowInput, MacroNode *parentMacroNodeInput) :
    QGraphicsView(parent)
{
    mainWindow = mainWindowInput;

    absoluteScale = 1.0;
    rightMousePressed = false;
    leftMousePressed = false;
    connectionDragging = false;
    mouseMovedWhilePressing = true;
    selectingMultipleNodes = false;
    draggingNode = false;
    zoomSliderPressed = false;
    zooming = false;
    totalScaleFactor = 1;
    touchMode = "";
    touchModeActive = false;

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-1750, -1000, 3500, 2105);

    // recognizing pinch by touch
    grabGesture(Qt::PinchGesture);

    setScene(scene);
    setCacheMode( CacheBackground );
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint((QPainter::Antialiasing));
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(4.0), qreal(4.0));

    setUpProxys();

    customEventFilter = new Graph_KeyPressEventFilter(this);
    installEventFilter(customEventFilter);


    allNodeInstances = new QList<QList<NodeInstance*>>();
    parentMacroNode = parentMacroNodeInput;
    if(parentMacroNode != nullptr){
        allNodeInstances = &parentMacroNode->containedNodeInstances;
        for(QList<NodeInstance*> &l : *allNodeInstances){
            for(NodeInstance *ni : l){
                ni->graph = this;
                scene->addItem(ni);
                ni->addContentToScene(scene);
            }
        }
        MacroInputNodeInstance *mini = parentMacroNode->inputNodeInstance;
        mini->graph = this;
        scene->addItem(mini);
        mini->addContentToScene(scene);
        if(mini->pos() == QPointF(0,0)){
            mini->setPos(-(scene->width()/6), 0);
        }
        mini->updateYourself();

        MacroOutputNodeInstance *moni = parentMacroNode->outputNodeInstance;
        moni->graph = this;
        scene->addItem(moni);
        moni->addContentToScene(scene);
        if(moni->pos() == QPointF(0,0)){
            moni->setPos(+(scene->width()/6), 0);
        }
        moni->updateYourself();
    }

    setUpScrollBars();

    setSelectedNodeInstance(nullptr);

}

GraphWidget::~GraphWidget()
{
    //delete ui;
}

QList<NodeInstance *> &GraphWidget::allNodeInstances_getRefListAt(int i){
    return (*allNodeInstances)[i]; // https://forum.qt.io/topic/98053/referencing-to-a-qlist-from-a-qlist-qlist-x/4
//or: return allNodeInstances->operator [](i);
}

QList<NodeInstance*> GraphWidget::getAllInstances_includingInpOut(){
    QList<NodeInstance*> instances;
    if(parentMacroNode != nullptr){
        instances.append(parentMacroNode->inputNodeInstance);
        instances.append(parentMacroNode->outputNodeInstance);
    }
    for(NodeInstance *ni : mainWindow->nodeInstList2Dto1D(*allNodeInstances)){
        instances.append(ni);
    }

    return instances;
}

void GraphWidget::setUpProxys(){

    // zoom slider proxy
    // -----------------------------------------------------------------
    zoomSlider_Proxy = new QGraphicsProxyWidget();
    zoomSlider_Proxy->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    zoomSlider = new ZoomSlider(Qt::Vertical, this);
    zoomSlider->setGeometry(0, 0, 50, 300);
    zoomSlider_Proxy->setWidget(zoomSlider);
    // proxy->setOpacity(0.9999999); // https://forum.qt.io/topic/98014/qgraphicsview-scene-paints-over-other-widgets/9
    scene()->addItem(zoomSlider_Proxy);
    zoomSlider_Proxy->setPos(mapToScene(0, 0));
    // -----------------------------------------------------------------



    // proxy for button that changes mode to edit
    // -----------------------------------------------------------------
    changeTouchModeToEdit_Proxy = new QGraphicsProxyWidget();
    changeTouchModeToEdit_Proxy->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    changeTouchModeToEdit_Button = new QPushButton();

    // connecting the pressed signal to local slot here
    connect(changeTouchModeToEdit_Button, SIGNAL(pressed()),
            this, SLOT(on_touchModeChangedToEdit()));

    // setting the stylesheet
    QString dir = QCoreApplication::applicationDirPath();
    QFile editModeButtonStylesheetFile(dir+"/../custom content/StyleSheets/TouchModeButtonsSS.txt");
    editModeButtonStylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString editModeButtonStylesheetFileContent = editModeButtonStylesheetFile.readAll();
    editModeButtonStylesheetFile.close();
    changeTouchModeToEdit_Button->setStyleSheet(editModeButtonStylesheetFileContent);

    // setting the Icon
    changeTouchModeToEdit_Button->setGeometry(0, 0, 80, 80);
    changeTouchModeToEdit_Button->setIcon(QIcon(":/Images/cursor_normal.svg"));
    changeTouchModeToEdit_Proxy->setWidget(changeTouchModeToEdit_Button);
    scene()->addItem(changeTouchModeToEdit_Proxy);
    changeTouchModeToEdit_Proxy->setPos(mapToScene(0, 0));
    // -----------------------------------------------------------------



    // proxy for button that changes mode to scroll
    // -----------------------------------------------------------------
    changeTouchModeToScroll_Proxy = new QGraphicsProxyWidget();
    changeTouchModeToScroll_Proxy->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    changeTouchModeToScroll_Button = new QPushButton();

    // connecting the pressed signal to local slot here
    connect(changeTouchModeToScroll_Button, SIGNAL(pressed()),
            this, SLOT(on_touchModeChangedToScroll()));

    // setting the stylesheet
    QFile scrollModeButtonStylesheetFile(dir+"/../custom content/StyleSheets/TouchModeButtonsSS.txt");
    scrollModeButtonStylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString scrollModeButtonStylesheetFileContent = scrollModeButtonStylesheetFile.readAll();
    scrollModeButtonStylesheetFile.close();
    changeTouchModeToScroll_Button->setStyleSheet(scrollModeButtonStylesheetFileContent);

    // setting the Icon
    changeTouchModeToScroll_Button->setGeometry(0, 0, 80, 80);
    changeTouchModeToScroll_Button->setIcon(QIcon(":/Images/cursor_hand.svg"));
    changeTouchModeToScroll_Proxy->setWidget(changeTouchModeToScroll_Button);
    scene()->addItem(changeTouchModeToScroll_Proxy);
    changeTouchModeToScroll_Proxy->setPos(mapToScene(0, 0));
    // -----------------------------------------------------------------



    // proxy for checkbox to enable or disable touch mode
    // -----------------------------------------------------------------
    enableTouchMode_Proxy = new QGraphicsProxyWidget();
    enableTouchMode_Proxy->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    enableTouchMode_CheckBox = new QCheckBox();

    // connecting the changed signal to local slot here
    connect(enableTouchMode_CheckBox, SIGNAL(toggled(bool)),
            this, SLOT(on_enableTouchModeCheckBox_stateChanged(bool)));

    // setting the stylesheet
    QFile enableTouchModeCheckBox_StylesheetFile(dir+"/../custom content/StyleSheets/TouchModeCheckBoxSS.txt");
    enableTouchModeCheckBox_StylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString enableTouchModeCheckBox_StylesheetFileContent = enableTouchModeCheckBox_StylesheetFile.readAll();
    enableTouchModeCheckBox_StylesheetFile.close();
    enableTouchMode_CheckBox->setStyleSheet(enableTouchModeCheckBox_StylesheetFileContent);

    enableTouchMode_Proxy->setScale(1);
    enableTouchMode_CheckBox->setGeometry(0,0,80,80);
    enableTouchMode_Proxy->setWidget(enableTouchMode_CheckBox);
    scene()->addItem(enableTouchMode_Proxy);
    // -----------------------------------------------------------------
}

void GraphWidget::placeInputAndOutput(InputNode *inpNode, OutputNode *otpNode){
    // INPUT
//    if(activeFunctionNode->inputNodeInst == nullptr){
//        activeFunctionNode->inputNodeInst = new InputNodeInstance(inpNode, mainWindow, this);
//        activeFunctionNode->inputNodeInst->setPos(inpNode->position);
//    }
//    inputNodeInst = activeFunctionNode->inputNodeInst; // the functionNode holds its own instance here
//    scene()->addItem(inputNodeInst);
//    inputNodeInst->addContentToScene(scene());

//    // OUTPUT
//    if(activeFunctionNode->outputNodeInst == nullptr){
//        activeFunctionNode->outputNodeInst = new OutputNodeInstance(otpNode, mainWindow, this);
//        activeFunctionNode->outputNodeInst->setPos(otpNode->position);
//    }
//    outputNodeInst = activeFunctionNode->outputNodeInst; // the functionNode holds its own instance here
//    scene()->addItem(outputNodeInst);
//    outputNodeInst->addContentToScene(scene());
}

NodeInstance* GraphWidget::placeExistingNode(Node *node, QPointF pos){

    /*if(node == activeFunctionNode){
        return nullptr;
    }*/

    int newInstanceNumber = 1;
    int allNodesIndex = -1;

    for(int i=0; i<allNodeInstances->length(); i++){
        if(allNodeInstances->at(i).at(0)->parentNode->number == node->number){
            allNodesIndex = i;
            break;
        }
    }

    if(allNodesIndex == -1){
        QList<NodeInstance*> newNodeInstanceList;
        allNodeInstances->append(newNodeInstanceList);
        allNodesIndex = allNodeInstances->length() - 1;
    }
    if(allNodeInstances->at(allNodesIndex).length() > 0){
        newInstanceNumber = allNodeInstances->at(allNodesIndex).last()->instanceNumber + 1;
    }

    NodeInstance *newNode;

    if(node->nodeType == "macro"){
        newNode = new MacroNodeInstance(static_cast<MacroNode*>(node), newInstanceNumber, mainWindow, this);
    }else if(node->nodeType == "code"){
        newNode = new CodeNodeInstance(static_cast<CodeNode*>(node), newInstanceNumber, mainWindow, this);
    }else{
        newNode = new NodeInstance(static_cast<Node*>(node), newInstanceNumber, mainWindow, this);
    }

    allNodeInstances_getRefListAt(allNodesIndex).append(newNode);
    allNodeInstances->at(allNodesIndex).last()->setPos(pos);
    newNode->setDesignStyle(GraphWidget::designStyle);
    scene()->addItem(allNodeInstances->at(allNodesIndex).last());
    setSelectedNodeInstance(allNodeInstances->at(allNodesIndex).last());
    node->instances.append(newNode);
    //if(activeFunctionNode != nullptr){activeFunctionNode->subordinates = allNodeInstances;}

    if(connectedDataExecInterfaces.length() > 0){
        //qDebug() << "trying to connect";
        tryToConnectNodes(connectedDataExecInterfaces.first(), selectedNodeInstance);
    }
    updateInstancesAndRepaint();
    updateInstancesAndRepaint(); // for repainting reasons, this is nesseccairy
    return allNodeInstances->at(allNodesIndex).last();
}

void GraphWidget::placeExistingNodeInstance(NodeInstance *instance, QPointF pos){

    int allNodesIndex = -1;
    for(int i=0; i<allNodeInstances->length(); i++){
        if(allNodeInstances->at(i).at(0)->parentNode->number == instance->parentNode->number){
            allNodesIndex = i;
            break;
        }
    }
    if(allNodesIndex == -1){
        QList<NodeInstance*> newNodeInstanceList;
        allNodeInstances->append(newNodeInstanceList);
        allNodesIndex = allNodeInstances->length() - 1;
    }


    allNodeInstances_getRefListAt(allNodesIndex).append(instance);
    allNodeInstances->at(allNodesIndex).last()->setPos(pos);
    scene()->addItem(allNodeInstances->at(allNodesIndex).last());
    setSelectedNodeInstance(allNodeInstances->at(allNodesIndex).last());
    instance->parentNode->instances.append(instance);
    //if(activeFunctionNode != nullptr){activeFunctionNode->subordinates = allNodeInstances;}
}

void GraphWidget::updateInstancesAndRepaint(){
    for(int i=0; i<allNodeInstances->length(); i++){
        for(NodeInstance *instance : allNodeInstances->at(i)){
            instance->update();
        }
    }
    this->viewport()->repaint();
    update();
}

void GraphWidget::setUpScrollBars(){
    int hMax = horizontalScrollBar()->maximum();
    int hMin = horizontalScrollBar()->minimum();
    int horizontalCenter = (  hMin + hMax ) / 2;
    horizontalScrollBar()->setValue( horizontalCenter );

    int vMax = verticalScrollBar()->maximum();
    int vMin = verticalScrollBar()->minimum();
    int verticalCenter = (  vMin + vMax ) / 2;
    verticalScrollBar()->setValue( verticalCenter );


    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    horizontalScrollBar()->hide();
    verticalScrollBar()->hide();
}

void GraphWidget::setSelectedNodeInstance(NodeInstance *nodeInput){

//    if(selectedNode != nullptr){
//        if(selectedNode->detailsWidget != nullptr){
//            selectedNode->detailsWidget = mainWindow->getNodeDetailsWidget();
//            mainWindow->changeNodeDetailsWidget(nullptr);
//            // this was a huge thing. For explanation look here: https://forum.qt.io/topic/81907/scrollarea-setwidget-reusing-widget-crashes-program/2
//        }
//    }


    if(nodeInput != nullptr){
        if(selectedNodeInstance != nullptr){
            selectedNodeInstance->setSelectedInGraphStatus(false);
        }if(selectedNodeInstances.length() > 0){
            clearSelectedNodeInstances();
        }
        selectedNode = nodeInput->parentNode;
        selectedNodeInstance = nodeInput;
        selectedNodeInstance->setSelectedInGraphStatus(true);
        if(selectedNode->detailsWidget != nullptr){
            mainWindow->changeNodeDetailsWidget(selectedNode, selectedNode->detailsWidget);
        }
    }else if(nodeInput == nullptr && selectedNodeInstance != nullptr){
        selectedNodeInstance->setSelectedInGraphStatus(false);
        selectedNodeInstance = nullptr;
        selectedNode = nullptr;
        mainWindow->changeNodeDetailsWidget(nullptr, nullptr);
    }
}

void GraphWidget::addSelectedNodeInstance(NodeInstance *ni){
    ni->setSelectedInGraphStatus(true);
    selectedNodeInstances.append(ni);
}

void GraphWidget::copyNodeInstance(NodeInstance *ni){
    copiedNodeInstance = ni;
    copiedNodeInstances.clear();
}

void GraphWidget::copyNodeInstances(QList<NodeInstance *> l){
    copiedNodeInstances = l;
}

void GraphWidget::pasteCopiedNodeInstance(){
    placeExistingNode(selectedNodeInstance->parentNode, lastMouseMovePos);
}

void GraphWidget::pasteCopiedNodeInstances(){
    QPointF middlePos = getMiddlePosOfNodeInstances(copiedNodeInstances);
    clearSelectedNodeInstances();
    setSelectedNodeInstance(nullptr);
    for(NodeInstance *ni : copiedNodeInstances){
        //qDebug() << middlePos;
        //qDebug() << ni->pos();
        int xPosDiffToMiddle = ni->pos().x() - middlePos.x();
        //qDebug() << xPosDiffToMiddle;
        int yPosDiffToMiddle = ni->pos().y() - middlePos.y();
        //qDebug() << yPosDiffToMiddle;

        addSelectedNodeInstance(
                    placeExistingNode(ni->parentNode,
                                      QPointF(lastMouseMovePos.x() + xPosDiffToMiddle, lastMouseMovePos.y() + yPosDiffToMiddle))
                    );
    }
}

QPointF GraphWidget::getMiddlePosOfNodeInstances(QList<NodeInstance *> instances){
    int left = scene()->width();
    int up = scene()->height();
    int right = 0;
    int down = 0;
    for(NodeInstance *ni : instances){
        int ni_left = ni->pos().x()-(ni->width/2)*ni->scale;
        int ni_right = ni->pos().x()+(ni->width/2)*ni->scale;
        int ni_up = ni->pos().y()-(ni->height/2)*ni->scale;
        int ni_down = ni->pos().y()+(ni->height/2)*ni->scale;

        ni_left < left ? left = ni_left : left = left;
        ni_right > right ? right = ni_right : right = right;
        ni_up < up ? up = ni_up : up = up;
        ni_down > down ? down = ni_down : down = down;
    }
    return QPointF(qreal(left+(right-left)/2.0), qreal(up+(down-up)/2.0));
}

void GraphWidget::clearSelectedNodeInstances(){
    for(NodeInstance *ni : selectedNodeInstances){
        ni->setSelectedInGraphStatus(false);
    }
    selectedNodeInstances.clear();
}

void GraphWidget::deleteInstance(NodeInstance *ni){

    removeAllConnections(ni);

    // REMOVE FROM SCENE
    scene()->removeItem(ni);

    removeFromAllNodeList(ni);

    ni->deleteContent();

    setSelectedNodeInstance(nullptr);

    if(mainWindow->generateCodeSynchronous){
        mainWindow->generateCodeAndUpdateTextEdit();
    }
}

void GraphWidget::deleteInstances(QList<NodeInstance*> l){
    for(NodeInstance *ni : l){
        deleteInstance(ni);
    }
}

void GraphWidget::removeInstance(NodeInstance *ni){

    removeFromAllNodeList(ni);

    ni->graph = nullptr;

    // remove content from scene
    for(PortInstance *p : ni->inputPortInstances){
        scene()->removeItem(p->deio);
        scene()->removeItem(p->label);
        if(p->proxy != nullptr){
            scene()->removeItem(p->proxy);
        }
    }
    for(PortInstance *p : ni->outputPortInstances){
        scene()->removeItem(p->deio);
        scene()->removeItem(p->label);
        if(p->proxy != nullptr){
            scene()->removeItem(p->proxy);  // I guess, this is unesseccairy, but Im not sure
        }
    }

    scene()->removeItem(ni);

    setSelectedNodeInstance(nullptr);
}

void GraphWidget::removeInstances(QList<NodeInstance *> l){
    for(NodeInstance *ni : l){
        removeInstance(ni);
    }
}

void GraphWidget::removeFromAllNodeList(NodeInstance *ni){
    // REMOVE FROM ALLNODES-LIST
    qDebug() << allNodeInstances->length();
    for(int i=0; i<allNodeInstances->length(); i++){
        QList<NodeInstance*> &l = allNodeInstances_getRefListAt(i);
        if(l.indexOf(ni) != -1){
            qDebug() << "removing ni from l";
            qDebug() << "l.length(): " << l.length();
            qDebug() << "(*allNodeInstances)[i].length(): " << (*allNodeInstances)[i].length();
            l.removeOne(ni);
            qDebug() << "l.length(): " << l.length();
            qDebug() << "(*allNodeInstances)[i].length(): " << (*allNodeInstances)[i].length();
            if(l.length() == 0){
                allNodeInstances->removeAt(i);
            }
            break;
        }
    }
    qDebug() << allNodeInstances->length();
}

void GraphWidget::removeAllConnections(NodeInstance *ni){
    // REMOVE CONNECTIONS
    // remove inputs
    for(PortInstance *dii : ni->inputPortInstances){
        for(PortInstGate *deio_other : dii->deio->connectionsTo){
            disconnectedNodeInterface("output", deio_other);
        }
        disconnectedNodeInterface("input", dii->deio);
    }

    // remove outputs
    for(PortInstance *dii : ni->outputPortInstances){
        for(PortInstGate *deio_other : dii->deio->connectionsTo){
            disconnectedNodeInterface("input", deio_other);
        }
        disconnectedNodeInterface("output", dii->deio);
    }
}

void GraphWidget::connectedNodeInterface(QString posInNode, PortInstGate *deio){
    if(posInNode == "input"){
        deio->parentNode->inputConnected( deio->parentNode->parentNode->inputs.indexOf(deio->input_parentPort)+1 );
    }else if(posInNode == "output"){
        deio->parentNode->outputConnected( deio->parentNode->parentNode->outputs.indexOf(deio->output_parentPort)+1 );
    }
}

void GraphWidget::disconnectedNodeInterface(QString posInNode, PortInstGate *deio){
    if(posInNode == "input"){
        deio->parentNode->inputDisconnected( deio->parentNode->parentNode->inputs.indexOf(deio->input_parentPort)+1 );
    }else if(posInNode == "output"){
        deio->parentNode->outputDisconnected( deio->parentNode->parentNode->outputs.indexOf(deio->output_parentPort)+1 );
    }
}


void GraphWidget::tryToConnectNodes(PortInstGate *p, NodeInstance *c){
    PortInstGate *childInstGate = nullptr;

    if(p->positionInNode == "input"){
        for(PortInstance *pi : c->outputPortInstances){
            if(pi->deio->getExecType() == p->getExecType()){
                childInstGate = pi->deio;
                break;
            }
        }
    }else{ // p->positionInNode == "output"
        for(PortInstance *pi : c->inputPortInstances){
            if(pi->deio->getExecType() == p->getExecType()){
                childInstGate = pi->deio;
                break;
            }
        }
    }

    if(childInstGate == nullptr)
        return;

    connectNodes(p, childInstGate);
}

bool GraphWidget::canConnectDataInterfaces(PortInstGate *parentInterface, PortInstGate *childInterface){
    // the main problem is, that every outputInterface of the input node of a functionNode points to the inputs of its parentFunction
    // and the inputPortInstances of the outputNode point to the outputs of the parentFunction
    // ->> so I can not just ask, whether one of the interfaces is an output and one is an input,
    //     first I have to make sure, that I handle the outputDataInterfaces of an inputNode as outputs and not as inputs (same with the output node)


    //QString parentInterfacePosition = "";
    //QString childInterfacePosition = "";

    /*
    // if one of the nodes is a input or a output node
    if( ((parentInterface->parentNode->parentNode->nodeType == "input node") || (parentInterface->parentNode->parentNode->nodeType == "output node")) ||
         ((childInterface->parentNode->parentNode->nodeType == "input node") || (childInterface->parentNode->parentNode->nodeType == "output node"))   ){
        if((parentInterface->parentNode->parentNode->nodeType == "input node"))
            parentInterfacePosition = "output";
        else if((parentInterface->parentNode->parentNode->nodeType == "output node"))
            parentInterfacePosition = "input";
        else
            parentInterfacePosition = parentInterface->parentInterface->posInNode;

        if((childInterface->parentNode->parentNode->nodeType == "input node"))
            childInterfacePosition = "output";
        else if((childInterface->parentNode->parentNode->nodeType == "output node"))
            childInterfacePosition = "input";
        else
            childInterfacePosition = childInterface->parentInterface->posInNode;

    // if no one of the nodes is a input or a output node, just use the normal stored positionsInNode
    }*/

    QString parentInterface_pos = "";
    QString parentInterface_type = "";
    if(parentInterface->positionInNode == "input"){
        parentInterface_pos = "input";
        parentInterface_type = parentInterface->input_parentPort->type;
    }else if(parentInterface->positionInNode == "output"){
        parentInterface_pos = "output";
        parentInterface_type = parentInterface->output_parentPort->type;
    }

    QString childInterface_pos = "";
    QString childInterface_type = "";
    if(childInterface->positionInNode == "input"){
        childInterface_pos = "input";
        childInterface_type = childInterface->input_parentPort->type;
    }else if(childInterface->positionInNode == "output"){
        childInterface_pos = "output";
        childInterface_type = childInterface->output_parentPort->type;
    }

    // only if we do not try to connect an input with an input or an output with an output
    if( (parentInterface_pos != childInterface_pos) &&
        (parentInterface_type == childInterface_type)   ){
        return true;
    }else{
        return false;
    }
}

void GraphWidget::connectNodes(PortInstGate *parentDeio, PortInstGate *childDeio){
    //DataInterface *parentInterface = parentDeio->parentInterface;
    //DataInterface *childInterface = childDeio->parentInterface;

    if(canConnectDataInterfaces(parentDeio, childDeio)){
        if(childDeio->connectionsTo.indexOf(parentDeio) == -1){

            if(parentDeio->positionInNode == "output" && childDeio->positionInNode == "input"){
                OutputPort *parentInterface = parentDeio->output_parentPort;

                // if the parentInterface is exec, remove all it's current connections
                if( parentInterface->type == "exec"){
                    // I could also do this by using NodeInstance::removeAllConnectionsOfIn/Output, but the code would be a bit ugly here
                    for(PortInstGate *deio : parentDeio->connectionsTo){
                        deio->connectionsTo.removeOne(parentDeio);
                        disconnectedNodeInterface(deio->positionInNode, deio);
                    }
                    parentDeio->connectionsTo.clear();
                }

                // if the childDeio already has connections, remove them all (because, here the childDeio is an input and inputs always can hold only one connection)
                for(PortInstGate *deio : childDeio->connectionsTo){
                    deio->connectionsTo.removeOne(childDeio);
                    disconnectedNodeInterface(deio->positionInNode, deio);
                }
                childDeio->connectionsTo.clear();

            }else if(parentDeio->positionInNode == "input" && childDeio->positionInNode == "output"){
                OutputPort *childInterface = childDeio->output_parentPort;

                // if the childInterface is exec, remove all it's connections
                if( childInterface->type == "exec"){
                    for(PortInstGate *deio : childDeio->connectionsTo){
                        deio->connectionsTo.removeOne(childDeio);
                        disconnectedNodeInterface(deio->positionInNode, deio);
                    }
                    childDeio->connectionsTo.clear();
                }

                // if the parentDeio already has connections, remove them all (because, here the parentDeio is an input and inputs always can hold only one connection)
                for(PortInstGate *deio : parentDeio->connectionsTo){
                    deio->connectionsTo.removeOne(parentDeio);
                    disconnectedNodeInterface(deio->positionInNode, deio);
                }
                parentDeio->connectionsTo.clear();
            }

            if(childDeio->parentNode != parentDeio->parentNode){ // if I am not connecting one Node to itself
                // CONNECT
                parentDeio->connectionsTo.append(childDeio);
                childDeio->connectionsTo.append(parentDeio);

                connectedNodeInterface(parentDeio->positionInNode, parentDeio);
                connectedNodeInterface(childDeio->positionInNode, childDeio);
            }

        }else{ // remove existing connection
            parentDeio->connectionsTo.removeOne(childDeio);
            childDeio->connectionsTo.removeOne(parentDeio);

            if(parentDeio->connectionsTo.length() == 0){
                disconnectedNodeInterface(parentDeio->positionInNode, parentDeio);
            }
            if(childDeio->connectionsTo.length() == 0){
                disconnectedNodeInterface(childDeio->positionInNode, childDeio);
            }

        }
    }
    connectedDataExecInterfaces.clear();

    if(mainWindow->generateCodeSynchronous){
        mainWindow->generateCodeAndUpdateTextEdit();
    }
}

bool GraphWidget::nodeInstanceBetweenTwoPointsInScene(NodeInstance *ni, QPointF p1, QPointF p2){
    if(p1.x() < p2.x()){
        if(ni->pos().x() > p1.x() && ni->pos().x() < p2.x()){
            if(p1.y() < p2.y()){
                if(ni->pos().y() > p1.y() && ni->pos().y() < p2.y()){
                    return true;
                }
            }else{
                if(ni->pos().y() < p1.y() && ni->pos().y() > p2.y()){
                    return true;
                }
            }
        }
    }else{
        if(ni->pos().x() < p1.x() && ni->pos().x() > p2.x()){
            if(p1.y() < p2.y()){
                if(ni->pos().y() > p1.y() && ni->pos().y() < p2.y()){
                    return true;
                }
            }else{
                if(ni->pos().y() < p1.y() && ni->pos().y() > p2.y()){
                    return true;
                }
            }
        }
    }
    return false;
}

void GraphWidget::mousePressEvent(QMouseEvent *event){
    //qDebug() << "mousepressevent";
    mousePressed = true;
    mouseMovedWhilePressing = false;
    mousePressPosition = mapToScene(event->pos());
    nodePlacePos = mousePressPosition;

    if(event->button() == Qt::LeftButton){
        leftMousePressed = true;
        if(!itemAt(event->pos())){
            selectingMultipleNodes = true;
//            mousePressPosition = mapToScene(event->pos());
//            mainWindow->getNewNodeFromSelectionDialog();
        }else if(touchMode != "scroll"){
            QGraphicsItem *itemAtPos = itemAt(event->pos());
            PortInstGate *deioObject = dynamic_cast<PortInstGate*>(itemAtPos);
            NodeInstance *niObject = dynamic_cast<NodeInstance*>(itemAtPos);
            if(deioObject){ // if the item is a PortInstGate object
                connectionDragging = true;
                connectedDataExecInterfaces.clear();
                connectedDataExecInterfaces.append(static_cast<PortInstGate*>(itemAt(event->pos())));
            }else if(niObject){
                draggingNode = true;
                if(selectedNodeInstances.indexOf(niObject) == -1){ // if the nodeInstance is not already in the selectedNodeInstances
                    setSelectedNodeInstance(niObject);
                }
            }
        }
    }else if(event->button() == Qt::RightButton){
        rightMousePressed = true;
    }

    if(rightMousePressed || (leftMousePressed && touchModeActive && touchMode == "scroll" && !itemAt(event->pos()))){
        _panStartX = event->x();
        _panStartY = event->y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    QGraphicsView::mousePressEvent(event);

}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event){ // OK

    if (event->button() == Qt::RightButton){
        rightMousePressed = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }else if(event->button() == Qt::LeftButton){
        if(itemAt(event->pos())){
            QGraphicsItem *itemAtPos = itemAt(event->pos());
            PortInstGate *pigObject = dynamic_cast<PortInstGate*>(itemAtPos);
            NodeInstance *niObject = dynamic_cast<NodeInstance*>(itemAtPos);

            if(connectionDragging){
                if(pigObject){ // if the item is a PortInstGate object
                    if(static_cast<PortInstGate*>(itemAt(event->pos())) != connectedDataExecInterfaces.first()){
                        connectedDataExecInterfaces.append(static_cast<PortInstGate*>(itemAt(event->pos())));
                        connectNodes(connectedDataExecInterfaces.first(), connectedDataExecInterfaces.last());
                    }
                }
                connectionDragging = false;
                connectedDataExecInterfaces.clear();
            }else if(niObject && !mouseMovedWhilePressing){
                setSelectedNodeInstance(niObject);
            }
        }else if(!itemAt(event->pos())){
            if(currentlySelectedNodes.length() == 0){
                clearSelectedNodeInstances();
                setSelectedNodeInstance(nullptr);
            }
            if(!mouseMovedWhilePressing){
//                clearSelectedNodeInstances();
//                setSelectedNodeInstance(nullptr);
                mainWindow->getNewNodeFromSelectionDialog();
            }else if(connectionDragging){
                setSelectedNodeInstance(nullptr);
                clearSelectedNodeInstances();
                setSelectedNodeInstance(nullptr);
                nodePlacePos =  mapToScene(event->pos());
                mainWindow->getNewNodeFromSelectionDialog();
            }
            connectionDragging = false;
        }
        leftMousePressed = false;
        //qDebug() << "leftmousepressed is now false";
    }

    selectingMultipleNodes = false;
    currentlySelectedNodes.clear();
    mousePressed = false;
    mouseMovedWhilePressing = false;
    draggingNode = false;
    update();
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event){
    //qDebug() << "mouse moving";

    if(leftMousePressed && !itemAt(event->pos()) && !draggingNode && !connectionDragging && !(touchModeActive && touchMode == "scroll")){
        if((mouseMovedWhilePressing && !selectingMultipleNodes) || zooming){
            //pass
        }else{
            selectingMultipleNodes = true;
            clearSelectedNodeInstances();
            QList<NodeInstance*> instances = mainWindow->nodeInstList2Dto1D(*allNodeInstances);
            if(parentMacroNode != nullptr){
                instances.append(parentMacroNode->inputNodeInstance);
                instances.append(parentMacroNode->outputNodeInstance);
            }
            for(NodeInstance *ni : instances){
                if(nodeInstanceBetweenTwoPointsInScene( ni, mousePressPosition, mapToScene(event->pos()) )){
                    addSelectedNodeInstance(ni);
                    currentlySelectedNodes.append(ni);
                    setSelectedNodeInstance(nullptr);
                }else{
                    //ni->setSelectedInGraphStatus(false);
                }
            }
        }
    }else if(leftMousePressed && selectedNodeInstances.length() > 1 && !selectingMultipleNodes && !connectionDragging){
        for(NodeInstance *ni : selectedNodeInstances){
            NodeInstance *niObject = dynamic_cast<NodeInstance*>(itemAt(event->pos()));
            if(niObject)
                if(ni == niObject)
                    continue;

            double mouseDistanceX = mapToScene(event->pos()).x() - lastMouseMovePos.x();
            double mouseDistanceY = mapToScene(event->pos()).y() - lastMouseMovePos.y();
            ni->setPos(QPointF(ni->pos().x() + mouseDistanceX, ni->pos().y() + mouseDistanceY));

            ni->setInpOutPositions();
        }
    }

    if (rightMousePressed || (leftMousePressed && touchModeActive && touchMode == "scroll" && !itemAt(event->pos()) && !connectionDragging))
    {
        //qDebug() << rightMousePressed;
        //qDebug() << (leftMousePressed && touchModeActive && touchMode == "scroll" && !itemAt(event->pos()) && !connectionDragging);
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
    }
    //qDebug() << "mousePressed: " << mousePressed;
    if(mousePressed){
        mouseMovedWhilePressing = true;
    }
    mousePosition = mapToScene(event->pos());
    lastMouseMovePos = mapToScene(event->pos());

    QGraphicsView::mouseMoveEvent(event);

    update();
}

void GraphWidget::mouseDoubleClickEvent(QMouseEvent *event){
    if(itemAt(event->pos())){
        QList<QGraphicsItem*> allItemsAt = items(event->pos());
        if(getItemInList("NodeInstance", allItemsAt)){
            QGraphicsItem *item = getItemInList("NodeInstance", allItemsAt);
            if(dynamic_cast<NodeInstance*>(item)->parentNode->nodeType == "macro"){
                mainWindow->addGraphTab(static_cast<MacroNode*>(static_cast<NodeInstance*>(item)->parentNode));
            }
        }
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

bool GraphWidget::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        zooming = true;
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2) {
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor *= currentScaleFactor;
                currentScaleFactor = 1;
                zooming = false;
            }else{
            }
            setTransform(QTransform().scale(totalScaleFactor * currentScaleFactor,
                                            totalScaleFactor * currentScaleFactor));
        }
        return true;
    }
    default: break;
    }
    return QGraphicsView::viewportEvent(event);
}

//bool GraphWidget::event(QEvent *event){
//    qDebug() << "event";
//    if(event->type() == QEvent::Gesture){
//        return gestureEvent(static_cast<QGestureEvent*>(event));
//    }
//    return QGraphicsView::event(event);
//}

//bool GraphWidget::gestureEvent(QGestureEvent *event){
//    qDebug() << "gesture!";
//    mainWindow->setStatus("gesture");
//    if(QGesture *pinch = event->gesture(Qt::PinchGesture)){
//        pinchTriggered(static_cast<QPinchGesture*>(pinch));
//    }
//    return true;
//}

//void GraphWidget::pinchTriggered(QPinchGesture *gesture){
//    qDebug() << "pinch!";
//    mainWindow->setStatus("pinch");
//    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
//    if(changeFlags && QPinchGesture::ScaleFactorChanged){
//        qreal currentStepScaleFactor = gesture->scaleFactor();
//        setTransform(QTransform().scale(currentStepScaleFactor,
//                                        currentStepScaleFactor));
//    }
//    update();
//}

void GraphWidget::zoomSliderNowPressed(){
    zoomSliderPressed = true;
}

void GraphWidget::zoomSliderNowReleased(){
    zoomSliderPressed = false;
}

void GraphWidget::on_enableTouchModeCheckBox_stateChanged(bool b){
    touchModeActive = b;
}

void GraphWidget::on_touchModeChangedToEdit(){
    if(touchModeActive){
        touchMode = "edit";
    }
}

void GraphWidget::on_touchModeChangedToScroll(){
    if(touchModeActive){
        touchMode = "scroll";
    }
}

QGraphicsItem *GraphWidget::getItemInList(QString search, QList<QGraphicsItem *> itemList){
    for(QGraphicsItem *i : itemList){
        if(search == "NodeInstance"){
            NodeInstance *ni = dynamic_cast<NodeInstance*>(i);
            if(ni){
                return ni;
            }
        }
    }
    return nullptr;
}

void GraphWidget::clearAllContent(){
    for(NodeInstance *ni : mainWindow->nodeInstList2Dto1D(*allNodeInstances)){
        ni->deleteContent();
        scene()->removeItem(ni);
    }
    allNodeInstances->clear();
    update();
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{

//    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    painter->fillRect(rect.intersected(sceneRect), GraphWidget::bgColor);
    //painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    const int gridSize = 25;

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVector<QLineF> lines;

    if(GraphWidget::gridType == "+"){
        int s = 2;
        for (qreal x = left; x < rect.right(); x += gridSize){
            for (qreal y = top; y < rect.bottom(); y += gridSize){
                lines.append(QLineF(x, y-s, x, y+s));
                lines.append(QLineF(x-s, y, x+s, y));
            }
        }
    }else if(GraphWidget::gridType == "grid"){
        for (qreal x = left; x < rect.right(); x += gridSize){
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
            for (qreal y = top; y < rect.bottom(); y += gridSize){
                lines.append(QLineF(rect.left(), y, rect.right(), y));
            }
        }
    }else if(GraphWidget::gridType == "x"){
        int s = 2;
        for (qreal x = left; x < rect.right(); x += gridSize){
            for (qreal y = top; y < rect.bottom(); y += gridSize){
                lines.append(QLineF(x-s, y-s, x+s, y+s));
                lines.append(QLineF(x+s, y-s, x-s, y+s));
            }
        }
    }

    QPen linesPen(GraphWidget::gridColor);
    linesPen.setWidth(0.1);
    painter->setPen( linesPen );
    painter->drawLines(lines.data(), lines.size());


    // setting the positions of the proxys
    // BUG here?:
    zoomSlider_Proxy->setPos(mapToScene( 0, viewport()->height() - (zoomSlider->height()*1.2)));

    changeTouchModeToEdit_Proxy->setPos(mapToScene( viewport()->width() - changeTouchModeToEdit_Button->width()*1.5*changeTouchModeToEdit_Proxy->scale(),
                                                    viewport()->height() - changeTouchModeToEdit_Button->height()*changeTouchModeToEdit_Proxy->scale()));
    changeTouchModeToScroll_Proxy->setPos(mapToScene( mapFromScene(changeTouchModeToEdit_Proxy->pos()).x() - changeTouchModeToScroll_Button->width()*1.5*changeTouchModeToScroll_Proxy->scale(),
                                                    viewport()->height() - changeTouchModeToScroll_Button->height()*changeTouchModeToScroll_Proxy->scale()));
    enableTouchMode_Proxy->setPos(mapToScene( mapFromScene(changeTouchModeToScroll_Proxy->pos()).x() - enableTouchMode_CheckBox->width()*1.5*enableTouchMode_Proxy->scale(),
                                              viewport()->height() - enableTouchMode_CheckBox->height()*enableTouchMode_Proxy->scale()));
}

QPainterPath GraphWidget::drawConnection(PortInstGate *parentDataExecInputOutput, PortInstGate *childDataExecInputOutput){

    QPointF pPos = parentDataExecInputOutput->pos(); //+ parentDataExecInputOutput->parentInterface->localShapePos;
    QPointF cPos = childDataExecInputOutput->pos(); //+ childDataExecInputOutput->parentInterface->localShapePos;

    return drawConnection(pPos, cPos);

//    QPainterPath connectionPath;
////    connectionPath.moveTo(QPointF(pPos.x() + parentDataExecInputOutput->width/2.0, pPos.y()));
//    connectionPath.moveTo(pPos);

//    double distanceX = qAbs(pPos.x()) - qAbs(cPos.x());
//    double distanceY = qAbs(pPos.y()) - qAbs(cPos.y());
//    if( ((pPos.x() < cPos.x() - 30.0)  ||  qSqrt( (distanceX*distanceX) + (distanceY*distanceY) ) < 40.0) && (pPos.x() < cPos.x()) ){

//        connectionPath.cubicTo(QPointF( pPos.x() + (( cPos.x() - pPos.x() )/2), pPos.y() ),
//                               QPointF( pPos.x() + (( cPos.x() - pPos.x() )/2), cPos.y() ),
//                               cPos);
//    }else{
//        connectionPath.cubicTo(QPointF( pPos.x() + 100 + (pPos.x() - cPos.x())/3, pPos.y() ),
//                               QPointF( cPos.x() - 100 - (pPos.x() - cPos.x())/3, cPos.y() ),
//                               cPos);
//    }
//    return connectionPath;
}

QPainterPath GraphWidget::drawConnection(QPointF p1, QPointF p2){

    QPainterPath connectionPath;

    connectionPath.moveTo(p1);

    double distanceX = qAbs(p1.x()) - qAbs(p2.x());
    double distanceY = qAbs(p1.y()) - qAbs(p2.y());
    if( ((p1.x() < p2.x() - 30.0)  ||  qSqrt( (distanceX*distanceX) + (distanceY*distanceY) ) < 40.0) && (p1.x() < p2.x()) ){

        connectionPath.cubicTo(QPointF( p1.x() + (( p2.x() - p1.x() )/2), p1.y() ),
                               QPointF( p1.x() + (( p2.x() - p1.x() )/2), p2.y() ),
                               p2);
    }else{
        connectionPath.cubicTo(QPointF( p1.x() + 100 + (p1.x() - p2.x())/3, p1.y() ),
                               QPointF( p2.x() - 100 - (p1.x() - p2.x())/3, p2.y() ),
                               p2);
    }
    return connectionPath;
}

void GraphWidget::drawForeground(QPainter *painter, const QRectF &rect){
    // zoom
    if(zoomSliderPressed){
        double sliderPos = ((double)zoomSlider->value() - ((double)zoomSlider->maximum() / 2.0)) / ((double)zoomSlider->maximum()/ 2.0);
        zoom(QPoint(viewport()->width()/2, viewport()->height()/2), sliderPos*10.0);
    }

    QList<NodeInstance*> _allNodeInstances = mainWindow->nodeInstList2Dto1D(*allNodeInstances);
    if(parentMacroNode != nullptr){
        _allNodeInstances.append(parentMacroNode->inputNodeInstance);
        _allNodeInstances.append(parentMacroNode->outputNodeInstance);
    }


    for(NodeInstance *p : _allNodeInstances){
        for(PortInstance *dii : p->outputPortInstances){
            PortInstGate *pDeio = dii->deio;
            if(pDeio->connectionsTo.length() > 0){
                QPen pen;
                pen.setColor(pDeio->color);
                pen.setWidthF(connectionsWidth);
                pen.setCapStyle(Qt::RoundCap);
                painter->setPen(pen);
                for(PortInstGate *cDeio : pDeio->connectionsTo){
                    painter->drawPath(drawConnection(pDeio, cDeio));
                }
            }
        }
    }

//    if(connectionDragging){
//        painter->setPen(QPen(QColor("white")));
//        painter->drawLine(connectedDataExecInterfaces.first()->pos(), mousePosition);
//    }
    if(connectionDragging){
        QPen pen(QColor(qRgb(145, 187, 255)));
        pen.setCapStyle(Qt::RoundCap);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        if(connectedDataExecInterfaces.first()->positionInNode == "output")
            painter->drawPath( drawConnection(connectedDataExecInterfaces.first()->pos(), mousePosition) );
        else if(connectedDataExecInterfaces.first()->positionInNode == "input")
            painter->drawPath( drawConnection(mousePosition, connectedDataExecInterfaces.first()->pos()) );
    }

    if(selectingMultipleNodes){
        QPen pen(QColor(0, 50, 0, 100));
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        QBrush brush(QColor(255, 255, 50, 50));
        painter->setBrush(brush);
        int xLeft = mousePressPosition.x() < mousePosition.x() ? mousePressPosition.x() : mousePosition.x();
        int xRight = mousePressPosition.x() > mousePosition.x() ? mousePressPosition.x() : mousePosition.x();
        int yTop = mousePressPosition.y() < mousePosition.y() ? mousePressPosition.y() : mousePosition.y();
        int yBottom = mousePressPosition.y() > mousePosition.y() ? mousePressPosition.y() : mousePosition.y();
        painter->drawRect(QRect(QPoint(xLeft, yTop), QPoint(xRight, yBottom)));
    }
}

void GraphWidget::wheelEvent( QWheelEvent *event ){ // OK

        if(event->angleDelta().x() == 0){

                zoom(event->pos(), event->angleDelta().y());

                event->accept();
        }
}

void GraphWidget::zoom(QPoint pos, double angle){

    QPointF posf = this->mapToScene(pos);

    double by;
    double velocity = 2.0*(1/absoluteScale)+0.5;
    if(velocity > 3){ velocity = 3;}

    QString direction;

    if      (angle > 0) { by = 1 + ( angle / 360 * 0.1 * velocity); direction = "in";} // in
    else if (angle < 0) { by = 1 - (-angle / 360 * 0.1 * velocity); direction = "out";} // out
    else                { by = 1; }

    int _width = this->mapFromScene( this->sceneRect() ).boundingRect().width();
    int _height = this->mapFromScene( this->sceneRect() ).boundingRect().height();

    if(direction == "in"){
        if( (absoluteScale*by) < 3){
            this->scale(by, by);
            absoluteScale *= by;
        }
    }else if(direction == "out"){
        if( ((_width*by) >= this->viewport()->size().width()) && ((_height*by) >= this->viewport()->size().height()) ){
            this->scale(by, by);
            absoluteScale *= by;
        }
    }


    double w = this->viewport()->width();
    double h = this->viewport()->height();

    double wf = this->mapToScene(QPoint(w-1, 0)).x()
                    - this->mapToScene(QPoint(0,0)).x();
    double hf = this->mapToScene(QPoint(0, h-1)).y()
                    - this->mapToScene(QPoint(0,0)).y();

    double lf = posf.x() - pos.x() * wf / w;
    double tf = posf.y() - pos.y() * hf / h;

    /* try to set viewport properly */
    this->ensureVisible(lf, tf, wf, hf, 0, 0);

    QPointF newPos = this->mapToScene(pos);

    /* readjust according to the still remaining offset/drift
     * I don't know how to do this any other way */
    QRectF visibleTargetRect = QRectF(QPointF(lf, tf) - newPos + posf,
                                      QSizeF(wf, hf));

    this->ensureVisible(visibleTargetRect, 0, 0);
}

void GraphWidget::ensureNodesVisibility(){
    /// doesn't work, don't ask me why. it just does not change zoom, so ensureVisible() seems not to work right
    // ensure visibility
    QList<QPointF> points;
    for(NodeInstance *ni : mainWindow->nodeInstList2Dto1D(*allNodeInstances)){
        points.append(QPointF(ni->pos().x() - (ni->width/2.0)*ni->scale, ni->pos().y() - (ni->height/2.0)*ni->scale));
        points.append(QPointF(ni->pos().x() + (ni->width/2.0)*ni->scale, ni->pos().y() - (ni->height/2.0)*ni->scale));
        points.append(QPointF(ni->pos().x() - (ni->width/2.0)*ni->scale, ni->pos().y() + (ni->height/2.0)*ni->scale));
        points.append(QPointF(ni->pos().x() + (ni->width/2.0)*ni->scale, ni->pos().y() + (ni->height/2.0)*ni->scale));
    }
    qreal xmin = maximumWidth();
    qreal xmax = 0;
    qreal ymin = maximumHeight();
    qreal ymax = 0;
    for(QPointF p : points){
        xmin = p.x() < xmin ? p.x() : xmin;
        xmax = p.x() > xmax ? p.x() : xmax;
        ymin = p.y() < ymin ? p.y() : ymin;
        ymax = p.y() > ymax ? p.y() : ymax;
    }
    ensureVisible(xmin, ymin, xmax-xmin, ymax-ymin, 10, 10);
}

void GraphWidget::applyStylesheetToContent(QString ss){
    graphContentStyleSheet = ss;
    for(NodeInstance *ni : mainWindow->nodeInstList2Dto1D(*allNodeInstances)){
        ni->applyStylesheetToContent(ss);
        ni->update();
    }
}

void GraphWidget::newMacroCreated(QList<NodeInstance*> containedNodeInstances, MacroNode *macroNode){
    // get average pos of containedNodeInstances
    QPointF pos = QPointF(0.0, 0.0);

    // remove all connections between contained nodes and others
    for(NodeInstance *ni : containedNodeInstances){ // iterate over all contained NodeInstances
        // inputs
        for(PortInstance *i : ni->inputPortInstances){ // iterate over all inputs
            for(int j=0; j<i->deio->connectionsTo.length(); j++){ // iterate over all connections
                PortInstGate *otherGate = i->deio->connectionsTo.at(j);
                if(containedNodeInstances.indexOf(otherGate->parentNode) == -1){ // if the NodeInstance is outside
                    // remove connection
                    otherGate->connectionsTo.removeOne(i->deio);
                    i->deio->connectionsTo.removeOne(otherGate);
                    j--;
                }
            }
        }

        // outputs
        for(PortInstance *o : ni->outputPortInstances){
            for(int j=0; j<o->deio->connectionsTo.length(); j++){
                PortInstGate *otherGate = o->deio->connectionsTo.at(j);
                if(containedNodeInstances.indexOf(otherGate->parentNode) == -1){
                    // remove connections
                    otherGate->connectionsTo.removeOne(o->deio);
                    o->deio->connectionsTo.removeOne(otherGate);
                    j--;
                }
            }
        }
    }

    removeInstances(containedNodeInstances);
//    for(NodeInstance *ni : containedNodeInstances){
//        ni->graph = nullptr;
//        for(int i=0; i<allNodeInstances.length(); i++){
//            QList<NodeInstance*> l = allNodeInstances.at(i);
//            if(l.indexOf(ni) != -1){
//                l.removeOne(ni);
//                if(l.length() == 0){
//                    allNodeInstances.removeAt(i);
//                    i--;
//                }
//                removeInstance(ni);
//            }
//        }
//    }
    // create new MacroNodeInstance and place it at the computed average pos
    placeExistingNode(macroNode, pos);
}


// content design

void GraphWidget::setDesignStyle(QString s){
    GraphWidget::designStyle = s;
    for(NodeInstance *ni : mainWindow->nodeInstList2Dto1D(*allNodeInstances)){
        ni->setDesignStyle(s);
    }
}












// EVENT FILTER

bool Graph_KeyPressEventFilter::eventFilter(QObject *watched, QEvent *event){
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(watched, event);

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
//    switch(keyEvent->key()) {
//    case Qt::Key_Space: { qDebug() << "Space"; break; }
//    case Qt::Key_Left: { qDebug() << "Left"; break; }
//    case Qt::Key_Right: { qDebug() << "Right"; break; }
//    default: { qDebug() << "Unhandled"; break; }
//    }
    if(keyEvent->key() == Qt::Key_C && keyEvent->modifiers().testFlag(Qt::ControlModifier)){
        if(graph->selectedNodeInstances.length() > 0){
            graph->copyNodeInstances(graph->selectedNodeInstances);
        }else if(graph->selectedNodeInstances.length() == 0 && graph->selectedNodeInstance != nullptr){
            graph->copyNodeInstance(graph->selectedNodeInstance);
        }
    }else if(keyEvent->key() == Qt::Key_V && keyEvent->modifiers().testFlag(Qt::ControlModifier)){
        if(graph->copiedNodeInstances.length() > 0){
            graph->pasteCopiedNodeInstances();
        }else if(graph->copiedNodeInstance != nullptr){
            graph->pasteCopiedNodeInstance();
        }
    }else if(keyEvent->key() == Qt::Key_Delete){
        if(graph->selectedNodeInstances.length() > 0){
            graph->deleteInstances(graph->selectedNodeInstances);
        }else if(graph->selectedNodeInstance != nullptr){
            graph->deleteInstance(graph->selectedNodeInstance);
        }
    }else{
        return QObject::eventFilter(watched, event);
    }
    return true;
}
