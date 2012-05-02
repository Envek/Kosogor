#include "authdialog.h"
#include "ui_authdialog.h"

AuthDialog::AuthDialog(QWidget *parent, QSqlDatabase baseConnector) :
    QDialog(parent),
    ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    userModel = new QSqlTableModel(this,baseConnector);
    userModel->setTable("users");
    userModel->setEditStrategy(QSqlTableModel::OnRowChange);
    userModel->select();
    userModel->setHeaderData(0, Qt::Horizontal, QString("Логин"));
    userModel->setHeaderData(1, Qt::Horizontal, QString("Пароль"));
    userModel->setHeaderData(2, Qt::Horizontal, QString("Администратор?"));

    ui->errorLabel->hide();
}

AuthDialog::~AuthDialog()
{
    delete ui;
}

void AuthDialog::accept() {
    QSqlQuery aq;
    aq.prepare("SELECT `login`, `password`, `admin` FROM `users` WHERE `login` = :login AND `password` = :password");
    aq.bindValue(":login", ui->login->text());
    aq.bindValue(":password", ui->password->text());
    if (aq.exec()) {
        qDebug() << aq.lastQuery();
        if (aq.next()) {
            this->admin = aq.value(2).toBool();
            done(Accepted);
        } else {
            ui->errorLabel->show();
        }
    }
}
