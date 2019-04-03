#include "MacroNodeDetailsForm.h"
#include "ui_MacroNodeDetailsForm.h"

#include "MacroNode.h"
#include "myqts.h"
#include "mainwindow.h"

#include <QComboBox>
#include <QDebug>
#include <QColorDialog>

MacroNodeDetailsForm::MacroNodeDetailsForm(MacroNode *parentNodeInput, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MacroNodeDetailsForm)
{
    ui->setupUi(this);
    parentNode = parentNodeInput;
    standardWarningMessage = "Don't forget to open a new graph tab when with this macro before generating code if there currently is no opened one. "
                             "Otherwise the application will crash, because the input and output nodeinstances could not update";
}

MacroNodeDetailsForm::~MacroNodeDetailsForm()
{
    delete ui;
}

void MacroNodeDetailsForm::showMessageInMainWindow(QString m){
    auto* pMainWnd = ltsGet<MainWindow>("mainWindow");
    if(pMainWnd)
        static_cast<MainWindow*>(pMainWnd)->setStatus(m);
}

void MacroNodeDetailsForm::setUpEverything(){

    updateInputsAndOutputs();

    ui->title_LineEdit->setText(parentNode->title);
    ui->description_PlainTextEdit->setPlainText(parentNode->description);
}

void MacroNodeDetailsForm::updateInputsAndOutputs(){



    inputs.clear();
    //delete inputsWidget;
    inputsWidget = new QWidget();
    inputsWidget->setLayout(new QVBoxLayout);
    inputsWidget->layout()->setAlignment(Qt::AlignTop);



    //  INPUTS

    for(InputPort *di : parentNode->inputs){
        QWidget *dataInterfaceWidget = new QWidget;
        QVBoxLayout *inputLayout = new QVBoxLayout();
        dataInterfaceWidget->setLayout(inputLayout);
        dataInterfaceWidget->setObjectName("input" + QString::number(parentNode->inputs.indexOf(di)) + "_dataInterfaceWidget");
        inputs.insert(dataInterfaceWidget, di);
        // add horizontal line
        QFrame *hLine = new QFrame();
        hLine->setFrameShape(QFrame::HLine);
        inputLayout->addWidget(hLine);

        // NAME LINE EDIT
        QLineEdit *nameLineEdit = new QLineEdit();
        nameLineEdit->objectName() = "test";
        nameLineEdit->setText(di->displayName);
        //dataInterfaceWidget->layout()->addWidget(nameLineEdit);
        inputLayout->addWidget(nameLineEdit);
        connect(nameLineEdit, SIGNAL(textChanged(QString)),
                this, SLOT(renameInput(QString)));

        // TYPE-COMBOBOX
        QHBoxLayout *typeAndDelButtonLayout = new QHBoxLayout();
        QComboBox *typeComboBox = new QComboBox();
        typeComboBox->addItem("exec");
        typeComboBox->addItem("data");
        typeComboBox->setCurrentText(di->type);
        //dataInterfaceWidget->layout()->addWidget(typeComboBox);
        typeAndDelButtonLayout->addWidget(typeComboBox);
        connect(typeComboBox, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(changeInputType(QString)));

        // DELETE-BUTTON
        QPushButton *delButton = new QPushButton();
        delButton->setText("del");
        //dataInterfaceWidget->layout()->addWidget(delButton);
        typeAndDelButtonLayout->addWidget(delButton);
        connect(delButton, SIGNAL(pressed()),
                this, SLOT(deleteInput()));
        inputLayout->addLayout(typeAndDelButtonLayout);

        // add horizontal line
        QFrame *hLine2 = new QFrame();
        hLine2->setFrameShape(QFrame::HLine);
        inputLayout->addWidget(hLine2);

        inputsWidget->layout()->addWidget(dataInterfaceWidget);
    }
    QLayoutItem  *item;
    while((item = ui->inputs_verticalLayout->takeAt(0)) != NULL){
        delete item->widget();
        delete item;
    }
    ui->inputs_verticalLayout->addWidget(inputsWidget);



    //  OUTPUTS
    outputs.clear();
    //delete outputsWidget;
    outputsWidget = new QWidget();
    outputsWidget->setLayout(new QVBoxLayout);
    outputsWidget->layout()->setAlignment(Qt::AlignTop);

    // add outputs
    for(OutputPort *di : parentNode->outputs){
        QWidget *dataInterfaceWidget = new QWidget;
        QVBoxLayout *outputLayout = new QVBoxLayout();
        dataInterfaceWidget->setLayout(outputLayout);
        dataInterfaceWidget->setObjectName("output" + QString::number(parentNode->outputs.indexOf(di)) + "_dataInterfaceWidget");
        outputs.insert(dataInterfaceWidget, di);
        // add horizontal line
        QFrame *hLine = new QFrame();
        hLine->setFrameShape(QFrame::HLine);
        outputLayout->addWidget(hLine);

        // NAME LINE EDIT
        QLineEdit *nameLineEdit = new QLineEdit();
        nameLineEdit->setText(di->displayName);
        outputLayout->addWidget(nameLineEdit);
        connect(nameLineEdit, SIGNAL(textChanged(QString)),
                this, SLOT(renameOutput(QString)));

        // TYPE-COMBOBOX
        QHBoxLayout *typeAndDelButtonLayout = new QHBoxLayout();
        QComboBox *typeComboBox = new QComboBox();
        typeComboBox->addItem("exec");
        typeComboBox->addItem("data");
        typeComboBox->setCurrentText(di->type);
        //dataInterfaceWidget->layout()->addWidget(typeComboBox);
        typeAndDelButtonLayout->addWidget(typeComboBox);
        connect(typeComboBox, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(changeOutputType(QString)));

        // DELETE-BUTTON
        QPushButton *delButton = new QPushButton();
        delButton->setText("del");
        //dataInterfaceWidget->layout()->addWidget(delButton);
        typeAndDelButtonLayout->addWidget(delButton);
        connect(delButton, SIGNAL(pressed()),
                this, SLOT(deleteOutput()));
        outputLayout->addLayout(typeAndDelButtonLayout);

        // add horizontal line
        QFrame *hLine2 = new QFrame();
        hLine2->setFrameShape(QFrame::HLine);
        outputLayout->addWidget(hLine2);

        outputsWidget->layout()->addWidget(dataInterfaceWidget);
    }
    while((item = ui->outputs_verticalLayout->takeAt(0)) != NULL){
        delete item->widget();
        delete item;
    }
    ui->outputs_verticalLayout->addWidget(outputsWidget);




}

