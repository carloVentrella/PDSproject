#ifndef PRESETTINGS_H
#define PRESETTINGS_H

#include <QWidget>

namespace Ui {
class PreSettings;
}

class PreSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PreSettings(QWidget *parent = 0);
    ~PreSettings();

private:
    Ui::PreSettings *ui;
};

#endif // PRESETTINGS_H
