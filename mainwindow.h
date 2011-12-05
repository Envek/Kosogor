#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QDebug>

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase dbConnect ();
    QSqlTableModel *checksumModel, *paymentModel, *clientModel, *accountModel, *clientAccountPaymentsModel;
    QModelIndex currentChecksum;

protected slots:
    void filter_payments_by_checksum(QModelIndex index);
    void filter_payments_by_account(QModelIndex index);
    void filter_accounts_by_client(QModelIndex index);
    void on_importButton_clicked();
    void on_resetPaymentsFilterButton_clicked();
    void on_processButton_clicked();
    void on_exportButton_clicked();
    void on_syncClientsButton_clicked();
    void on_resetFilters_clicked();
};

#endif // MAINWINDOW_H
