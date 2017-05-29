#include "userswindow.h"
#include "ui_userswindow.h"

#include <iostream>

UsersWindow::UsersWindow(vector<string> files, shared_ptr<Users> users, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersWindow)
{
    this->u=users;

    this->files=files;

    this->centralLayout=new QGridLayout(this);

    ui->setupUi(this);

    //setting the title of the window
    setWindowTitle("Decide who you want to share with:");

    //finding the number of users to be showed
    int num_widget=u.get()->users.size();


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

            centralLayout->addWidget(pixmapLabels,i,j,1,1,Qt::Alignment());

            //NB the checkbox is added the row after the current one
            centralLayout->addWidget(username,i+1,j,1,1,Qt::Alignment());

            //this to control if we have to stop adding widgets because there are no more users
            n++;

            iter++;
        }
        iter--;

        i++;
        i++;
    }


    this->t=new Transfer(this->files, this);
    //connecting the share button to starting tranfer
    connect(this, SIGNAL(startTransfer()), this->t, SLOT(transferBegin()));

    //creating the button that will be used to determine who to share file with
    QPushButton *buttonToShare=new QPushButton("Share", this);

    buttonToShare->setMaximumWidth(140);
    buttonToShare->setMinimumWidth(60);

    //button added to the layout at the bottom of the window
    centralLayout->addWidget(buttonToShare, i,NUM_COL-1,1,1,Qt::Alignment());

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

                //now i save the user in the selected_users list
                shared_ptr<User> u=this->u.get()->users.at(ip);

                selected_users.push_back(u);
            }

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

            //here it opens the window with the trasfers
            //ONLY PASS THE LIST OF USERS
            t->setSelected_users(this->selected_users);

            t->show();

            emit startTransfer();
        }
    });

    this->setLayout(centralLayout);

}

UsersWindow::~UsersWindow()
{
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

QLabel *UsersWindow::createPixMapLabels(QIcon p)
{
      QLabel *label=new QLabel();

      //to enable the label, so that it will not appear grey
      label->setEnabled(true);

      //to make sure the image is centered in the box
      label->setAlignment(Qt::AlignCenter);
      label->setFrameShape(QFrame::Box);

      label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      label->setBackgroundRole(QPalette::Base);

      label->setAutoFillBackground(true);

      label->setMinimumSize(60, 60);
      label->setMaximumSize(140,140);

      //to associate the label and the icon
      const QPixmap pixmap=p.pixmap(100,100, QIcon::Normal, QIcon::On);
      label->setPixmap(pixmap);

      return label;
}
