#include "secondsettingswindow.h"
#include "ui_secondsettingswindow.h"

#include <QString>

SecondSettingsWindow::SecondSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondSettingsWindow)
{
    ui->setupUi(this);
    fileSystemModel=new QFileSystemModel();

    std::string path=Settings::getInstance().getRoot();

    fileSystemModel->setRootPath(path.c_str());

    fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    ui->tree->setAnimated(false);
    ui->tree->setSortingEnabled(true);
    ui->tree->setModel(fileSystemModel);
    ui->tree->setRootIndex(fileSystemModel->index(path.c_str()));

    setWindowTitle("Select Directory");

    this->setDestination(Settings::getInstance().getDestination());

    ui->directoryLabel->setText(QString::fromUtf8(this->destination.c_str()));

    QObject::connect(this, SIGNAL(destinationChanged(string)), this->parent(), SLOT(setDestination(string)));
}

SecondSettingsWindow::~SecondSettingsWindow()
{
    delete ui;
}

void SecondSettingsWindow::on_tree_doubleClicked(const QModelIndex &index)
{
    this->setTmp(this->fileSystemModel->fileInfo(index).filePath().toStdString());

    ui->directoryLabel->setText(QString::fromUtf8(this->getTmp().c_str()));
}

std::string SecondSettingsWindow::getTmp() const
{
    return tmp;
}

void SecondSettingsWindow::setTmp(const std::string &value)
{
    tmp = value;
}

std::string SecondSettingsWindow::getDestination() const
{
    return destination;
}

void SecondSettingsWindow::setDestination(const std::string &value)
{
    destination = value;
}

void SecondSettingsWindow::on_applyButton_clicked()
{
    this->setDestination(this->tmp);
    emit destinationChanged(this->destination);
    this->close();
}

void SecondSettingsWindow::on_cancelButton_clicked()
{
    this->close();
}
