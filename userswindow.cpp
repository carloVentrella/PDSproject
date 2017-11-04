#include "userswindow.h"
#include "ui_userswindow.h"

#include <iostream>

UsersWindow::UsersWindow(shared_ptr<discovery> scout, QList<std::shared_ptr<QFile>> files, shared_ptr<Users> users, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersWindow)
{    
    this->u=users;

    this->files=files;
    qDebug() << "Starting userwindows of " << this->files.size() << " files";
    this->scout=scout;

    this->x_lastElement=0;
    this->y_lastElement=0;

    this->centralLayout=new QGridLayout(this);

    ui->setupUi(this);

    //setting the title of the window
    setWindowTitle("Decide who you want to share with:");

    //finding the number of users to be showed
    int num_widget=u.get()->users.size();

    if(num_widget!=0)
    {
    //finding the right number of rows (if the division has a remainder!=0 the number of rows has to be increased by 1)
    if(num_widget%NUM_COL==0)
    {
        NUM_ROWS=num_widget/NUM_COL;
    }
    else
    {
        NUM_ROWS=num_widget/NUM_COL+1;
    }

    int n=0;
    int i=0;
    int j;

    for(std::map<QString, shared_ptr<User>>::iterator iter=this->u.get()->users.begin(); iter!=this->u.get()->users.end(); iter++)
    {
        for(j=0;j<NUM_COL && n<num_widget;j++)
        {
            User* user=iter->second.get();
            QIcon p;

            connect(user, SIGNAL(modifiedThumb(QIcon,QString)), this, SLOT(handleModThumb(QIcon,QString)));
            p=user->getThumbnail();

            //username
            QString us=user->getUsername();
            //IP
            QString ip=user->getIP();

            QToolButton* button = new QToolButton(this);
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            button->setIcon(p);
            button->setIconSize(QSize(100,100));

            button->setText(us);
            button->setAutoRaise(true);
            button->setCheckable(true);

            //saving the checkbox (it will be used for the sharing function)
            //allButtons.push_back(username);
            allButtons.push_back(button);
            usersMap.insert(us,ip);

            QVBoxLayout *l=new QVBoxLayout;

            //every widget is part of a subLayout which will be added to the fundamental one later
            l->addWidget(button);

            allLayouts.push_back(l);

            centralLayout->addLayout(l,i,j,1,1,Qt::Alignment());

            //this to control if we have to stop adding widgets because there are no more users
            n++;

            iter++;
        }
        iter--;

        i++;
    }

    i--;
    x_lastElement=i;
    y_lastElement=j;
    }

    //creating the button that will be used to determine who to share file with
    buttonToShare=new QPushButton("Share", this);

    buttonToShare->setMaximumWidth(140);
    buttonToShare->setMinimumWidth(60);

    buttonLayout=new QVBoxLayout;
    buttonLayout->addWidget(buttonToShare);

    //button added to the layout at the bottom of the window
    centralLayout->addLayout(buttonLayout, x_lastElement+1,NUM_COL-1,1,1,Qt::Alignment());

    //connecting the button to the function it has to implement
    connect(buttonToShare,&QPushButton::clicked, [this](){
        QListIterator<QToolButton *> iter(this->allButtons);

        int flag=0;
        while(iter.hasNext())
        {
            //QCheckBox * c=iter.next();
            QToolButton * c = iter.next();

            //here we have to handle the sharing with the users that have the checkbox checked

            //username
            QString s=c->text();

            if(c->isChecked())
            {
                flag=1;
                qDebug()<<s<<" is checked";

                //its IP
                QString ip=this->usersMap.find(s).value();

                qDebug()<<" and this is its ip "<<ip;

                try
                {
                    //now i save the user in the selected_users list
                    shared_ptr<User> u=this->u.get()->users.at(ip);

                    selected_users.push_back(u);
                }
                catch(...)
                {
                    QString warning="User ";
                    warning.append(s);
                    warning.append(" no longer active");
                    QMessageBox::warning(this, "Warning", warning);
                }

            }

        }

        if(selected_users.size()==0)
        {
            flag=0;
        }

        if(!flag)
        {
            QMessageBox::warning(this, "Warning", "No user checked.. Don't know who to send to..");
        }
        else if(!this->files.size()){
            QMessageBox::warning(this, "Warning", "No file selected.");
        }
        else
        {
            //now i have to close the window without stopping the applications
            hide();

            //here it opens the window with the trasfers after connecting signal and slot
            Transfer *t=new Transfer(this->selected_users,this->files, 0);
            connect(this, SIGNAL(startTransfer()), t, SLOT(transferBegin()));

            t->show();

            emit startTransfer();
        }
    });

    this->setLayout(centralLayout);


    connect(this->u.get(), SIGNAL(modifiedUsersMap(QString,bool)),this, SLOT(handleNewOrRemovedUsers(QString,bool)));

}

