#include "choosingiconwindow.h"
#include "ui_choosingiconwindow.h"

using namespace std;

ChoosingIconWindow::ChoosingIconWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoosingIconWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Choose the new icon:");

    currentLayout=new QGridLayout(this);

    currentIcon=Settings::getInstance().getThumbPath();

    int n=1; //to number how many icons we are showing
    int i=0; //parameters for the gridLayout
    int j=0;
    while(n<=NUM_ICONS)
    {
        //getting the icon of the user
        QString icon;
        icon=":/thumbnails/";

        //getting the number of the icon
        QString imageNumber=QString::number(n);

        icon.append(imageNumber);
        icon.append(".png");

        QIcon p=QIcon(icon);

        //trasferring the icon within the label
        QLabel *pixmapLabels=createPixMapLabels(p);

        icons.push_back(pixmapLabels);

        //creating the checkbox for its image
        QCheckBox *check=new QCheckBox(imageNumber, this);

        check->setMinimumWidth(60);
        check->setMaximumWidth(140);

        if(icon==currentIcon) //to check only the checkBox related to the actual icon
        {
            check->setChecked(true);
            currentCheck=n;
        }
        else
            check->setChecked(false);

        connect(check, &QCheckBox::clicked, this, [this, check]{

                //if i check another icon, i don't want any other icon to be selected,
                //i can't have 2 or more icons
                QListIterator<QCheckBox *> iter(this->checks);
                while(iter.hasNext())
                {
                    QCheckBox* c=iter.next();
                    if(c->text()!=check->text() && c->isChecked())
                    {
                        c->setChecked(false);
                    }
                }

                movableCheck=check->text().toInt();
        });

        //saving the checkbox
        checks.push_back(check);

        QVBoxLayout *l=new QVBoxLayout;

        //every widget is part of a subLayout which will be added to the fundamental one later
        l->addWidget(pixmapLabels);
        l->addWidget(check);

        allLayouts.push_back(l);

        currentLayout->addLayout(l,i,j,1,1,Qt::Alignment());

        j++;

        if(j>=NUM_COL)
        {
            j=0;
            i++;
        }

        n++;
    }

    //button to delete the operation
    closingButton=new QPushButton();
    closingButton->setText("Cancel");
    connect(closingButton, &QPushButton::clicked, this, [this]{

        QListIterator<QCheckBox *> iter(this->checks);
        while(iter.hasNext())
        {
            QCheckBox* c=iter.next();
            if(c->text().toInt()==movableCheck)
            {
                c->setChecked(false);
            }
            if(c->text().toInt()==currentCheck)
            {
                c->setChecked(true);
            }
        }


        this->close();
    });

    currentLayout->addWidget(closingButton, i+1, 0, 1, 1, Qt::Alignment());

    //button to apply the change
    applyingChangeButton=new QPushButton();
    applyingChangeButton->setText("Apply");
    connect(applyingChangeButton, &QPushButton::clicked, this, [this]{
        QString icon;
        icon=":/thumbnails/"+QString::number(movableCheck);
        icon.append(".png");

        if(currentIcon!=icon) //am i really changing the icon?
        {
            this->currentIcon=icon;
            this->currentCheck=movableCheck;
            emit iconChanged(icon);
            this->close();
        }
        else
        {
            this->close();
        }
    });

    currentLayout->addWidget(applyingChangeButton, i+1, 1, 1, 1, Qt::Alignment());
}

ChoosingIconWindow::~ChoosingIconWindow()
{
    delete closingButton;
    delete applyingChangeButton;
    delete currentLayout;
    delete ui;
}


QLabel *ChoosingIconWindow::createPixMapLabels(QIcon p)
{
    QLabel *label=new QLabel();

    //to enable the label, so that it will not appear grey
    label->setEnabled(true);

    //to make sure the image is centered in the box
    label->setAlignment(Qt::AlignCenter);
    label->setFrameShape(QFrame::Box);

    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // This assures a transparent background
    label->setBackgroundRole(QPalette::NoRole);
    label->setStyleSheet("border:0px");

    label->setAutoFillBackground(true);

    label->setMinimumSize(100, 100);
    label->setMaximumSize(140,140);

    //to associate the label and the icon
    const QPixmap pixmap=p.pixmap(100,100, QIcon::Normal, QIcon::On);
    label->setPixmap(pixmap);

    return label;
}
