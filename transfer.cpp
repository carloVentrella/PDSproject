#include "transfer.h"
#include "ui_transfer.h"

#include <iostream>
#include <QApplication>

Transfer::Transfer(vector<string> files, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Transfer)
{
    this->files=files;

    this->flag=0;

    this->centralLayout=new QVBoxLayout(this);

    ui->setupUi(this);

    setWindowTitle("Transfer");

    QLabel *label=new QLabel("Sending files to...", this, Qt::WindowFlags());
    centralLayout->addWidget(label);

    usersLabel=new QLabel(" ", this, Qt::WindowFlags());
    centralLayout->addWidget(usersLabel);

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

        flag=1;
        hide();

    });


    this->setLayout(centralLayout);
    this->setGeometry(8000,0, 250, 140);

    //transferFilesToUsers();
}

Transfer::~Transfer()
{
    delete progressBar;
    delete usersLabel;
    delete cancelOperation;
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
    string users;
    QListIterator<shared_ptr<User>> iter(this->selected_users);
    while(iter.hasNext())
    {
        users.append(" ");
        shared_ptr<User> u=iter.next();
        users.append(u.get()->getUsername());
        if(iter.hasNext())
        {
            users.append(",");
        }
    }

    usersLabel->setText(QString::fromStdString(users));

    qApp->processEvents();

    cout<<"i'm here transferring"<<endl;

    //HERE SENDING AND FUNCTION TO EVALUATE REMAINING TIME HAVE TO BE DONE

    int hr=100;
    for(int i=0;i<=100 && flag==0; i++)
    {
        int j=0;
        while(j<100 && flag==0)
        {
            cout<<std::to_string(j)<<endl;
            j++;
            int h=0;
            while(h<600 && flag==0)
            {
                h++;
                int g=0;
                while(g<6000 && flag==0)
                {
                    g++;
                }
            }
        }

        //this functions work (they set the value of the progress bar and of the remaining time)
        this->progressBar->setValue(i);
        hr=100-i;
        this->remainingTime->setText(std::to_string(hr).append(" seconds left").c_str());
        qApp->processEvents();
    }


}
