#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void requestReceived(QNetworkReply *reply);
private slots:
    void on_actExit_triggered();

    void on_rdbGet_clicked();

    void on_rdbPost_clicked();

    void on_rdbPut_clicked();

    void on_rdbPatch_clicked();

    void on_rdbdelete_clicked();

    void on_btnExecute_clicked();

    void on_chkProxyEnabled_stateChanged(int arg1);

    void on_cbxAuth_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void initialize();
    void setSelectMethod(int index); // 0-get, 1-post, 2-put, 3-patch, 4-delete
    void Execute(QString uri);
    void loadResultBody(QNetworkReply *reply);
    void loadResultHeader(QNetworkReply *reply);
    void loadResponseStatus(QNetworkReply *reply);
    void enableProxy(bool enabled);
    void enabledAuthorization(int index); // 0 - none, 1 - simple, 2 - basic, 3 - oauth
    void loadProxyToManager(QNetworkAccessManager *manager);
};

#endif // MAINWINDOW_H
