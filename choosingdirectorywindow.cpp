#include "choosingdirectorywindow.h"
#include "ui_choosingdirectorywindow.h"

ChoosingDirectoryWindow::ChoosingDirectoryWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoosingDirectoryWindow)
{
    ui->setupUi(this);

    fileSystemModel=new QFileSystemModel();

    //finding the root of the treeView
    std::string path=Settings::getInstance().getRoot();
    fileSystemModel->setRootPath(path.c_str());

    //filtering what to show (is useless to show files as we want to choose the destination directory)
    fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    ui->tree->setAnimated(false);
    ui->tree->setSortingEnabled(true);

    ui->tree->setModel(fileSystemModel);
    ui->tree->setRootIndex(fileSystemModel->index(path.c_str()));

    setWindowTitle("Select Directory");

    this->setDestination(Settings::getInstance().getDestination());

    //setting the text of the label related to the chosen destination dir (to verify what one was chosen)
    ui->directoryLabel->setText(QString::fromUtf8(this->destination.c_str()));


    QObject::connect(this, SIGNAL(destinationChanged(string)), this->parent(), SLOT(setDestination(string)));
}

ChoosingDirectoryWindow::~ChoosingDirectoryWindow()
{
    delete fileSystemModel;
    delete ui;
}


void ChoosingDirectoryWindow::on_tree_doubleClicked(const QModelIndex &index)
{
    this->setTmp(this->fileSystemModel->fileInfo(index).filePath().toStdString());

    ui->directoryLabel->setText(QString::fromUtf8(this->getTmp().c_str()));
}

std::string ChoosingDirectoryWindow::getTmp() const
{
    return tmp;
}

void ChoosingDirectoryWindow::setTmp(const std::string &value)
{
    tmp = value;
}

std::string ChoosingDirectoryWindow::getDestination() const
{
    return destination;
}

void ChoosingDirectoryWindow::setDestination(const std::string &value)
{
    destination = value;
}

void ChoosingDirectoryWindow::on_applyButton_clicked()
{
    //the definitive destination is written
    this->setDestination(this->tmp);

    //a signal is emitted to SettingsWindow
    emit destinationChanged(this->destination);
    this->close();
}

void ChoosingDirectoryWindow::on_cancelButton_clicked()
{
    this->close();
}
