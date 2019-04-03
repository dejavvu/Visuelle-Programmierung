#include "MacroInputNode.h"

MacroInputNode::MacroInputNode(int numberInput, MacroNode *parentMacroNodeInput)
    : Node(QByteArray(), numberInput)
{
    nodeType = "macro input node";
    title = "input >";
    parentMacroNode = parentMacroNodeInput;

    color = QColor(15,5,191,150);
}

void MacroInputNode::updateOutputs(QList<InputPort *> ports){

}
