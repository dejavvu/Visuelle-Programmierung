#include "EditCodeForm.h"
#include "ui_EditCodeForm.h"

EditCodeForm::EditCodeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditCodeForm)
{
    ui->setupUi(this);
}

EditCodeForm::~EditCodeForm()
{
    delete ui;
}
