#include "authdialog.h"
#include "ui_authdialog.h"

AuthDialog::AuthDialog(QWidget *parent, QSqlDatabase baseConnector) :
    QDialog(parent),
    ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
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
