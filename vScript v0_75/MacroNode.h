#ifndef MACRONODE_H
#define MACRONODE_H

#include "node.h"

class MacroNodeInstance;
class MacroInputNode;
class MacroInputNodeInstance;
class MacroOutputNode;
class MacroOutputNodeInstance;

class MacroNode : public Node
{
public:
    MacroNode(int numberInput, MainWindow *mainWindowInput, QList<QList<NodeInstance *> > containedNodeInstancesInput, bool createContent = true);
    void setContent(QList<QList<NodeInstance*>> containedNodeInstancesInput);
    void addNewInput(QString _displayName = "", int _width = 10, int _height = 10, QString _type = "data", QColor _color = nullptr, QString _wdgtType = "");
    void addNewOutput(QString _displayName = "", QString _internalName = "", int _width = 10, int _height = 10, QString _type = "data", QColor _color = nullptr);


    QList<QList<NodeInstance*>> containedNodeInstances;

    MacroNodeInstance *currentInstanceForGettingCode; // this is unfortunately neccessairy, otherwise, the inputNodeInstance is not able to cast out of a macroNodeINSTANCE for getting data
    MacroInputNode *inputNode;
    MacroInputNodeInstance *inputNodeInstance;
    MacroOutputNode *outputNode;
    MacroOutputNodeInstance *outputNodeInstance;


public slots:
    void inputNameChanged(int index, QString newName);
    void inputTypeChanged(int index, QString newType);
    void inputDeleted(int index);
    //void inputAdded();

    void outputNameChanged(int index, QString newName);
    void outputTypeChanged(int index, QString newType);
    void outputDeleted(int index);
    //void outputAdded();

};

#endif // MAKRONODE_H
