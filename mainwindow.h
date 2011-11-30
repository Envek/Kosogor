#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>

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
    QSqlTableModel *checksumModel, *paymentModel;

protected slots:
    void filter_payments_by_checksum(QModelIndex index);

};

#endif // MAINWINDOW_H
