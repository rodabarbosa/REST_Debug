#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actExit_triggered()
{
    this->close();
}

void MainWindow::initialize(){
    ui->centralWidget->setLayout(ui->gridLayout);
    ui->gridLayout->setAlignment(Qt::AlignTop);
    ui->gridLayout->setMargin(15);

    ui->tab_body->setLayout(ui->layResultBody);
    ui->tab_header->setLayout(ui->layResultHeader);

    ui->tabContentAuthorization->setLayout(ui->layContentAuthorization);
    ui->layContentAuthorization->setAlignment(Qt::AlignTop);
    ui->tabContentBody->setLayout(ui->layContentBody);
    ui->layContentAuthorization->setAlignment(Qt::AlignTop);
    ui->tabContentProxy->setLayout(ui->layContentProxy);
    ui->layContentProxy->setAlignment(Qt::AlignTop);
    ui->tabContentHeader->setLayout(ui->layContentHeader);
    ui->layContentHeader->setAlignment(Qt::AlignTop);

    ui->tabContent->setCurrentIndex(0);
    this->setSelectMethod(0);
    this->enableProxy(ui->chkProxyEnabled->isChecked());
    this->enabledAuthorization(ui->cbxAuth->currentIndex());
}

void MainWindow::setSelectMethod(int index)
{
    /*
     * GET and DELETE won't allow to send body content
     * So the content entry will be disabled
     */
    ui->rdbGet->setChecked(index == 0 || index > 4);
    ui->rdbPost->setChecked(index == 1);
    ui->rdbPut->setChecked(index == 2);
    ui->rdbPatch->setChecked(index == 3);
    ui->rdbdelete->setChecked(index == 4);

    if (ui->rdbGet->isChecked() || ui->rdbdelete->isChecked()) {
        ui->txtRequestContent->setEnabled(false);
    } else {
        ui->txtRequestContent->setEnabled(true);
    }
}

void MainWindow::on_rdbGet_clicked()
{
    this->setSelectMethod(0);
}

void MainWindow::on_rdbPost_clicked()
{
    this->setSelectMethod(1);
}

void MainWindow::on_rdbPut_clicked()
{
    this->setSelectMethod(2);
}

void MainWindow::on_rdbPatch_clicked()
{
    this->setSelectMethod(3);
}

void MainWindow::on_rdbdelete_clicked()
{
    this->setSelectMethod(4);
}

void MainWindow::on_btnExecute_clicked()
{
    QString uri = ui->txtUri->toPlainText();
    if (uri.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Warning", "URI Address missing.");
    } else {
        this->Execute(uri.trimmed());
    }
}

void MainWindow::Execute(QString uri) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));

    if (ui->chkProxyEnabled->isChecked()) {
        this->loadProxyToManager(manager);
    }

    if (ui->rdbGet->isChecked() || ui->rdbdelete->isChecked()) {
        manager->get(QNetworkRequest(QUrl(uri)));
    } else {
        QString content = ui->txtRequestContent->toPlainText();
        QByteArray data(content.toUtf8());

        if (ui->rdbPut->isChecked()) {
            manager->put(QNetworkRequest(QUrl(uri)), data);
        } else {
            manager->post(QNetworkRequest(QUrl(uri)), data);
        }
    }
    delete manager;
}

void MainWindow::loadProxyToManager(QNetworkAccessManager *manager) {
    QNetworkProxy proxy ;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy->setHostName(ui->txtProxyAddress->getText());
    proxy->setPort(ui->spnProxyPort->getValue());
    proxy->setUser(ui->txtProxyUsername->getText());
    proxy->setPassword(ui->txtProxyPassword->getText());

    manager->setProxy(proxy);
}

void MainWindow::requestReceived(QNetworkReply* reply) {
    ui->txtResultHeader->clear();
    ui->txtResultBody->clear();

    this->loadResponseStatus(reply);
    this->loadResultBody(reply);
    this->loadResultHeader(reply);
}

void MainWindow::loadResponseStatus(QNetworkReply* reply) {
   ui->lbl_status->setText('Status: ' + reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toString() + " - " + (reply->attribute(QNetworkRequest::Attribute::HttpReasonPhraseAttribute).toString()));
}

void MainWindow::loadResultBody(QNetworkReply* reply) {
    QString content = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    ui->txtResultBody->appendPlainText(doc.toJson(QJsonDocument::Indented));
}

void MainWindow::loadResultHeader(QNetworkReply* reply) {
    QList<QByteArray> headerlist = reply->rawHeaderList();
    QByteArray header;
    QString headerLine;
    for (auto it = headerlist.cbegin(); it != headerlist.cend(); ++it) {
        header = reply->rawHeader(*it);
        headerLine = *it + ": " + header;
        ui->txtResultHeader->appendPlainText(headerLine);
    }
}

void MainWindow::on_chkProxyEnabled_stateChanged(int arg1)
{
    this->enableProxy(ui->chkProxyEnabled->isChecked());
}

void MainWindow::enableProxy(bool enabled) {
   ui->txtProxyAddress->setEnabled(enabled);
   ui->spnProxyPort->setEnabled(enabled);
   ui->txtProxyPassword->setEnabled(enabled);
   ui->txtProxyUsername->setEnabled(enabled);
}

void MainWindow::on_cbxAuth_currentIndexChanged(const QString &arg1)
{
    this->enabledAuthorization(ui->cbxAuth->currentIndex());
}

void MainWindow::enabledAuthorization(int index) {
    ui->txtAuthUsername->setEnabled(index != 0);
    ui->txtAuthUsernameOAuth->setEnabled(index == 3);
    ui->txtAuthPassword->setEnabled(index != 0);
    ui->txtAuthPasswordOAuth->setEnabled(index == 3);
}
