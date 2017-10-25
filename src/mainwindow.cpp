#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <QGridLayout>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->SetMainWindow(this);
    ui->mygl->setFocus();
    ui->mygl->setFocusPolicy(Qt::StrongFocus);


    ui->consoleText->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->fpsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    QGridLayout * layout = new QGridLayout();
    this->centralWidget()->setLayout(layout);

    layout->setSpacing(0);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(0);
    layout->addWidget(ui->mygl);
    layout->setContentsMargins(0,0,0,0);

//    QMainWindow::showFullScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetFPS(float fps)
{
    this->ui->fpsLabel->setText(QString::number(fps, 'f', 2) + QString(" FPS"));
}

QTextBrowser *MainWindow::GetConsoleLabel()
{
    return ui->consoleText;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
