#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dbConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::dbConnect () {
    QSqlDatabase baseConnector = QSqlDatabase::addDatabase("QSQLITE");
    baseConnector.setDatabaseName("bugor");
    if(baseConnector.open()) {
        qDebug()<<"Connected to database";
        return 0;
    } else {
        QMessageBox msg;
        msg.setText("Не удалось подсоединится к базе данных. Проверьте права доступа.");
        msg.show();
        qDebug()<<"Not connected to database";
        return 1;
    }
}
