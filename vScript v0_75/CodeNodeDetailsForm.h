#ifndef CODENODEDETAILSFORM_H
#define CODENODEDETAILSFORM_H

#include <QWidget>
#include "node.h"

namespace Ui {
class CodeNodeDetailsForm;
}
class CodeNode;

class CodeNodeDetailsForm : public QWidget
{
    Q_OBJECT

public:
    explicit CodeNodeDetailsForm(CodeNode *parentNodeInput, QWidget *parent = 0);
    ~CodeNodeDetailsForm();

    void setUpEverything();
    void setCode(QString newCode);
    //void updateContentFromSelectedNode(NodeInstance *node);

    CodeNode *parentNode;


    QWidget *inputsWidget;
    QWidget *outputsWidget;
    QMap<QWidget*, InputPort*> inputs;
    QMap<QWidget*, OutputPort*> outputs;

    QFont codeFont;

private slots:

    void on_showDescription_pushButton_clicked();

    void on_changeColor_pushButton_clicked();

    void on_showCode_pushButton_clicked();

private:
    Ui::CodeNodeDetailsForm *ui;
};

#endif // CODENODEDETAILSFORM_H
