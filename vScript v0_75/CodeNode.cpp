#include "CodeNode.h"

#include "CodeNodeDetailsForm.h"

#include <QFileInfo>
#include <QDebug>

CodeNode::CodeNode(int numberInput)
    : Node(QByteArray(), numberInput)
{
    nodeType = "code";
    metaCode = "";
    additionalNeededCode = "";
    description = "";
    title = "Code";
    package = "";
    genre = "";

    codeFontSize = 10;
    codeFont = QFont("MS Reference Sans Serif");
    codeFont.setPointSize(codeFontSize);
    codeFont.setStyleHint(QFont::Monospace);
    codeFont.setFixedPitch(true);

    color = QColor(112, 167, 255, 100); // standard color

    detailsWidget = new CodeNodeDetailsForm(this);

    setCodeToExample();
}

void CodeNode::createOrUpdateCodeFile(){
    codeFile.remove();


    QString currentPath = QFileInfo(".").absolutePath();
    QString dir = currentPath + "/codesDir/";
    codeFile.setFileName(dir + title + ".txt");
    codeFile.open(QFile::WriteOnly);
    QByteArray codeByteArray = metaCode.toUtf8();
    codeFile.write(codeByteArray);
    codeFile.close();

}

void CodeNode::updateCodeFromFile(){
    codeFile.open(QFile::ReadOnly);
    metaCode = codeFile.readAll();
    codeFile.close();

    static_cast<CodeNodeDetailsForm*>(detailsWidget)->setCode(metaCode);
}

void CodeNode::setCodeToExample(){
    QString currentPath = QFileInfo(".").absolutePath();
    QString dir = currentPath + "/codesDir/";
    QFile file;
    file.setFileName(dir + "test_codes.txt");
    file.open(QFile::ReadOnly);
    metaCode = file.readAll();
    file.close();

    static_cast<CodeNodeDetailsForm*>(detailsWidget)->setCode(metaCode);
}

void CodeNode::finishedCreation(){
    static_cast<CodeNodeDetailsForm*>(detailsWidget)->setUpEverything();
}
