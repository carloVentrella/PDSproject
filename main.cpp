#include "settingswindow.h"
#include "settings.h"

#include "userswindow.h"


#include <QApplication>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <discovery.h>
#include <QHostAddress>

int new_selection(void);

void signal_handler(int sig_no){

    if (sig_no == SIGUSR1)
        new_selection();
}

// Example paramenters, should be read from config
chrono::seconds User::MAX_SILENT = chrono::seconds(10);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // install signal handler
    signal(SIGUSR1, signal_handler);

    //settings window containing all basic informations
    SettingsWindow s;

    // Example paramenters, should be read from config
    QHostAddress groupAddress("239.255.43.21");
    quint16 port(45454);

    // list of neighbours
    shared_ptr<Users> users(new Users());

    // The scout is charge of handling the user list.
    // It sends and receives advertisements
    discovery scout(groupAddress,port, users);

//TESTING USERWINDOW (TO BE DELETED)

    //list of files to test
    vector<string> files;
    files.push_back("file1");
    files.push_back("file2");
    files.push_back("file3");

    UsersWindow *u=new UsersWindow(files, users,0);

    string image=":/thumbnails/t7.png";
    shared_ptr<User> us(new User());
    us.get()->setUsername("assuntap");
    us.get()->setIP("1");
    us.get()->setThumbnail(QIcon(image.c_str()));
    users.get()->addUser(us);

    shared_ptr<User> us1(new User());
    us1.get()->setUsername("charlesv");
    us1.get()->setIP("2");
    us1.get()->setThumbnail(QIcon(image.c_str()));
    users.get()->addUser(us1);

    int i=3;
    while(i<17)
    {
        shared_ptr<User> us(new User());
        us.get()->setUsername(std::to_string(i));
        us.get()->setIP(std::to_string(i));
        us.get()->setThumbnail(QIcon(image.c_str()));
        users.get()->addUser(us);
        i++;
    }

    u->show();
//END TESTING (TO BE DELETED)

    return a.exec();
}

int new_selection(void){

        // read selected files
        // should be read from a config file
        string f("/tmp/selected_files");
        vector<string> selected;

        ifstream file(f, ifstream::in);

        if (!file){
                cerr << "Can't retrieve selected file list: can't open file." << endl;
                return 0;
        }

        int i, n_selected_files;
        string tmp;

        getline(file,tmp);
        n_selected_files = stoi(tmp);

        cout << n_selected_files << " file selected:" << endl;

        for (i=0; i<n_selected_files; i++){
                getline(file,tmp);
                selected.push_back(move(tmp));
        }

        for (string s : selected)
                cout << s << endl;

        // Do something ...

        file.close();

        return 1;

}
