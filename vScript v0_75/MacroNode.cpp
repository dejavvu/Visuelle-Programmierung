#include "MacroNode.h"

#include "MacroNodeInstance.h"
#include "MacroInputNodeInstance.h"
#include "MacroOutputNodeInstance.h"
#include "MacroNodeDetailsForm.h"
#include "MacroInputNode.h"
#include "MacroOutputNode.h"
#include "mainwindow.h"

#include <QDebug>

MacroNode::MacroNode(int numberInput, MainWindow *mainWindowInput, QList<QList<NodeInstance *>> containedNodeInstancesInput, bool createContent)
    : Node(QByteArray(), numberInput)
{
    nodeType = "macro";
    currentInstanceForGettingCode = nullptr;

    title = "Macro";
    color = QColor(112, 167, 255, 100); // standard color

    containedNodeInstances = containedNodeInstancesInput;

    if(createContent){
        inputNode = new MacroInputNode(mainWindowInput->nodeCounter++, this);
        inputNodeInstance = new MacroInputNodeInstance(inputNode, this, mainWindowInput);
        inputNode->instances.append(inputNodeInstance);
        outputNode = new MacroOutputNode(mainWindowInput->nodeCounter++, this);
        outputNodeInstance = new MacroOutputNodeInstance(outputNode, this, mainWindowInput);
        outputNode->instances.append(outputNodeInstance);
    }else{
        inputNode = nullptr;
        inputNodeInstance = nullptr;
        outputNode = nullptr;
        outputNodeInstance = nullptr;
    }
    detailsWidget = new MacroNodeDetailsForm(this); //Macro_DetailsWidget(this);
}

void MacroNode::setContent(QList<QList<NodeInstance *> > containedNodeInstancesInput){
    containedNodeInstances = containedNodeInstancesInput;
}



void MacroNode::inputNameChanged(int index, QString newName){
    Node::inputNameChanged(index, newName);
    //inputNode->outputs[index]->displayName = newName;
    inputNode->outputNameChanged(index, newName);
    //inputNode->updateOutputs(inputs);
}

void MacroNode::inputTypeChanged(int index, QString newType){
    Node::inputTypeChanged(index, newType);
//    inputNode->removeAllConnectionsOfOutput(index);
//    inputNode->outputs[index]->type = newType;
    inputNode->outputTypeChanged(index, newType);
    //inputNode->updateOutputs(inputs);
}

void MacroNode::inputDeleted(int index){
//    for(NodeInstance *ni : instances){
//        ni->updateInputs();
//    }
    Node::inputDeleted(index);
    //inputNode->outputs.removeAt(index); // connections already get removed in Node::inputDeleted(index)
    inputNode->outputDeleted(index);
    //inputNode->updateOutputs(inputs);
}

//void MacroNode::inputAdded(){
//    Node::inputAdded();
//    inputNode->updateOutputs(inputs);
//}

void MacroNode::outputNameChanged(int index, QString newName){
    Node::outputNameChanged(index, newName);
    //outputNode->inputs[index]->displayName = newName;
    outputNode->inputNameChanged(index, newName);
    //outputNode->updateInputs(outputs);
}

void MacroNode::outputTypeChanged(int index, QString newType){
    Node::outputTypeChanged(index, newType);
//    outputNode->removeAllConnectionsOfInput(index);
//    outputNode->inputs[index]->type = newType;
    outputNode->inputTypeChanged(index, newType);
    //outputNode->updateInputs(outputs);
}

void MacroNode::outputDeleted(int index){
//    for(NodeInstance *ni : instances){
//        ni->updateOutputs();
//    }
    Node::outputDeleted(index);
    //outputNode->inputs.removeAt(index); // connections already get removed in Node::outputDeleted(index)
    outputNode->inputDeleted(index);
    //outputNode->updateInputs(outputs);
}

//void MacroNode::outputAdded(){
//    Node::outputAdded();
//    outputNode->updateInputs(outputs);
//}


void MacroNode::addNewInput(QString _displayName, int _width, int _height, QString _type, QColor _color, QString _wdgtType){
    Node::addNewInput(_displayName, _width, _height, _type, _color, _wdgtType);
    inputNode->addNewOutput(_displayName, "", _width, _height, _type, _color);
}

void MacroNode::addNewOutput(QString _displayName, QString _internalName, int _width, int _height, QString _type, QColor _color){
    Node::addNewOutput(_displayName, _internalName, _width, _height, _type, _color);
    outputNode->addNewInput(_displayName, _width, _height, _type, _color, "");
}
