#ifndef ASKCONFIRMATIONWINDOW_H
#define ASKCONFIRMATIONWINDOW_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>

namespace Ui {
class AskConfirmationWindow;
}

class AskConfirmationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AskConfirmationWindow(QString username, int dimension, QWidget *parent = 0);
    ~AskConfirmationWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_YesButton_clicked();

    void on_NoButton_clicked();

signals:
    //this signal is used by the form to let the socketthread accept the reception
    void canBeReceived();
    void cannotBeReceived();

private:
    Ui::AskConfirmationWindow *ui;
};

#endif // ASKCONFIRMATIONWINDOW_H
