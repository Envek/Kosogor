#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase baseConnector = dbConnect();

    checksumModel = new QSqlTableModel(this,baseConnector);
    checksumModel->setTable("checksums");
    checksumModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    checksumModel->select();
    ui->checksumView->setModel(checksumModel);
    ui->checksumView->setSelectionBehavior(QAbstractItemView::SelectRows);
    paymentModel = new QSqlTableModel(this,baseConnector);
    paymentModel->setTable("payments");
    paymentModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    paymentModel->select();
    ui->paymentView->setModel(paymentModel);
    ui->paymentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientModel = new QSqlTableModel(this,baseConnector);
    clientModel->setTable("clients");
    clientModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    clientModel->select();
    ui->clientView->setModel(clientModel);
    ui->clientView->setSelectionBehavior(QAbstractItemView::SelectRows);
    accountModel = new QSqlTableModel(this,baseConnector);
    accountModel->setTable("accounts");
    accountModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    accountModel->select();
    ui->accountView->setModel(accountModel);
    ui->accountView->setSelectionBehavior(QAbstractItemView::SelectRows);
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
        ui->processButton->setEnabled(true);
        ui->exportButton->setEnabled(true);
        ui->resetPaymentsFilterButton->setEnabled(true);
    }
    qDebug()<<"Filter payments by checksums "<<paymentModel->filter();
}

