#include "choosingdirectorywindow.h"
#include "ui_choosingdirectorywindow.h"

ChoosingDirectoryWindow::ChoosingDirectoryWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoosingDirectoryWindow)
{
    ui->setupUi(this);

    fileSystemModel=new QFileSystemModel();

    //finding the root of the treeView
    QString path=Settings::getInstance().getRoot();
    fileSystemModel->setRootPath(path);

    //filtering what to show (is useless to show files as we want to choose the destination directory)
    fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    ui->tree->setAnimated(false);
    ui->tree->setSortingEnabled(true);

    ui->tree->setModel(fileSystemModel);
    ui->tree->setRootIndex(fileSystemModel->index(path));

    setWindowTitle("Select Directory");

    this->setDestination(Settings::getInstance().getDestination());

    //setting the text of the label related to the chosen destination dir (to verify what one was chosen)
    ui->directoryLabel->setText(this->destination);


    QObject::connect(this, SIGNAL(destinationChanged(QString)), this->parent(), SLOT(setDestination(QString)));
}

ChoosingDirectoryWindow::~ChoosingDirectoryWindow()
{
    delete fileSystemModel;
    delete ui;
}


void ChoosingDirectoryWindow::on_tree_doubleClicked(const QModelIndex &index)
{
    this->setTmp(this->fileSystemModel->fileInfo(index).filePath());

    ui->directoryLabel->setText(this->getTmp());
}

QString ChoosingDirectoryWindow::getTmp() const
{
    return tmp;
}

void ChoosingDirectoryWindow::setTmp(const QString &value)
{
    tmp = value;
}

QString ChoosingDirectoryWindow::getDestination() const
{
    return destination;
}

void ChoosingDirectoryWindow::setDestination(const QString &value)
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
