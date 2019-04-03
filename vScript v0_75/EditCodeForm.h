#ifndef EDITCODEFORM_H
#define EDITCODEFORM_H

#include <QWidget>

namespace Ui {
class EditCodeForm;
}

class EditCodeForm : public QWidget
{
    Q_OBJECT

public:
    explicit EditCodeForm(QWidget *parent = 0);
    ~EditCodeForm();

private:
    Ui::EditCodeForm *ui;
};

#endif // EDITCODEFORM_H
