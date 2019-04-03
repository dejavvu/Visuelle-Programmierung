#ifndef MACROINPUTNODE_H
#define MACROINPUTNODE_H

#include "node.h"

class MacroNode;

class MacroInputNode : public Node
{
public:
    MacroInputNode(int numberInput, MacroNode *parentMacroNodeInput);

    void updateOutputs(QList<InputPort*> ports);

    MacroNode *parentMacroNode;
};

#endif // MACROINPUTNODE_H
