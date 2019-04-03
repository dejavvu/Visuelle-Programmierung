#ifndef MACRONODEDETAILSFORM_H
#define MACRONODEDETAILSFORM_H

#include <QWidget>
#include "datainterface.h"

namespace Ui {
class MacroNodeDetailsForm;
}
class MacroNode;

class MacroNodeDetailsForm : public QWidget
{
    Q_OBJECT

public:
    explicit MacroNodeDetailsForm(MacroNode *parentNodeInput, QWidget *parent = 0);
    ~MacroNodeDetailsForm();

    void setUpEverything();
    void updateInputsAndOutputs();
    int getInputIndexFromSenderName(QString name);
    int getOutputIndexFromSenderName(QString name);
    void showMessageInMainWindow(QString m);

    MacroNode *parentNode;

    QWidget *inputsWidget;
    QWidget *outputsWidget;
    QMap<QWidget*, InputPort*> inputs;
    QMap<QWidget*, OutputPort*> outputs;
    QString standardWarningMessage;

private slots:
    void on_addNewInput_pushButton_clicked();

    void on_addNewOutput_pushButton_clicked();

    void on_showDescription_pushButton_clicked();

    void renameInput(QString newName);

    void renameOutput(QString newName);

    void changeInputType(QString newType);

    void changeOutputType(QString newType);

    void deleteInput();

    void deleteOutput();

    void on_description_PlainTextEdit_textChanged();

    void on_title_LineEdit_textChanged(const QString &arg1);

    void on_changeColor_pushButton_clicked();

private:
    Ui::MacroNodeDetailsForm *ui;
};

#endif // MACRONODEDETAILSFORM_H
