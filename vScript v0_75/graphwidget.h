#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>

#include "variable.h"

class QSlider;
class QGestureEvent;
class QPinchGesture;
class QPushButton;
class QCheckBox;

class MainWindow;
class Node;
class NodeInstance;
class FunctionNodeInstance;
class CodeNodeInstance;
class InputNodeInstance;
class InputNode;
class OutputNodeInstance;
class OutputNode;
class PortInstGate;
class DataInterface;
class ZoomSlider;
class Graph_KeyPressEventFilter;
class MacroNode;

namespace Ui {
class GraphWidget;
}

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:

    // METHODS
    explicit GraphWidget(QWidget *parent = Q_NULLPTR, MainWindow *mainWindowInput = Q_NULLPTR, MacroNode *parentMacroNodeInput = nullptr);
    ~GraphWidget();

    //void newNode(QPointF pos, QString nodeType);
    QList<NodeInstance*>& allNodeInstances_getRefListAt(int i); // useful access function for allNodeInstances
    QList<NodeInstance*> getAllInstances_includingInpOut();
    void setUpScrollBars();
    void setUpProxys();
    void setSelectedNodeInstance(NodeInstance *nodeInput);
    void addSelectedNodeInstance(NodeInstance *ni);
    void copyNodeInstance(NodeInstance *ni);
    void copyNodeInstances(QList<NodeInstance*> l);
    void pasteCopiedNodeInstance();
    void pasteCopiedNodeInstances();
    QPointF getMiddlePosOfNodeInstances(QList<NodeInstance*> instances);
    void clearSelectedNodeInstances();
    void connectNodes(PortInstGate *parentDeio, PortInstGate *childDeio);
    NodeInstance *placeExistingNode(Node *node, QPointF pos);
    void placeExistingNodeInstance(NodeInstance *instance, QPointF pos);
    void updateInstancesAndRepaint();
    void placeInputAndOutput(InputNode *inpNode, OutputNode *otpNode);
    QGraphicsItem *getItemInList(QString search, QList<QGraphicsItem*> itemList);
    void deleteInstance(NodeInstance *ni);
    void deleteInstances(QList<NodeInstance*> l);
    void removeInstance(NodeInstance *ni);
    void removeInstances(QList<NodeInstance*> l);
    void removeFromAllNodeList(NodeInstance *ni);
    void removeAllConnections(NodeInstance *ni);
    QPainterPath drawConnection(PortInstGate *parentDataExecInputOutput, PortInstGate *childDataExecInputOutput);
    QPainterPath drawConnection(QPointF p1, QPointF p2);
    void tryToConnectNodes(PortInstGate *p, NodeInstance *c);
    bool canConnectDataInterfaces(PortInstGate *parentInterface, PortInstGate *childInterface);
    void connectedNodeInterface(QString posInNode, PortInstGate *deio);
    void disconnectedNodeInterface(QString posInNode, PortInstGate *deio);
    void clearAllContent();
    bool nodeInstanceBetweenTwoPointsInScene(NodeInstance *ni, QPointF p1, QPointF p2);
    void zoom(QPoint pos, double angle);
    void ensureNodesVisibility();
    void applyStylesheetToContent(QString ss);
    void newMacroCreated(QList<NodeInstance*> containedNodeInstances, MacroNode *macroNode);

    // content design
    void setDesignStyle(QString s);

    //void paintEvent(QPaintEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
     // METHODS


    // ATTRIBUTES
    MainWindow *mainWindow;
    MacroNode *parentMacroNode;

    // touch and proxys
    QString touchMode;
    bool touchModeActive;
    QGraphicsProxyWidget *changeTouchModeToEdit_Proxy;
    QPushButton *changeTouchModeToEdit_Button;
    QGraphicsProxyWidget *changeTouchModeToScroll_Proxy;
    QPushButton *changeTouchModeToScroll_Button;
    QGraphicsProxyWidget *enableTouchMode_Proxy;
    QCheckBox *enableTouchMode_CheckBox;
    QGraphicsProxyWidget *zoomSlider_Proxy;
    ZoomSlider *zoomSlider;

    qreal totalScaleFactor;
    Graph_KeyPressEventFilter *customEventFilter;
    QList<QList<NodeInstance*>> *allNodeInstances; // the instances are only for displaying
    QList<PortInstGate*> connectedDataExecInterfaces;
    Node *selectedNode = nullptr;
    NodeInstance *selectedNodeInstance = nullptr;
    NodeInstance *copiedNodeInstance = nullptr;
    QList<NodeInstance*> selectedNodeInstances;
    QList<NodeInstance*> currentlySelectedNodes;
    QList<NodeInstance*> copiedNodeInstances;
    QString graphContentStyleSheet;
    bool rightMousePressed;
    bool leftMousePressed;
    bool mouseMovedWhilePressing;
    bool selectingMultipleNodes;
    bool draggingNode;
    int _panStartX, _panStartY;
    int m_originX;
    int m_originY;
    bool mousePressed;
    bool connectionDragging;
    bool zoomSliderPressed;
    bool zooming;
    //bool anyKeyPressed;
    //int pressedKey;
    QPointF mousePressPosition;
    QPointF nodePlacePos; // is normally mousePressPosision but not always
    QPointF mousePosition;
    QPointF lastMouseMovePos;
    bool mouseOverMe;
    double absoluteScale;
    QPointF absoluteTranslation;
//    InputNodeInstance *inputNodeInst = nullptr;
//    OutputNodeInstance *outputNodeInst = nullptr;

    // content design
    static QString designStyle;
    static QColor bgColor;
    static QColor gridColor;
    static QString gridType;
    static double connectionsWidth;

    // ATTRIBUTES

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void wheelEvent( QWheelEvent *event ) override;
//    virtual void keyPressEvent( QKeyEvent *event ) override;
//    virtual void keyReleaseEvent( QKeyEvent *event ) override;
    virtual bool viewportEvent(QEvent *event) override;
//    virtual bool event(QEvent *event) override;
//    bool gestureEvent(QGestureEvent *event);
//    void pinchTriggered(QPinchGesture *gesture);

signals:
    void selectedNodeChanged(Node *nodeInput);

public slots:
    void zoomSliderNowPressed();
    void zoomSliderNowReleased();
    void on_enableTouchModeCheckBox_stateChanged(bool b);
    void on_touchModeChangedToEdit();
    void on_touchModeChangedToScroll();

};

#endif // GRAPHWIDGET_H
















// THE EVENT FILTER

#ifndef GRAPH_KEYPRESSEVENTFILTER
#define GRAPH_KEYPRESSEVENTFILTER

class Graph_KeyPressEventFilter : public QObject{
    Q_OBJECT
public:
    Graph_KeyPressEventFilter(GraphWidget *_graph, QObject *parent = nullptr){graph = _graph;}

    GraphWidget *graph;
protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif
