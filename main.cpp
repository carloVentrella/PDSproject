#include "mainwindow.h"
#include "presettings.h"
#include <QApplication>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


int new_selection(void);

void signal_handler(int sig_no){

    if (sig_no == SIGUSR1)
        new_selection();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PreSettings s;

    // install signal handler
    signal(SIGUSR1, signal_handler);

    return a.exec();
}

int new_selection(void){

        // read selected files

        string f("/tmp/selected_files"); // should be read from a config file
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
