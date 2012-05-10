#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authdialog.h"

void setPaymentModelHeaders (QSqlTableModel *mdl) {
    mdl->setHeaderData(0, Qt::Horizontal, QString("Файл"));
    mdl->setHeaderData(1, Qt::Horizontal, QString("Запись"));
    mdl->setHeaderData(2, Qt::Horizontal, QString("Банк"));
    mdl->setHeaderData(3, Qt::Horizontal, QString("Филиал"));
    mdl->setHeaderData(4, Qt::Horizontal, QString("Страница"));
    mdl->setHeaderData(5, Qt::Horizontal, QString("Код с/б филиала"));
    mdl->setHeaderData(6, Qt::Horizontal, QString("Код с/б"));
    mdl->setHeaderData(7, Qt::Horizontal, QString("Исходный код"));
    mdl->setHeaderData(8, Qt::Horizontal, QString("Код МУСЗН"));
    mdl->setHeaderData(9, Qt::Horizontal, QString("Фамилия"));
    mdl->setHeaderData(10, Qt::Horizontal, QString("Имя"));
    mdl->setHeaderData(11, Qt::Horizontal, QString("Отчество"));
    mdl->setHeaderData(12, Qt::Horizontal, QString("Номер счёта"));
    mdl->setHeaderData(13, Qt::Horizontal, QString("Пенсия/пособие"));
    mdl->setHeaderData(14, Qt::Horizontal, QString("Доплата мэра"));
    mdl->setHeaderData(15, Qt::Horizontal, QString("Прочие доплаты"));
    mdl->setHeaderData(16, Qt::Horizontal, QString("Доплата участникам ВОВ"));
    mdl->setHeaderData(17, Qt::Horizontal, QString("Доплата на содерж. детей"));
    mdl->setHeaderData(18, Qt::Horizontal, QString("Доплата за телефон"));
    mdl->setHeaderData(19, Qt::Horizontal, QString("Общая сумма"));
    mdl->setHeaderData(20, Qt::Horizontal, QString("Валюта"));
    mdl->setHeaderData(21, Qt::Horizontal, QString("Дата создания"));
    mdl->setHeaderData(22, Qt::Horizontal, QString("Код ошибки"));
    mdl->setHeaderData(23, Qt::Horizontal, QString("К оплате?"));
    mdl->setHeaderData(24, Qt::Horizontal, QString("Дата обработки"));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase baseConnector = dbConnect();

    // Первым делом — авторизация
    AuthDialog auth_dialog(this, baseConnector);
    this->authorized = auth_dialog.exec();
    this->admin = auth_dialog.admin;

    // Непосредственная инициализация главного окна
    checksumModel = new QSqlTableModel(this,baseConnector);
    checksumModel->setTable("checksums");
    checksumModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    checksumModel->select();
    checksumModel->setHeaderData(0, Qt::Horizontal, QString("Файл"));
    checksumModel->setHeaderData(1, Qt::Horizontal, QString("Записей"));
    checksumModel->setHeaderData(2, Qt::Horizontal, QString("Сумма"));
    ui->checksumView->setModel(checksumModel);
    ui->checksumView->setSelectionBehavior(QAbstractItemView::SelectRows);
    paymentModel = new QSqlTableModel(this,baseConnector);
    paymentModel->setTable("payments");
    paymentModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    paymentModel->select();
    setPaymentModelHeaders(paymentModel);
    ui->paymentView->setModel(paymentModel);
    ui->paymentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientModel = new QSqlTableModel(this,baseConnector);
    clientModel->setTable("clients");
    clientModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    clientModel->select();
    clientModel->setHeaderData(0, Qt::Horizontal, QString("Номер паспорта"));
    clientModel->setHeaderData(1, Qt::Horizontal, QString("Фамилия"));
    clientModel->setHeaderData(2, Qt::Horizontal, QString("Имя"));
    clientModel->setHeaderData(3, Qt::Horizontal, QString("Отчество"));
    clientModel->setHeaderData(4, Qt::Horizontal, QString("Доп. счёт"));
    ui->clientView->setModel(clientModel);
    ui->clientView->setSelectionBehavior(QAbstractItemView::SelectRows);
    accountModel = new QSqlTableModel(this,baseConnector);
    accountModel->setTable("accounts");
    accountModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    accountModel->select();
    accountModel->setHeaderData(0, Qt::Horizontal, QString("Номёр счёта"));
    accountModel->setHeaderData(1, Qt::Horizontal, QString("Номер паспорта"));
    accountModel->setHeaderData(2, Qt::Horizontal, QString("Закрыт?"));
    accountModel->setHeaderData(3, Qt::Horizontal, QString("Нарушены условия?"));
    ui->accountView->setModel(accountModel);
    ui->accountView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientAccountPaymentsModel = new QSqlTableModel(this,baseConnector);
    clientAccountPaymentsModel->setTable("payments");
    clientAccountPaymentsModel->setEditStrategy(QSqlTableModel::OnRowChange);//здесь лучше в ман залезть
    clientAccountPaymentsModel->select();
    setPaymentModelHeaders(clientAccountPaymentsModel);
    currentChecksum = QModelIndex();
    ui->clientPaymentView->setModel(clientAccountPaymentsModel);
    ui->clientPaymentView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Только администратор может управлять пользователями
    if (admin) {
        userModel = new QSqlTableModel(this,baseConnector);
        userModel->setTable("users");
        userModel->setEditStrategy(QSqlTableModel::OnRowChange);
        userModel->select();
        userModel->setHeaderData(0, Qt::Horizontal, QString("Логин"));
        userModel->setHeaderData(1, Qt::Horizontal, QString("Пароль"));
        userModel->setHeaderData(2, Qt::Horizontal, QString("Администратор?"));
        ui->userView->setModel(userModel);
        ui->userView->setSelectionBehavior(QAbstractItemView::SelectRows);
    } else {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->usersTab));
        // Простым смертным базу редактировать нельзя!
        ui->checksumView->setEditTriggers(QTableView::NoEditTriggers);
        ui->paymentView->setEditTriggers(QTableView::NoEditTriggers);
        ui->clientView->setEditTriggers(QTableView::NoEditTriggers);
        ui->accountView->setEditTriggers(QTableView::NoEditTriggers);
        ui->clientPaymentView->setEditTriggers(QTableView::NoEditTriggers);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSqlDatabase MainWindow::dbConnect () {
    QSqlDatabase baseConnector = QSqlDatabase::addDatabase("QSQLITE");
    baseConnector.setDatabaseName("kosogor");
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
        currentChecksum = index;
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
    if (!canContinue) {
        qDebug()<<"No file selected or can't open the file.";
        return;
    } else {
        bool conversionSuccess;
        QFileInfo chksumFileInfo(*chksumFile);
        checksumFilename = chksumFileInfo.fileName();
        chksumBank    = chksumFile->read(10);
        paymentsCount = (chksumFile->read( 7).toInt(&conversionSuccess));
        paymentsTotal = (chksumFile->read(15).toLongLong(&conversionSuccess));
        if (!conversionSuccess)
            canContinue = false;
    }
    // Очеловечивание имени
    QRegExp rx("(\\w{4})(\\d{2})(\\d{2})");
    rx.indexIn(checksumFilename);
    QStringList hl = rx.capturedTexts();
    QString humanized_filename = QString("%1 от %2.%3").arg(hl[1],hl[2],hl[3]);
    humanized_filename.remove(QChar('_'));
    if (canContinue) {
        // Внесение в базу
        QSqlQuery query;
        if (!query.prepare("INSERT INTO checksums ("
                      "`filename`, `record_count`, `total`"
                      ") VALUES (:filename, :record_count, :total"
                      ")"))
            qDebug()<<"Query preparation failed!";
        query.bindValue(":filename", humanized_filename);
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
            // Капитализация ФИО
            surname_decoded = surname_decoded.toLower();
            surname_decoded[0] = surname_decoded[0].toUpper();
            name_decoded = name_decoded.toLower();
            name_decoded[0] = name_decoded[0].toUpper();
            patronymic_decoded = patronymic_decoded.toLower();
            patronymic_decoded[0] = patronymic_decoded[0].toUpper();
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
            query.bindValue(":filename", humanized_filename);
            query.bindValue(":record_number", number);
            query.bindValue(":bank_short", QString(bank).remove(QChar('_')));
            query.bindValue(":branch_number", QString(filial).remove(QChar('.')));
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
            query.bindValue(":filename", humanized_filename);
            query.exec();
            qDebug()<<query.lastQuery();
            query.prepare("DELETE FROM checksums WHERE filename = :filename");
            query.bindValue(":filename", humanized_filename);
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

void MainWindow::on_processButton_clicked() {
    if (currentChecksum.isValid()) {
        qDebug() << "Starting processing of the data…";
        QString reject_query = "UPDATE `payments` SET `result` = :reject_code, `paid` = 0 WHERE `filename` = :filename AND `record_number` LIKE :record";
        int paid=0, count = 0;
        QString filename = checksumModel->record(currentChecksum.row()).value("filename").toString();
        QSqlQuery notProcessed;
        notProcessed.prepare("SELECT filename, record_number, account_number, surname, name, patronymic FROM payments WHERE `filename` = :filename");
        notProcessed.bindValue(":filename", filename);
        if (notProcessed.exec()) {
            qDebug() << notProcessed.lastQuery();
            while (notProcessed.next()) {
                count++;
                QString account = notProcessed.value(2).toString();
                // Находим номер счёта, если такого нет - в расхождения.
                QSqlQuery aq;
                aq.prepare("SELECT `passport_number`, `closed`, `abuse` FROM `accounts` WHERE `account_number` = :account");
                aq.bindValue(":account", account);
                if (aq.exec()) {
                    qDebug() << aq.lastQuery();
                    if (aq.next()) {
                        if (aq.value(2).toBool()) {
                            QSqlQuery reject;
                            reject.prepare(reject_query);
                            reject.bindValue(":reject_code", 5);
                            reject.bindValue(":filename", filename);
                            reject.bindValue(":record", notProcessed.value(1).toString());
                            if (reject.exec())
                                qDebug() << reject.lastQuery();
                        } else if (aq.value(3).toBool()) {
                            QSqlQuery reject;
                            reject.prepare(reject_query);
                            reject.bindValue(":reject_code", 6);
                            reject.bindValue(":filename", filename);
                            reject.bindValue(":record", notProcessed.value(1).toString());
                            if (reject.exec())
                                qDebug() << reject.lastQuery();
                        } else {
                            // Сверяем ФИО, если не совпадают - в расхождения
                            QSqlQuery cl;
                            cl.prepare("SELECT surname, name, patronymic FROM clients WHERE passport_number = :passport");
                            cl.bindValue(":passport", aq.value(0).toString());
                            if (cl.exec()) {
                                qDebug() << cl.lastQuery();
                                if (cl.next()) {
                                    if (!(
                                            cl.value(0).toString().trimmed().toLower() == notProcessed.value(3).toString().trimmed().toLower() &&
                                            cl.value(1).toString().trimmed().toLower() == notProcessed.value(4).toString().trimmed().toLower() &&
                                            cl.value(2).toString().trimmed().toLower() == notProcessed.value(5).toString().trimmed().toLower()
                                            )) {
                                        QSqlQuery reject;
                                        reject.prepare(reject_query);
                                        reject.bindValue(":reject_code", 3);
                                        reject.bindValue(":filename", filename);
                                        reject.bindValue(":record", notProcessed.value(1).toString());
                                        if (reject.exec())
                                            qDebug()<<reject.lastQuery();
                                    } else {
                                        // Вот теперь можно отметить как оплаченное
                                        QSqlQuery approve;
                                        approve.prepare("UPDATE `payments` SET `paid` = 1 WHERE `filename` = :filename AND `record_number` LIKE :record");
                                        approve.bindValue(":filename", notProcessed.value(0).toString());
                                        approve.bindValue(":record", notProcessed.value(1).toString());
                                        if (approve.exec()) {
                                            qDebug()<<approve.lastQuery();
                                            paid++;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        QSqlQuery reject;
                        reject.prepare(reject_query);
                        reject.bindValue(":reject_code", 4);
                        reject.bindValue(":filename", filename);
                        reject.bindValue(":record", notProcessed.value(1));
                        if (reject.exec())
                            qDebug() << reject.lastQuery();
                    }
                    paymentModel->select();
                }
            }
        }
        QMessageBox::information(this, "Обработка завершена", QString("Обработано записей: %1\nК оплате: %2\nРасхождений: %3").arg(count).arg(paid).arg(count-paid));
    }
}

void MainWindow::on_exportButton_clicked() {
    if (currentChecksum.isValid()) {
        QString filename = checksumModel->record(currentChecksum.row()).value("filename").toString();
        QFileDialog chksum(this, QString("Выберите место сохранения файлов"));
        chksum.setFileMode(QFileDialog::Directory);
        if (chksum.exec())
            if (!chksum.selectedFiles().isEmpty()) {
                QDir savePlace (chksum.selectedFiles().first());
                QString dFilename = filename.replace(".q0", ".d0", Qt::CaseSensitive);
                dFilename = dFilename.replace(".Q0", ".D0", Qt::CaseSensitive);
                QString eFilename = filename.replace(".q0", ".e0", Qt::CaseSensitive);
                eFilename = eFilename.replace(".Q0", ".E0", Qt::CaseSensitive);
                QString fFilename = filename.replace(".q0", ".f0", Qt::CaseSensitive);
                fFilename = fFilename.replace(".Q0", ".F0", Qt::CaseSensitive);
                QString nFilename = filename.replace(".q0", ".n0", Qt::CaseSensitive);
                nFilename = nFilename.replace(".Q0", ".N0", Qt::CaseSensitive);
                QString rFilename = filename.replace(".q0", ".r0", Qt::CaseSensitive);
                rFilename = rFilename.replace(".Q0", ".R0", Qt::CaseSensitive);
                QFile dFile(savePlace.absoluteFilePath(dFilename));
                QFile eFile(savePlace.absoluteFilePath(eFilename));
                QFile fFile(savePlace.absoluteFilePath(fFilename));
                QFile nFile(savePlace.absoluteFilePath(nFilename));
                QFile rFile(savePlace.absoluteFilePath(rFilename));
                if (dFile.open(QFile::WriteOnly | QFile::Text)) dFile.close();
                if (eFile.open(QFile::WriteOnly | QFile::Text)) eFile.close();
                if (fFile.open(QFile::WriteOnly | QFile::Text)) fFile.close();
                if (nFile.open(QFile::WriteOnly | QFile::Text)) nFile.close();
                if (rFile.open(QFile::WriteOnly | QFile::Text)) rFile.close();
                QMessageBox::information(this, "Вывод файлов сделан", QString("Файлы результатов сохранены в директорию %1").arg(savePlace.absolutePath()));
            }
    }
}

void MainWindow::on_syncClientsButton_clicked() {
    QTimer *a = new QTimer(this);
    a->setInterval(5000);
    a->setSingleShot(true);
    a->start();
    connect(a, SIGNAL(timeout()), this, SLOT(fakeTimerFire()));
    ui->syncClientsButton->setEnabled(false);
}

void MainWindow::fakeTimerFire() {
    QMessageBox::warning(this, "Синхронизация не удалась", QString("Не удаётся подсоединиться к мастер-серверу."));
    ui->syncClientsButton->setEnabled(true);
}

void MainWindow::on_resetFilters_clicked()
{
    ui->resetFilters->setEnabled(false);
    clientModel->setFilter(QString());
    accountModel->setFilter(QString());
    clientAccountPaymentsModel->setFilter(QString());
    ui->clientView->clearSelection();
    ui->accountView->clearSelection();
    ui->clientPaymentView->clearSelection();
}

void MainWindow::filter_payments_by_account(QModelIndex index) {
    if (index.isValid()) {
        QString account = accountModel->record(index.row()).value("account_number").toString();
        clientAccountPaymentsModel->setFilter(QString("`account_number` LIKE '%1'").arg(account));
        clientAccountPaymentsModel->select();
        ui->resetFilters->setEnabled(true);
    }
    qDebug()<<"Filter payments by account "<<paymentModel->filter();
}

void MainWindow::filter_accounts_by_client(QModelIndex index) {
    if (index.isValid()) {
        QString account = clientModel->record(index.row()).value("passport_number").toString();
        accountModel->setFilter(QString("`passport_number` = '%1'").arg(account));
        accountModel->select();
        ui->resetFilters->setEnabled(true);
    }
    qDebug()<<"Filter accounts by client "<<paymentModel->filter();
}

void MainWindow::on_addUserButton_clicked() {
    bool ok = false;
    QString text = QInputDialog::getText(this, "Новый пользователь",
                                         "Введите логин (имя входа) нового пользователя",
                                         QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        QSqlQuery query;
        query.prepare("INSERT INTO users (`login`, `password`, `admin`) VALUES (:login, '', 0)");
        query.bindValue(":login", text);
        query.exec();
        qDebug()<<query.lastQuery();
    }
    userModel->select();
}

void MainWindow::on_deleteUserButton_clicked() {
    QModelIndex index = ui->userView->currentIndex();
    if (index.isValid()) {
        QString login = userModel->record(index.row()).value("login").toString();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Удаление пользователя"),
                                        tr("Вы действительно хотите удалить пользователя «%1»?").arg(login),
                                        QMessageBox::Yes | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            QSqlQuery query;
            query.prepare("DELETE FROM users WHERE `login` = :login");
            query.bindValue(":login", login);
            query.exec();
            qDebug()<<query.lastQuery();
            ui->deleteUserButton->setEnabled(false);
        }
    }
    userModel->select();
}

void MainWindow::user_selected(QModelIndex index) {
    if (index.isValid()) {
        ui->deleteUserButton->setEnabled(true);
    }
}
