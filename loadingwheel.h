#ifndef LOADINGWHEEL_H
#define LOADINGWHEEL_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class LoadingWheel;
}

class LoadingWheel : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingWheel(QWidget *parent = 0);
    ~LoadingWheel();

public slots:
    void setProgress(float value);

protected:
    void paintEvent(QPaintEvent *);

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::LoadingWheel *ui;

    float progress;
};

#endif // LOADINGWHEEL_H
