#ifndef SINGLETRANSFERS_H
#define SINGLETRANSFERS_H

#include <QDialog>

namespace Ui {
class SingleTransfers;
}

class SingleTransfers : public QDialog
{
    Q_OBJECT

public:
    explicit SingleTransfers(QWidget *parent = 0);
    ~SingleTransfers();

private:
    Ui::SingleTransfers *ui;
};

#endif // SINGLETRANSFERS_H
