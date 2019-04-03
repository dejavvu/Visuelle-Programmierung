#ifndef CODENODE_H
#define CODENODE_H

#include "node.h"

#include <QString>
#include <QFont>
#include <QFile>

class FunctionNode;
class CodeNodeDetailsForm;

class CodeNode : public Node
{
public:
    CodeNode(int numberInput = -1);
    void createOrUpdateCodeFile();
    void updateCodeFromFile();
    void setCodeToExample();
    void finishedCreation();

    QFile codeFile;
    QString metaCode;
    QString additionalNeededCode;
    QFont codeFont;
    int codeFontSize;
    //QString displayName;
    QString genre;


};

#endif // CODENODE_H
