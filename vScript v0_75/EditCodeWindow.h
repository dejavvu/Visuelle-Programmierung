#ifndef EDITCODEWINDOW_H
#define EDITCODEWINDOW_H

#include <QMainWindow>

namespace Ui {
class EditCodeWindow;
}

class EditCodeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditCodeWindow(QWidget *parent = 0, QString* _code = nullptr);
    ~EditCodeWindow();

    QString* code;

private slots:
    void on_save_pushButton_clicked();

private:
    Ui::EditCodeWindow *ui;
};

#endif // EDITCODEWINDOW_H
