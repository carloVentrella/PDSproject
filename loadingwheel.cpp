#include "loadingwheel.h"
#include "ui_loadingwheel.h"

#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QString>
#include <QDesktopWidget>
#include <QApplication>

LoadingWheel::LoadingWheel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadingWheel)
{
    ui->setupUi(this);
    this->progress=0;

    setWindowTitle("Receiving files...");

    this->resize(100,110);

    //to make it appear at the right top of the screen
    QDesktopWidget *desktop=QApplication::desktop();
    int x=desktop->width();
    int y=desktop->height();

    this->move(x,y-200);
}

LoadingWheel::~LoadingWheel()
{
    delete ui;
}

void LoadingWheel::setProgress(float value)
{
    if(value<0)
        value=0;

    progress = (float)value*100;

    this->update();
    qApp->processEvents();
}

void LoadingWheel::paintEvent(QPaintEvent *)
{
    QPainter p(this);

        QPen pen;
        pen.setWidth(7);
        pen.setColor(Qt::blue);
        p.setPen(pen);

        p.setRenderHint(QPainter::Antialiasing);

        QRectF rectangle(10.0, 20.0, 80.0, 80.0);
        int startAngle = -90 * 16;
        int spanAngle = progress/100 * 360 * 16;

        p.drawArc(rectangle, startAngle, spanAngle);

        p.drawText(rectangle,Qt::AlignCenter,QString::number((int)progress)+" %");
}

void LoadingWheel::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible()) {
                return;
            }
    hide();
    event->ignore();
}
