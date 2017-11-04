#include "settingswindow.h"
#include "settings.h"
#include "tcptransferserver.h"
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

#include "tcpthumbserver.h"

int new_selection(void);
void createFileList(QFileInfoList FileInfoList);

// list of neighbours
shared_ptr<Users> users;
// scout
shared_ptr<discovery> scout;
QList<std::shared_ptr<QFile>> fileList;


void signal_handler(int sig_no){

    if (sig_no == SIGUSR1)
        new_selection();
}

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

    QHostAddress serverAddr("127.0.0.1");
    qint16 serverPort = 5555;

    // The scout is in charge of handling the user list.
    // It sends and receives advertisements
    scout = discovery::getInstance(groupAddress,port, users);

    Server tcpServer(serverAddr, serverPort);
    tcpServer.start();

    // start TCP thumb server
    ThumbServer server(QHostAddress::AnyIPv4,serverPort+1,users);
    server.start();

    return a.exec();
}

int new_selection(void){

        // read selected files
        // should be read from a config file
        string f("/tmp/selected_files");
        vector<string> selected;
        fileList.clear();

        ifstream file(f, ifstream::in);

        if (!file){
                cerr << "Can't retrieve selected file list: can't open file." << endl;
                return 0;
        }

        int i, n_selected_files;
        string tmp;

        getline(file,tmp);
        n_selected_files = stoi(tmp);
        cout << "files present in selected: " << selected.size() << endl;
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

        for (auto x : fileList)
            cout << x->fileName().toStdString() << endl;

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
