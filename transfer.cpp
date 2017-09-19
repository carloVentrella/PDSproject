#include "transfer.h"
#include "ui_transfer.h"

#include <iostream>
#include <QApplication>
#include <QDesktopWidget>

Transfer::Transfer(QList<shared_ptr<User> > selected_users, QList< std::shared_ptr<QFile>> files, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Transfer)
{
    this->files=files;
    this->selected_users=selected_users;

    //this flag is needed to understand if all the threads have finished their work so that the window can be closed
    this->flagToQuit=0;

    //this flag is needed to understand whether to  display the bars of each user or not
    this->visible=false;

    ui->setupUi(this);

    this->verticalLayout=new QVBoxLayout;

    setWindowTitle("Transfer");

    //to make it appear at the right top of the screen
    QDesktopWidget *desktop=QApplication::desktop();
    int screenWidth=desktop->width();

    int x=screenWidth;
    int y=0;

    this->move(x,y);
    //

    QLabel *label=new QLabel("Sending files to...", this);
    verticalLayout->addWidget(label);

    usersLabel=new QLabel(" ", this, Qt::WindowFlags());
    verticalLayout->addWidget(usersLabel);

    progressBar=new QProgressBar(this);
    progressBar->setValue(0);

    verticalLayout->addWidget(progressBar);

    remainingTime=new QLabel(this);
    verticalLayout->addWidget(remainingTime);

    cancelOperation=new QPushButton("Cancel Operation", this);
    cancelOperation->setMaximumHeight(20);
    cancelOperation->setMaximumWidth(250);

    verticalLayout->addWidget(cancelOperation);

    connect(cancelOperation, &QPushButton::clicked, [this](){

        for(int i=0;i<this->sendingThreads.size();i++)
        {
            if(this->flags.at(i)==0)  //maybe the thread has already terminated
            {
                this->flags.at(i)=1; //this flag will make the thread stop

                if(!this->sendingThreads.at(i)->wait(5000)) //wait untill it actually has terminated
                {
                        this->sendingThreads.at(i)->terminate(); //thread didn't exit in time, terminate it
                        this->sendingThreads.at(i)->wait(); //wait to synchronize
                }
            }
        }

        hide();

    });


    singleTransfers=new QPushButton("Show each single transfer", this);

    QPalette pal=singleTransfers->palette();
    pal.setColor(QPalette::Button, QColor(Qt::blue));
    singleTransfers->setPalette(pal);
    singleTransfers->update();
    singleTransfers->setMaximumHeight(20);
    singleTransfers->setMaximumWidth(200);

    connect(singleTransfers, &QPushButton::clicked, [this](){

        if(this->visible)
        {
            //got to hide
            QListIterator<QLabel*> iterl(this->usersLabelPerSingleTransfer);
            while(iterl.hasNext())
            {
                iterl.next()->setVisible(false);
            }
            QListIterator<QProgressBar*> iterpb(this->progressBarPerSingleTransfer);
            while(iterpb.hasNext())
            {
                iterpb.next()->setVisible(false);
            }
            QListIterator<QLabel*> iterrt(this->remainingTimePerSingleTransfer);
            while(iterrt.hasNext())
            {
                iterrt.next()->setVisible(false);
            }
            QListIterator<QPushButton*> iterb(this->cancelOperationPerSingleTransfer);
            while(iterb.hasNext())
            {
                iterb.next()->setVisible(false);
            }

            this->visible=false;
        }
        else
        {
            //got to show
            QListIterator<QLabel*> iterl(this->usersLabelPerSingleTransfer);
            while(iterl.hasNext())
            {
                iterl.next()->setVisible(true);
            }
            QListIterator<QProgressBar*> iterpb(this->progressBarPerSingleTransfer);
            while(iterpb.hasNext())
            {
                iterpb.next()->setVisible(true);
            }
            QListIterator<QLabel*> iterrt(this->remainingTimePerSingleTransfer);
            while(iterrt.hasNext())
            {
                iterrt.next()->setVisible(true);
            }
            QListIterator<QPushButton*> iterb(this->cancelOperationPerSingleTransfer);
            while(iterb.hasNext())
            {
                iterb.next()->setVisible(true);
            }
            this->visible=true;
        }

        this->adjustSize();

    });


    verticalLayout->addWidget(singleTransfers);

    //users progress time cancel
    QListIterator<shared_ptr<User>> iter(this->selected_users);
    while(iter.hasNext())
    {
        shared_ptr<User> u=iter.next();

        string sending=" - Sending to ";  //for each transfer I know who I am transferring to
        sending.append(u.get()->getUsername());

        QLabel* label=new QLabel(QString::fromStdString(sending),this);
        label->setVisible(false);
        this->usersLabelPerSingleTransfer.push_back(label);
        this->verticalLayout->addWidget(label);

        QProgressBar* pb=new QProgressBar(this);
        pb->setValue(0);
        pb->setVisible(false);
        this->progressBarPerSingleTransfer.push_back(pb);
        this->verticalLayout->addWidget(pb);

        QLabel* rm=new QLabel("", this, Qt::WindowFlags());
        rm->setVisible(false);
        this->remainingTimePerSingleTransfer.push_back(rm);
        this->verticalLayout->addWidget(rm);

        QPushButton* c=new QPushButton("Cancel this transfer",this);
        c->setMaximumHeight(15);
        c->setMaximumWidth(150);
        c->setVisible(false);
        this->cancelOperationPerSingleTransfer.push_back(c);
        verticalLayout->addWidget(c);

        connect(c, &QPushButton::clicked, [this,c](){
            int position=this->cancelOperationPerSingleTransfer.indexOf(c);

            if(this->flags.at(position)==0)
            {
                this->flags.at(position)=1;  //this flag will make the thread stop

                if(!this->sendingThreads.at(position)->wait(5000)) //wait untill it actually has terminated
                {
                        this->sendingThreads.at(position)->terminate(); //thread didn't exit in time, terminate it
                        this->sendingThreads.at(position)->wait(); //wait to synchronize
                }

                //this function will be done after exiting the thread
                this->handleCancelation(position);


                //check if there are other threads working;
                //if there are not any, close the window
                int i=0;
                while(i!=(int) flags.size())
                {
                    if(flags.at(i)==0)
                    {
                        break;
                    }
                    i++;
                }
                if(i==(int)flags.size())
                {
                    this->hide();
                }
            }

        });

        this->flags.push_back(0);

    }

    this->setLayout(verticalLayout);
    this->adjustSize();

    //connecting each thread to the signal that can handle it
    for(int pos=0;pos<this->selected_users.size();pos++)
    {
        WorkerThread* t=new WorkerThread(this, this, pos);
        connect(t,SIGNAL(finished(int)), this,SLOT(transferEnd(int))); //signal to handle the end of the work in each thread
        connect(t, SIGNAL(processEvents()), this, SLOT(handleProcessEvents())); //signal to update the window
        connect(t,SIGNAL(progBarModifying(int,int)),this, SLOT(handleProgBarModifying(int,int))); //signal to update the progBar for each user
        connect(t,SIGNAL(progBarModifying()),this, SLOT(handleProgBarModifying())); //signal to update the general progBar
        connect(t,SIGNAL(remTimeModifying(QString,int)), this, SLOT(handleRemTimeModifying(QString,int))); //signal to update the label of the remainingTime for each user
        connect(t,SIGNAL(remTimeModifying(QString)),this,SLOT(handleRemTimeModifying(QString))); //signal to handle the label of the general remaining time

        this->sendingThreads.push_back(std::move(t));
    }

}

