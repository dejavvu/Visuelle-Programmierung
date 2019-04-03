#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "PortInstGate.h"
#include "DataExecIOLabel.h"
class PortInstGate;
class DataExecIOLabel;

#include <QColor>
#include <QString>
#include <QLabel>
#include <QGraphicsItem>

// this is a model struct, not for the view
struct Port {
    int width;
    int height = 10;
    QPointF localShapePos;
    QPointF localLabelPos;
    QString displayName;
    QColor color;
    QString type; // "data" or "exec"
};

struct InputPort : Port{
    QString wdgtType;
};

struct OutputPort : Port{
    QString internalName;
    /* QString code; */ /// NOPE!!! An output does not have a code.
                        /// if it is a dataOutput, the metaCode is
                        /// stored in internalName. The "code"
                        /// which I used in the past is old and
                        /// won't be used anymore
};

struct PortInstance{
    PortInstGate *deio = nullptr;
    DataExecIOLabel *label = nullptr;
    QWidget *wdgt = nullptr;
    QGraphicsProxyWidget *proxy = nullptr;
};

struct MacroOutputPortInstance : PortInstance{ // only gets used in a MacroInputNodeInstance
    InputPort *parentInputPortOfMacroNode;
};

struct MacroInputPortInstance : PortInstance{ // only gets used in a MacroOutputNodeInstance
    OutputPort *parentOutputPortOfMacroNode;
};

#endif // DATAINTERFACE_H
