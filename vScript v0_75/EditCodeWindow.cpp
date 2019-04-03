#include "EditCodeWindow.h"
#include "ui_EditCodeWindow.h"

#include <QFontMetrics>

EditCodeWindow::EditCodeWindow(QWidget *parent, QString *_code) :
    QMainWindow(parent),
    ui(new Ui::EditCodeWindow)
{
    ui->setupUi(this);
    QFontMetrics metrics(ui->code_plainTextEdit->font());
    ui->code_plainTextEdit->setTabStopWidth(4 * metrics.width('_'));

    code = _code;
    ui->code_plainTextEdit->setPlainText(*_code);
}

EditCodeWindow::~EditCodeWindow()
{
    delete ui;
}

void EditCodeWindow::on_save_pushButton_clicked()
{
    *code = ui->code_plainTextEdit->toPlainText();
    this->destroy();
}
