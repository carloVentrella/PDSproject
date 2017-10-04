#include "settingswindow.h"
#include "settings.h"
#include "tcpserver.h"
#include "userswindow.h"
#include "discovery.h"


#include <QApplication>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <QHostAddress>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

int new_selection(void);
void createFileList(QFileInfoList FileInfoList);

// list of neighbours
shared_ptr<Users> users;
// list of selected files
QList<std::shared_ptr<QFile>> fileList;
// scout
shared_ptr<discovery> scout;

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

    users = std::make_shared<Users>();

    // The scout is in charge of handling the user list.
    // It sends and receives advertisements
    scout = discovery::getInstance(groupAddress,port, users);

    //TESTING USERWINDOW (TO BE DELETED)

    QHostAddress serverAddr;
    qint16 serverPort;

    QDir dirtest;

    if ( argc > 1 && atoi(argv[1]) == 1){

        // Config 1, RECEIVER

        string image2=":/thumbnails/8.png";
        dirtest.setPath("/home/assuntap/remember.txt");

        shared_ptr<User> us(new User());
        us.get()->setUsername("assuntap");
        us.get()->setIP("127.0.0.1");
        us.get()->setThumbnail(QIcon(image2.c_str()));
        users.get()->addUser(us);

        // Example paramenters, should be read from config
        serverAddr.setAddress("127.0.0.1");
        serverPort = 5555;

        Settings::getInstance().setTCPServerAddr("127.0.0.1");
        Settings::getInstance().setTCPServerPort(5556);


    }else{

        // Config 2, SENDER

        string image=":/thumbnails/10.png";
        dirtest.setPath("/home/assuntap/remember.txt");

        shared_ptr<User> us1(new User());
        us1.get()->setUsername("charlesv");
        us1.get()->setIP("localhost");
        us1.get()->setThumbnail(QIcon(image.c_str()));
        users.get()->addUser(us1);

        // Example paramenters, should be read from config
        serverAddr.setAddress("127.0.0.1");
        serverPort = 5556;

        Settings::getInstance().setTCPServerAddr("127.0.0.1");
        Settings::getInstance().setTCPServerPort(5555);

    }

    Server tcpServer(serverAddr, serverPort);
    tcpServer.start();

    QString path = dirtest.absolutePath();

    QFileInfoList files;
    QFileInfo fileInfo(path);
    files.append(fileInfo);
    createFileList(files);

    string image3=":/thumbnails/6.png";
    string image4=":/thumbnails/4.png";

    //list of files to test
    //vector<string> files;
    //files.push_back("file1");
    //files.push_back("file2");
    //files.push_back("file3");


    shared_ptr<User> us3(new User());
    us3->setUsername("Prof2");
    us3->setIP(std::to_string(4));
    us3->setThumbnail(QIcon(image3.c_str()));
    users->addUser(us3);

    shared_ptr<User> us4(new User());
    us4->setUsername("Prof1");
    us4->setIP(std::to_string(5));
    us4->setThumbnail(QIcon(image4.c_str()));
    users->addUser(us4);

    UsersWindow *u=new UsersWindow(scout,fileList, users,0);

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

        QFileInfoList files;

        for (string s : selected){

                QString path = QString::fromStdString(s);

                // remove "file://"
                path = path.mid(7,-1);
                qDebug() << path;

                QFileInfo fileInfo(path);
                files.append(fileInfo);
        }

        // Create file/dir tree
        createFileList(files);

        if( fileList.size() == 0){
            qDebug("No file to send.");
            return 0;
        }

        file.close();

        UsersWindow *u=new UsersWindow(scout,fileList, users,0);
        u->show();

        return 1;

}


void createFileList(QFileInfoList FileInfoList){

    for (QFileInfo fileInfo : FileInfoList){

        if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
            continue;

        // qDebug() << "Filename : " << fileInfo.absoluteFilePath() ;
        // qDebug() << "   Exists? " << fileInfo.exists();
        // qDebug() << "   Is dir?" << fileInfo.isDir();
        // qDebug() << "   Is symlink? " << fileInfo.isSymLink();
        // qDebug() << "   Read-only? " << fileInfo.isReadable();

        if (!fileInfo.exists() | !fileInfo.isReadable())
            continue;

        fileList.push_back( std::shared_ptr<QFile>( new QFile(fileInfo.absoluteFilePath())) );

        if (fileInfo.isDir()){
            // Dir:  add it to the list and recur on it
            QDir dir(fileInfo.absoluteFilePath());
            createFileList(dir.entryInfoList());
        }

    }

}
