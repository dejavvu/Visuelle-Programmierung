#ifndef ALLNODESSELECTION_DIALOG_H
#define ALLNODESSELECTION_DIALOG_H

#include <QDialog>

class MainWindow;
class Node;

namespace Ui {
class AllNodesSelection_Dialog;
}

class AllNodesSelection_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllNodesSelection_Dialog(MainWindow *mainWindowInput, QWidget *parent = 0);
    ~AllNodesSelection_Dialog();

private slots:
    void destroyYourself();

private:
    Ui::AllNodesSelection_Dialog *ui;
};

#endif // ALLNODESSELECTION_DIALOG_H
