#include "CodeNodeDetailsForm.h"
#include "ui_codenodedetailsform.h"

#include "CodeNode.h"
#include "datainterface.h"
#include "mainwindow.h"
#include "myqts.h"

#include <QColorDialog>
#include <QLayout>
#include <QFileInfo>
#include <QDebug>

CodeNodeDetailsForm::CodeNodeDetailsForm(CodeNode *parentNodeInput, QWidget *parent) :
    ui(new Ui::CodeNodeDetailsForm)
{
    ui->setupUi(this);
    layout()->setAlignment(Qt::AlignTop);

    parentNode = parentNodeInput;
    inputsWidget = new QWidget();
    outputsWidget = new QWidget();

    setUpEverything();
}

CodeNodeDetailsForm::~CodeNodeDetailsForm()
{
    delete ui;
}

void CodeNodeDetailsForm::setUpEverything(){
    ui->title_LineEdit->setText(static_cast<CodeNode*>(parentNode)->title);
    ui->description_PlainTextEdit->setPlainText(static_cast<CodeNode*>(parentNode)->description);


    //QFont newCodeFont("Leelawadee UI");
    //QFont newCodeFont("ANDALE MONO");
    QFont newCodeFont("MS Reference Sans Serif");
    newCodeFont.setPointSize(8);
    newCodeFont.setStyleHint(QFont::Monospace);
    newCodeFont.setFixedPitch(true);
    codeFont = newCodeFont;

    QFontMetrics codeFM(codeFont);
    ui->code_plainTextEdit->setTabStopWidth(4 * codeFM.width(' '));
    ui->code_plainTextEdit->setFont(codeFont);
    setCode(parentNode->metaCode);


    emit on_showDescription_pushButton_clicked();

}

void CodeNodeDetailsForm::setCode(QString newCode){
    ui->code_plainTextEdit->setPlainText(newCode);
}



void CodeNodeDetailsForm::on_showDescription_pushButton_clicked()
{
    if(ui->showDescription_pushButton->text() == "show description"){
        ui->description_PlainTextEdit->show();
        ui->showDescription_pushButton->setText("hide description");
    }else{
        ui->description_PlainTextEdit->hide();
        ui->showDescription_pushButton->setText("show description");
    }
}

void CodeNodeDetailsForm::on_showCode_pushButton_clicked(){

    if(ui->showCode_pushButton->text() == "show code"){
        ui->code_widget->show();
        ui->showCode_pushButton->setText("hide code");
    }else{
        ui->code_widget->hide();
        ui->showCode_pushButton->setText("show code");
    }
}



// COLOR

void CodeNodeDetailsForm::on_changeColor_pushButton_clicked(){
    QColorDialog *colorDialog = new QColorDialog();
    colorDialog->setCustomColor(0, QColor(112, 167, 255, 100));
    colorDialog->setCustomColor(1, QColor(0, 255, 149, 100));
    colorDialog->setCustomColor(2, QColor(255, 34, 37, 100));
    colorDialog->setCustomColor(3, QColor(26, 26, 255, 100));
    colorDialog->setCustomColor(4, QColor(188, 255, 241, 100));
    int alpha = static_cast<CodeNode*>(parentNode)->color.alpha();
    static_cast<CodeNode*>(parentNode)->color = colorDialog->getColor(static_cast<CodeNode*>(parentNode)->color, NULL, NULL, QColorDialog::ShowAlphaChannel);
    static_cast<CodeNode*>(parentNode)->color.setAlpha(alpha);

    static_cast<CodeNode*>(parentNode)->updateAllInstances();
}