int MacroNodeDetailsForm::getInputIndexFromSenderName(QString name){
    QString senderName = sender()->parent()->objectName();
     // 0 1 2 3 4   5
     // i n p u t number ...
    QChar charNumber = senderName[5];
    return charNumber.unicode() - 48;
}

int MacroNodeDetailsForm::getOutputIndexFromSenderName(QString name){
    QString senderName = sender()->parent()->objectName();
     // 0 1 2 3 4 5    6
     // o u t p u t number ...
    QChar charNumber = senderName[6];
    return charNumber.unicode() - 48;
}

void MacroNodeDetailsForm::renameInput(QString newName){
    emit parentNode->inputNameChanged(getInputIndexFromSenderName(sender()->parent()->objectName()), newName);
}

void MacroNodeDetailsForm::renameOutput(QString newName){
    emit parentNode->outputNameChanged(getOutputIndexFromSenderName(sender()->parent()->objectName()), newName);
}

void MacroNodeDetailsForm::changeInputType(QString newType){
    showMessageInMainWindow(standardWarningMessage);
    emit parentNode->inputTypeChanged(getInputIndexFromSenderName(sender()->parent()->objectName()), newType);
}

void MacroNodeDetailsForm::changeOutputType(QString newType){
    showMessageInMainWindow(standardWarningMessage);
    emit parentNode->outputTypeChanged(getOutputIndexFromSenderName(sender()->parent()->objectName()), newType);
}

void MacroNodeDetailsForm::deleteInput(){
    showMessageInMainWindow(standardWarningMessage);
    emit parentNode->inputDeleted(getInputIndexFromSenderName(sender()->parent()->objectName()));
    updateInputsAndOutputs();
}

void MacroNodeDetailsForm::deleteOutput(){
    showMessageInMainWindow(standardWarningMessage);
    emit parentNode->outputDeleted(getOutputIndexFromSenderName(sender()->parent()->objectName()));
    updateInputsAndOutputs();
}

void MacroNodeDetailsForm::on_addNewInput_pushButton_clicked()
{
    showMessageInMainWindow(standardWarningMessage);
    parentNode->addNewInput("input", 10, 10, "data");
    updateInputsAndOutputs();
}

void MacroNodeDetailsForm::on_addNewOutput_pushButton_clicked()
{
    showMessageInMainWindow(standardWarningMessage);
    parentNode->addNewOutput("output", "", 10, 10, "exec");
    updateInputsAndOutputs();
}

void MacroNodeDetailsForm::on_showDescription_pushButton_clicked()
{

}

void MacroNodeDetailsForm::on_description_PlainTextEdit_textChanged()
{
    static_cast<MacroNode*>(parentNode)->description = ui->description_PlainTextEdit->toPlainText();
    static_cast<MacroNode*>(parentNode)->updateAllInstances();
}

void MacroNodeDetailsForm::on_title_LineEdit_textChanged(const QString &arg1)
{
    static_cast<MacroNode*>(parentNode)->setTitle(arg1);
    //static_cast<MacroNode*>(parentNode)->updateAllInstances();

    auto* pMainWnd = ltsGet<MainWindow>("mainWindow");
    if(pMainWnd)
        static_cast<MainWindow*>(pMainWnd)->setUpCodeNodes();
}

void MacroNodeDetailsForm::on_changeColor_pushButton_clicked()
{
    parentNode->color = QColorDialog::getColor(parentNode->color, this);
}
