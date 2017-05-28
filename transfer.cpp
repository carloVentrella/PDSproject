#include "transfer.h"
#include "ui_transfer.h"

#include <iostream>

Transfer::Transfer(vector<string> files, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Transfer)
{
    this->files=files;

    this->centralLayout=new QVBoxLayout(this);

    ui->setupUi(this);

    setWindowTitle("Transfer");


    QLabel *label=new QLabel("Sending files..", this, Qt::WindowFlags());
    centralLayout->addWidget(label);

    progressBar=new QProgressBar(this);
    progressBar->setValue(0);

    centralLayout->addWidget(progressBar);

    remainingTime=new QLabel(this);
    centralLayout->addWidget(remainingTime);


    //has to be handled the widget for the possibility of canceling the operation
    cancelOperation=new QPushButton("Cancel Operation", this);
    cancelOperation->setMaximumHeight(20);
    cancelOperation->setMaximumWidth(250);

    centralLayout->addWidget(cancelOperation);

    connect(cancelOperation, &QPushButton::clicked, [this](){

        //TO BE HANDLED: CANCELING OPERATION


        hide();

    });


    this->setLayout(centralLayout);

    //transferFilesToUsers();
}

Transfer::~Transfer()
{
    delete progressBar;
    delete remainingTime;
    delete centralLayout;
    delete ui;
}

void Transfer::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible()) {
                return;
            }
    hide();
    event->ignore();

}

QList<shared_ptr<User> > Transfer::getSelected_users() const
{
    return selected_users;
}

void Transfer::setSelected_users(const QList<shared_ptr<User> > &value)
{
    selected_users = value;
}

void Transfer::transferBegin()
{
    cout<<"i'm here transferring"<<endl;

    //HERE SENDING AND FUNCTION TO EVALUATE REMAINING TIME HAVE TO BE DONE

    int h=rand();
    for(int i=0;i<=100; i++)
    {
        int j=0;
        while(j<100)
        {
            cout<<std::to_string(j)<<endl;
            j++;
            int h=0;
            while(h<600)
            {
                h++;
            }
        }

        //this functions work (they set the value of the progress bar and of the remaining time)
        this->progressBar->setValue(i);
        h=h-i;
        this->remainingTime->setText(std::to_string(h).append(" left").c_str());
    }


}