Transfer::~Transfer()
{
    //here i have to stop each single thread, join and then exit
    for(int i=0;i<this->sendingThreads.size();i++)
    {
        if(this->flags.at(i)==0)
        {
            this->flags.at(i)=1;
            if(!this->sendingThreads.at(i)->wait(5000)) //wait untill it actually has terminated
            {
                    this->sendingThreads.at(i)->terminate(); //thread didn't exit in time, terminate it
                    this->sendingThreads.at(i)->wait(); //wait to synchronize
            }
        }
    }

    delete singleTransfers;
    delete progressBar;
    delete usersLabel;
    delete cancelOperation;
    delete remainingTime;
    delete verticalLayout;
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

QList<std::shared_ptr<QFile>> Transfer::getFiles() const
{
    return files;
}

void Transfer::setFiles(const QList<std::shared_ptr<QFile>> &value)
{
    files = value;
}

QLabel *Transfer::getRemainingTime() const
{
    return remainingTime;
}

void Transfer::setRemainingTime(QLabel *value)
{
    remainingTime = value;
}

QProgressBar *Transfer::getProgressBar() const
{
    return progressBar;
}

void Transfer::setProgressBar(QProgressBar *value)
{
    progressBar = value;
}

QList<shared_ptr<User> > Transfer::getSelected_users() const
{
    return selected_users;
}

void Transfer::setSelected_users(const QList<shared_ptr<User> > &value)
{
    selected_users = value;
}

void Transfer::handleCancelation(int node)
{   int precValue=this->progressBar->value();
    int thisValue=this->progressBarPerSingleTransfer.at(node)->value();
    int n=this->selected_users.size();
    this->progressBar->setValue(precValue+(100-thisValue)/n);
    qApp->processEvents();
    this->progressBarPerSingleTransfer.at(node)->setValue(100);
    qApp->processEvents();
    this->remainingTimePerSingleTransfer.at(node)->setText(QString::fromStdString("0 seconds left"));
    qApp->processEvents();

    this->update();
    qApp->processEvents();
}

int Transfer::getFlagAtNode(int node)
{
    return this->flags.at(node);
}

void Transfer::transferBegin()
{
    //I want to show who are the users I want to send files
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

    //here the threads start
    int i;
    for(i=0;i<this->sendingThreads.size();i++)
    {
        this->sendingThreads.at(i)->start();
    }

}

void Transfer::transferEnd(int node)
{
    this->flags.at(node)=1; //this flag will make the thread stop

    if(!this->sendingThreads.at(node)->wait(5000)) //wait untill it actually has terminated
    {
            this->sendingThreads.at(node)->terminate(); //thread didn't exit in time, terminate it
            this->sendingThreads.at(node)->wait(); //wait to synchronize
    }

    this->flagToQuit++;
    if(this->flagToQuit==this->sendingThreads.size()) //if all threads have terminated, the window closes
    {
        this->hide();
    }
}

void Transfer::handleProcessEvents()
{
    qApp->processEvents();
}

void Transfer::handleProgBarModifying(int value, int node)
{
    this->mBar.lock();

    this->progressBarPerSingleTransfer.at(node)->setValue(value);

    this->mBar.unlock();


    if(value==100)
    {
        this->flags.at(node)=1;
    }

}

void Transfer::handleProgBarModifying()
{
    this->mBar.lock();

    int v=0;
    QListIterator<QProgressBar*>  iter(this->progressBarPerSingleTransfer);
    while(iter.hasNext())
    {
        v=v+iter.next()->value();
    }

    v=v/this->sendingThreads.size();

    this->progressBar->setValue(v);

    this->mBar.unlock();
}

void Transfer::handleRemTimeModifying(QString value, int node)
{
    this->remainingTimePerSingleTransfer.at(node)->setText(value);
}

void Transfer::handleRemTimeModifying(QString value)
{
    this->mTime.lock();
    this->remainingTime->setText(value);
    this->mTime.unlock();
}
