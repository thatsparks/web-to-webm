#include "updater.h"
#include "ui_updater.h"
#include "utilities.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>

Updater::Updater(QWidget *parent) : QDialog(parent), ui(new Ui::Updater) {
    ui->setupUi(this);
    ui->plainTextEdit->setStyleSheet("QPlainTextEdit{background:#888; color: #222;}");
    addToLog("After update restart is required, so finish your work before you start.");
}

Updater::~Updater() {
    delete ui;
}

void Updater::start() {
    addToLog("Started update");
    checkForUpdates();
}

void Updater::checkForUpdates() {
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    manager->get(QNetworkRequest(QUrl("http://solusipse.net/software/web-to-webm/version.txt")));
}

void Updater::replyFinished(QNetworkReply *reply) {
    QString newestVersion = reply->readAll();
    addToLog("Connected to update server.");
    incrementProgressBar();
    addToLog("Your web-to-webm version is: " VERSION);
    if (newestVersion == VERSION) {
        addToLog("There are no updates.");
        ui->progressBar->setValue(5);
        return;
    }

    addToLog("Newest version is: " + newestVersion);
    addToLog("Downloading...");
    incrementProgressBar();

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
    manager->get(QNetworkRequest(QUrl("http://solusipse.net/software/web-to-webm/" + prepareBinaryName(newestVersion))));
}

void Updater::downloadFinished(QNetworkReply *reply) {
    addToLog("Download complete.");
    incrementProgressBar();
    QFile f(QCoreApplication::applicationDirPath() + "/update");
    if (f.open(QIODevice::WriteOnly)) {
        f.write(reply->readAll());
        f.close();
    }
    addToLog("Saved to file.");
    addToLog("Application will be restarted now.");
    qApp->exit();
}

void Updater::addToLog(QString s) {
    ui->plainTextEdit->appendPlainText(s);
}

void Updater::incrementProgressBar() {
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

QString Updater::prepareBinaryName(QString v) {
    if (SYSTEM == 0)
        return "win-web-to-webm-" + v;
    if (SYSTEM == 1)
        return "mac-web-to-webm-" + v;
    if (SYSTEM == 2)
        return "linux-web-to-webm-" + v;
}

void Updater::on_buttonStart_clicked()
{
    utils.killProcesses();
    start();
}