UsersWindow::~UsersWindow()
{
    delete buttonToShare;
    delete buttonLayout;
    delete centralLayout;
    delete ui;
}

void UsersWindow::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible()) {
                return;
            }
    hide();
    event->ignore();

}

void UsersWindow::handleNewOrRemovedUsers(QString whatsNeeded, bool state)
{
    if(state)
    {
        //user added
        cout<<"handle for adding users"<<endl;

        QString IP=whatsNeeded;

        shared_ptr<User> currentUser=this->u->users.at(IP);

        int flag=0;
        int pos=0;
        QListIterator<QToolButton *> iter(this->allButtons);

        while(iter.hasNext())
        {
            if(iter.next()->text()==currentUser->getUsername())
            {
                flag=1;
                break;
            }
            pos++;
        }

        if(flag==0)  //new user
        {
            QIcon p;

            connect(currentUser.get(), SIGNAL(modifiedThumb(QIcon,QString)), this, SLOT(handleModThumb(QIcon,QString)));
            p=currentUser->getThumbnail();

            //username
            QString us=currentUser->getUsername();
            //IP
            QString ip=currentUser->getIP();

            QToolButton* button = new QToolButton(this);
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            button->setIcon(p);
            button->setIconSize(QSize(100,100));

            button->setText(us);
            button->setAutoRaise(true);
            button->setCheckable(true);

            //saving the checkbox (it will be used for the sharing function)
            allButtons.push_back(button);

            usersMap.insert(us,ip);

            QVBoxLayout *l=new QVBoxLayout;

            //adding each widget to a subLayout which will be added to the fundamental one
            l->addWidget(button);

            allLayouts.push_back(l);

            if(y_lastElement>=NUM_COL)
            {
                x_lastElement++;
                y_lastElement=0;
            }

            centralLayout->removeItem(buttonLayout);

            centralLayout->addLayout(l,x_lastElement,y_lastElement, 1,1,Qt::Alignment());
            centralLayout->addLayout(this->buttonLayout,x_lastElement+1, NUM_COL-1,1,1,Qt::Alignment());

            y_lastElement++;

            this->update();
            this->adjustSize();
        }
        else
        {
            //if the user was previously on the form(so I only have all of its features disabled)
            //I only need to enable everything (do not need to create another pair of widgets)

            allButtons.at(pos)->setEnabled(true);

            this->update();
        }
    }
    else
    {
        //user removed
        cout<<"handle for removing users"<<endl;

        QString username=whatsNeeded;

        //I find the position of the current user over my layout
        int pos=0;
        QListIterator<QToolButton *> iter(this->allButtons);

        while(iter.hasNext())
        {
            if(iter.next()->text()==username)
            {
                break;
            }
            pos++;
        }

        allButtons.at(pos)->setEnabled(false);

        this->update();
    }
}

void UsersWindow::handleModThumb(const QIcon &value, QString username)
{

    //to find which label to update
    QListIterator<QToolButton *> iter(this->allButtons);
    int pos=0;
    while(iter.hasNext())
    {
        if(iter.next()->text()==username)
        {
            break;
        }
        pos++;
    }

    allButtons.at(pos)->setIcon(value);
    allButtons.at(pos)->repaint();

    qApp->processEvents();
}
