#include "userswindow.h"
#include "ui_userswindow.h"

#include <iostream>

UsersWindow::UsersWindow(shared_ptr<discovery> scout, QList<std::shared_ptr<QFile>> files, shared_ptr<Users> users, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersWindow)
{
    this->u=users;

    this->files=files;

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

    for(std::map<string, shared_ptr<User>>::iterator iter=this->u.get()->users.begin(); iter!=this->u.get()->users.end(); iter++)
    {
        for(j=0;j<NUM_COL && n<num_widget;j++)
        {
            //getting the icon of the user
            QIcon p=iter->second.get()->getThumbnail();

            //trasferring the icon within the label
            QLabel *pixmapLabels=createPixMapLabels(p);

            icons.push_back(pixmapLabels);

            //username
            string us=iter->second.get()->getUsername();
            //IP
            string ip=iter->second.get()->getIP();

            //creating the checkbox for its image
            QCheckBox *username=new QCheckBox(QString::fromStdString(us), this);

            username->setMinimumWidth(60);
            username->setMaximumWidth(140);

            //saving the checkbox (it will be used for the sharing function)
            allButtons.push_back(username);

            usersMap.insert(us,ip);

            QVBoxLayout *l=new QVBoxLayout;

            //every widget is part of a subLayout which will be added to the fundamental one later
            l->addWidget(pixmapLabels);
            l->addWidget(username);

            allLayouts.push_back(l);

            centralLayout->addLayout(l,i,j,1,1,Qt::Alignment());

            //this to control if we have to stop adding widgets because there are no more users
            n++;

            iter++;
        }
        iter--;

        i++;
    }

    x_lastElement=i--;
    y_lastElement=j;
    }

    //creating the button that will be used to determine who to share file with
    buttonToShare=new QPushButton("Share", this);

    buttonToShare->setMaximumWidth(140);
    buttonToShare->setMinimumWidth(60);

    buttonLayout=new QVBoxLayout;
    buttonLayout->addWidget(buttonToShare);

    //button added to the layout at the bottom of the window
    centralLayout->addLayout(buttonLayout, x_lastElement,NUM_COL-1,1,1,Qt::Alignment());

    //connecting the button to the function it has to implement
    connect(buttonToShare,&QPushButton::clicked, [this](){
        QListIterator<QCheckBox *> iter(this->allButtons);
        int flag=0;
        while(iter.hasNext())
        {
            QCheckBox * c=iter.next();

            //here we have to handle the sharing with the users that have the checkbox checked

            //username
            string s=c->text().toStdString();

            cout<<s<<endl;

            if(c->isChecked())
            {
                flag=1;
                cout<<s<<" is checked"<<endl;

                //its IP
                string ip=this->usersMap.find(s).value();

                cout<<" and this is its ip "<<ip<<endl;

                try
                {
                    //now i save the user in the selected_users list
                    shared_ptr<User> u=this->u.get()->users.at(ip);

                    selected_users.push_back(u);
                }
                catch(...)
                {
                    QString warning="User ";
                    warning.append(QString::fromStdString(s));
                    warning.append(" no longer active");
                    QMessageBox::warning(this, "Warning", warning);
                }

            }

        }

        if(selected_users.size()==0)
        {
            flag=0;
        }

        cout<<"here i am sharing"<<endl;

        if(flag==0)
        {
            QMessageBox::warning(this, "Warning", "No user checked.. Don't know who to send to..");
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


    connect(this->u.get(), SIGNAL(modifiedUsersMap(string,int,bool)),this, SLOT(handleNewOrRemovedUsers(string,int,bool)));


    connect((this->scout.get()), &discovery::modifiedThumb, this, &UsersWindow::handleModThumb);

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

void UsersWindow::handleNewOrRemovedUsers(string whatsNeeded,int whichUser, bool state)
{
    (void) whichUser;

    if(state)
    {
        //user added
        cout<<"handle for adding users"<<endl;

        string IP=whatsNeeded;

        shared_ptr<User> currentUser=this->u->users.at(IP);

        int flag=0;
        int pos;
        QListIterator<QCheckBox *> iter(this->allButtons);
        while(iter.hasNext())
        {
            if(iter.next()->text().toStdString()==currentUser->getUsername())
            {
                flag=1;
                break;
            }
            pos++;
        }

        if(flag==0)  //new user
        {
            QIcon p=currentUser->getThumbnail();

            //trasferring the icon within the label
            QLabel *pixmapLabels=createPixMapLabels(p);

            icons.push_back(pixmapLabels);

            //username
            string us=currentUser->getUsername();
            //IP
            string ip=currentUser->getIP();

            //creating the checkbox for its image
            QCheckBox *username=new QCheckBox(QString::fromStdString(us), this);

            username->setMinimumWidth(60);
            username->setMaximumWidth(140);

            //saving the checkbox (it will be used for the sharing function)
            allButtons.push_back(username);

            usersMap.insert(us,ip);

            QVBoxLayout *l=new QVBoxLayout;

            //adding each widget to a subLayout which will be added to the fundamental one
            l->addWidget(pixmapLabels);
            l->addWidget(username);

            allLayouts.push_back(l);

            if(y_lastElement>=NUM_COL)
            {
                x_lastElement++;
                y_lastElement=0;
            }

            centralLayout->removeItem(buttonLayout);
            centralLayout->addLayout(this->buttonLayout,x_lastElement+1, NUM_COL-1,1,1,Qt::Alignment());

            centralLayout->addLayout(l,x_lastElement,y_lastElement, 1,1,Qt::Alignment());
            y_lastElement++;

            this->update();
            this->adjustSize();
        }
        else
        {
            //if the user was previously on the form(so I only have all of its features disabled)
            //I only need to enable everything (do not need to create another pair of widgets)
            icons.at(pos)->setEnabled(true);

            allButtons.at(pos)->setEnabled(true);

            this->update();
        }
    }
    else
    {
        //user removed
        cout<<"handle for removing users"<<endl;

        string username=whatsNeeded;

        //I find the position of the current user over my layout
        int pos=0;
        QListIterator<QCheckBox *> iter(this->allButtons);
        while(iter.hasNext())
        {
            if(iter.next()->text().toStdString()==username)
            {
                break;
            }
            pos++;
        }

        //when I find the position I disable everything within it
        icons.at(pos)->setEnabled(false);

        allButtons.at(pos)->setEnabled(false);

        this->update();
    }
}

void UsersWindow::handleModThumb(const QIcon &value, string username)
{
    //to find which label to update
    QListIterator<QCheckBox *> iter(this->allButtons);
    int pos=0;
    while(iter.hasNext())
    {
        if(iter.next()->text().toStdString()==username)
        {
            break;
        }
        pos++;
    }

    //trasferring the icon within the label
    QLabel *pixmapLabels=createPixMapLabels(value);

    if(icons.at(pos)!=pixmapLabels)
    {
        //modifying the label
        icons[pos]=pixmapLabels;
    }
    qApp->processEvents();
}

QLabel *UsersWindow::createPixMapLabels(QIcon p)
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
