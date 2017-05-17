#ifndef SECONDSETTINGSWINDOW_H
#define SECONDSETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SecondSettingsWindow;
}

class SecondSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondSettingsWindow(QWidget *parent = 0);
    ~SecondSettingsWindow();

private:
    Ui::SecondSettingsWindow *ui;
};

#endif // SECONDSETTINGSWINDOW_H
