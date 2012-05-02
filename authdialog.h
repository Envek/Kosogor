#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AuthDialog(QWidget *parent, QSqlDatabase baseConnector);
    ~AuthDialog();
    bool admin;
    
private:
    Ui::AuthDialog *ui;
    QSqlTableModel *userModel;
    void accept();

};

#endif // AUTHDIALOG_H