void MainWindow::on_importButton_clicked() {
    // Переменные
    bool canContinue = false;
    QString checksumFilename;
    QFile *chksumFile, *paymentsFile;
    QString chksumBank;
    long paymentsCount;
    qint64 paymentsTotal;
    // Работа
    qDebug()<<"Importing checksums…";
    QFileDialog chksum(this,
      QString("Выберите файл контрольных сумм"),
      QString(),
      QString("Файлы контрольных сумм (*.q0*)"));
    chksum.setFileMode(QFileDialog::ExistingFile);
    if (chksum.exec())
        if (!chksum.selectedFiles().isEmpty()) {
            chksumFile = new QFile(chksum.selectedFiles().first());
            if (chksumFile->open(QFile::ReadOnly | QFile::Text))
                canContinue = true;
        }
    if (canContinue) {
        bool conversionSuccess;
        QFileInfo chksumFileInfo(*chksumFile);
        checksumFilename = chksumFileInfo.fileName();
        chksumBank    = chksumFile->read(10);
        paymentsCount = (chksumFile->read( 7).toInt(&conversionSuccess));
        paymentsTotal = (chksumFile->read(15).toLongLong(&conversionSuccess));
        if (!conversionSuccess)
            canContinue = false;
    }
    if (canContinue) {
        // Внесение в базу
        QSqlQuery query;
        if (!query.prepare("INSERT INTO checksums ("
                      "`filename`, `record_count`, `total`"
                      ") VALUES (:filename, :record_count, :total"
                      ")"))
            qDebug()<<"Query preparation failed!";
        query.bindValue(":filename", checksumFilename);
        query.bindValue(":record_count", qint32(paymentsCount));
        query.bindValue(":total", paymentsTotal);
        query.exec();
        qDebug()<<"Was executed: "<<query.lastQuery();
        qDebug()<< "Error: "<<query.lastError();
        qDebug()<<"OK!";
        qDebug()<<"Importing payments…";
    } else {
        qDebug()<<"Failed";
        return;
    }
    QString paymentsFilename = chksumFile->fileName().replace(".q0", ".v0", Qt::CaseSensitive);
    paymentsFilename = paymentsFilename.replace(".Q0", ".V0", Qt::CaseSensitive);
    paymentsFile = new QFile(paymentsFilename);
    if (paymentsFile->open(QFile::ReadOnly | QFile::Text)) {
        bool fail = false;
        int linesRead = 0;
        qint64 totalRead = 0;
        while (!paymentsFile->atEnd()) {
            QByteArray number, bank, filial;
            QByteArray sheet, sfilial, sbank, source, muszn;
            QByteArray surname, name, patronymic, account;
            qint64 pansion, mayor, additional, uvov, children, phone, total;
            QByteArray currency, created;
            bool ok = false;
            number     = paymentsFile->read( 8).trimmed();
            bank       = paymentsFile->read( 4).trimmed();
            filial     = paymentsFile->read( 5).trimmed();
            sheet      = paymentsFile->read( 3).trimmed();
            sfilial    = paymentsFile->read( 4).trimmed();
            sbank      = paymentsFile->read( 4).trimmed();
            source     = paymentsFile->read( 4).trimmed();
            muszn      = paymentsFile->read( 4).trimmed();
            surname    = paymentsFile->read(30).trimmed();
            name       = paymentsFile->read(30).trimmed();
            patronymic = paymentsFile->read(30).trimmed();
            account    = paymentsFile->read(25).trimmed();
            pansion    = (paymentsFile->read(11).toLongLong(&ok));
            mayor      = (paymentsFile->read(11).toLongLong(&ok));
            additional = (paymentsFile->read(11).toLongLong(&ok));
            uvov       = (paymentsFile->read(11).toLongLong(&ok));
            children   = (paymentsFile->read(11).toLongLong(&ok));
            phone      = (paymentsFile->read(11).toLongLong(&ok));
            total      = (paymentsFile->read(11).toLongLong(&ok));
            currency   = paymentsFile->read( 3).trimmed();
            created    = paymentsFile->read( 6).trimmed();
            paymentsFile->readLine();
            // Перекодирование строк в Unicode
            QTextCodec *codec = QTextCodec::codecForName("CP866");
            QString surname_decoded = codec->toUnicode(surname);
            QString name_decoded = codec->toUnicode(name);
            QString patronymic_decoded = codec->toUnicode(patronymic);
            // Проверки
            if (total != pansion+mayor+additional+uvov+children+phone) {
                QMessageBox::critical(this, "Ошибка в платежах", "Общая сумма платежа не сходится со слагаемыми.");
                fail = true;
            }
            if (number.isEmpty() || bank.isEmpty() || filial.isEmpty() ||
                    surname.isEmpty() || name.isEmpty() || patronymic.isEmpty() ||
                    account.isEmpty() || currency.isEmpty() || created.isEmpty()) {
                QMessageBox::critical(this, "Ошибка в платежах", "Отсутствуют необходимые поля.");
                fail = true;
            }
            totalRead += total;
            linesRead++;
            if (fail) break;
            // Внесение в базу
            QSqlQuery query;
            query.prepare("INSERT INTO payments ("
                          "`filename`, `record_number`, `bank_short`, `branch_number`, `sheet_number`, `sberbank_branch_code`, `sberbank_code`, `source_code`, `muszi_code`, `surname`, `name`, `patronymic`, `account_number`, `pansion`, `mayor_surcharge`, `additional_surcharge`, `uvov_surcharge`, `children_surcharge`, `phone_surcharge`, `total`, `currency_code`, `creation_date`"
                          ") VALUES (:filename, :record_number, :bank_short, :branch_number, :sheet_number, :sberbank_branch_code, :sberbank_code, :source_code, :muszi_code, :surname, :name, :patronymic, :account_number, :pansion, :mayor_surcharge, :additional_surcharge, :uvov_surcharge, :children_surcharge, :phone_surcharge, :total, :currency_code, :creation_date"
                          ")");
            query.bindValue(":filename", checksumFilename);
            query.bindValue(":record_number", number);
            query.bindValue(":bank_short", bank);
            query.bindValue(":branch_number", filial);
            query.bindValue(":sheet_number", sheet);
            query.bindValue(":sberbank_branch_code", sfilial);
            query.bindValue(":sberbank_code", sbank);
            query.bindValue(":source_code", source);
            query.bindValue(":muszi_code", muszn);
            query.bindValue(":surname", surname_decoded);
            query.bindValue(":name", name_decoded);
            query.bindValue(":patronymic", patronymic_decoded);
            query.bindValue(":account_number", account);
            query.bindValue(":pansion", pansion);
            query.bindValue(":mayor_surcharge", mayor);
            query.bindValue(":additional_surcharge", additional);
            query.bindValue(":uvov_surcharge", uvov);
            query.bindValue(":children_surcharge", children);
            query.bindValue(":phone_surcharge", phone);
            query.bindValue(":total", total);
            query.bindValue(":currency_code", currency);
            query.bindValue(":creation_date", created);
            query.exec();
            qDebug()<<query.lastQuery();
        }
        // Ещё проверки
        if (totalRead != paymentsTotal) {
            QMessageBox::critical(this, "Ошибка в платежах", "Общая сумма платежей не совпадает с контрольной.");
            fail = true;
        }
        if (linesRead != paymentsCount) {
            QMessageBox::critical(this, "Ошибка в платежах", "Количество платежей не совпадает с контрольным.");
            fail = true;
        }
        // Удаление всех занесённых записей
        if (fail) {
            qDebug()<<"Failed. Removing all inserted records…";
            QSqlQuery query;
            query.prepare("DELETE FROM payments WHERE filename = :filename");
            query.bindValue(":filename", checksumFilename);
            query.exec();
            qDebug()<<query.lastQuery();
            query.prepare("DELETE FROM checksums WHERE filename = :filename");
            query.bindValue(":filename", checksumFilename);
            query.exec();
            qDebug()<<query.lastQuery();
        } else {
            qDebug()<<"OK!";
        }
        // Обновление UI
        checksumModel->select();
        paymentModel->select();
    }
}

void MainWindow::on_resetPaymentsFilterButton_clicked()
{
    ui->processButton->setEnabled(false);
    ui->exportButton->setEnabled(false);
    ui->resetPaymentsFilterButton->setEnabled(false);
    paymentModel->setFilter(QString());
    ui->checksumView->clearSelection();
    ui->checksumView->clearSelection();
}

void MainWindow::on_processButton_clicked()
{

}

void MainWindow::on_exportButton_clicked()
{

}
