#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase baseConnector = dbConnect();

    checksumModel = new QSqlTableModel(this,baseConnector);
    checksumModel->setTable("checksums");
    checksumModel->setEditStrategy(QSqlTableModel::OnFieldChange);//здесь лучше в ман залезть
    checksumModel->select();
    ui->checksumView->setModel(checksumModel);
    paymentModel = new QSqlTableModel(this,baseConnector);
    paymentModel->setTable("payments");
    paymentModel->setEditStrategy(QSqlTableModel::OnFieldChange);//здесь лучше в ман залезть
    paymentModel->select();
    ui->paymentView->setModel(paymentModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSqlDatabase MainWindow::dbConnect () {
    QSqlDatabase baseConnector = QSqlDatabase::addDatabase("QSQLITE");
    baseConnector.setDatabaseName("bugor");
    if(baseConnector.open()) {
        qDebug()<<"Connected to database";
    } else {
        QMessageBox msg;
        msg.setText("Не удалось подсоединится к базе данных. Проверьте права доступа.");
        msg.show();
        qDebug()<<"Not connected to database";
    }
    return baseConnector;
}

void MainWindow::filter_payments_by_checksum(QModelIndex index) {
    if (index.isValid()) {
        QString filename = checksumModel->record(index.row()).value("filename").toString();
        paymentModel->setFilter(QString("`filename` = '%1'").arg(filename));
        paymentModel->select();
    }
    qDebug()<<"Filter payments by checksums "<<paymentModel->filter();
}
