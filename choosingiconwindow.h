#ifndef CHOOSINGICONWINDOW_H
#define CHOOSINGICONWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "settings.h"

namespace Ui {
class ChoosingIconWindow;
}

class ChoosingIconWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChoosingIconWindow(QWidget *parent = 0);
    ~ChoosingIconWindow();

signals:
    //this signal is emitted only when the user effectively change the icon
    void iconChanged(QString nameIcon);

private:
    Ui::ChoosingIconWindow *ui;

    //this function creates the label associated with the icon
    QLabel* createPixMapLabels(QIcon p);

    //path of the current icon
    QString currentIcon;

    //to understand if the user is really changing the image
    int currentCheck;
    int movableCheck;

    QGridLayout *currentLayout;

    QList<QVBoxLayout*> allLayouts;

    //this is the list of labels with icons
    QList<QLabel*> icons;
    //this is the list of checkbox
    QList<QCheckBox *> checks;

    //parameters of the layout
    const int NUM_COL=4;

    //we have 11 predefined icons
    const int NUM_ICONS=11;

    QPushButton *closingButton;
    QPushButton *applyingChangeButton;
};

#endif // CHOOSINGICONWINDOW_H
