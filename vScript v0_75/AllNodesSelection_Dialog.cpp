#include "AllNodesSelection_Dialog.h"
#include "ui_AllNodesSelection_Dialog.h"

#include <QDebug>

#include "NodeSelection_Widget.h"
#include "mainwindow.h"
#include "node.h"
#include "MacroNode.h"
#include "CodeNode.h"

AllNodesSelection_Dialog::AllNodesSelection_Dialog(MainWindow *mainWindowInput, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllNodesSelection_Dialog)
{
    ui->setupUi(this);
    NodeSelection_Widget *selectionWidget = new NodeSelection_Widget(mainWindowInput, this);
    ui->centralwidget = selectionWidget;

    connect(selectionWidget, SIGNAL(destroying()), this, SLOT(destroyYourself()));
}

AllNodesSelection_Dialog::~AllNodesSelection_Dialog()
{
    delete ui;
}

void AllNodesSelection_Dialog::destroyYourself(){
    this->destroy();
}
