/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Utils::HttpDownloader
  Simple Http downlader. The object must be deleted during download process.
*/

#include "httpdownloader.h"

#include <utils/global.h>
#include <utils/log.h>

#include <QProgressDialog>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QDir>
#include <QMainWindow>


using namespace Utils;

HttpDownloader::HttpDownloader(QObject *parent)
    : QObject(parent)
{
    setObjectName("HttpDownloader");
    progressDialog = new QProgressDialog;
    progressDialog->setWindowModality(Qt::WindowModal);
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
}

void HttpDownloader::setMainWindow(QMainWindow *win)
{
    if (progressDialog) {
        delete progressDialog;
        progressDialog = new QProgressDialog(win);
        progressDialog->setWindowModality(Qt::WindowModal);
        connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    }
}

void HttpDownloader::setUrl(const QUrl &url)
{
    m_Url = url;
//    m_Url.setAuthority();
}

void HttpDownloader::setOutputPath(const QString &absolutePath)
{
    if (QDir(absolutePath).exists())
        m_Path = absolutePath;
    else
        m_Path.clear();
}

void HttpDownloader::startDownload()
{
    if (!m_Url.isValid())
        return;
    if (m_Url.isEmpty())
        return;
    downloadFile();
}

void HttpDownloader::startRequest(const QUrl &url)
{
    if (!url.isValid())
        return;
    Utils::Log::addMessage(this, tr("Start downloading: %1 to %2").arg(m_Url.toString()).arg(m_Path));
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void HttpDownloader::downloadFile()
{
    QFileInfo fileInfo(m_Url.path());
    QString fileName = m_Path + QDir::separator() + fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.html";

    if (QFile::exists(fileName)) {
        if (!Utils::yesNoMessageBox(tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(fileName), ""))
            return;
        QFile::remove(fileName);
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        Utils::warningMessageBox(tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()), "");
        delete file;
        file = 0;
        return;
    }

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1\nTo %2").arg(m_Url.toString()).arg(m_Path));

    // schedule the request
    httpRequestAborted = false;
    startRequest(m_Url);
}

void HttpDownloader::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
}

void HttpDownloader::httpFinished()
{
    qWarning() << "httpFinished" << reply->error() << reply->errorString();

    if (httpRequestAborted || reply->error()!=QNetworkReply::NoError) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        progressDialog->hide();
        return;
    }

    progressDialog->hide();
    file->flush();
    file->close();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        file->remove();
        QMessageBox::information(0, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = m_Url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(0, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_Url = newUrl;
            reply->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(m_Url);
            return;
        }
    } else {
//        QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        Utils::Log::addMessage(this, tr("Downloaded %1 to current directory.").arg("file"));//.arg(fileName));
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    Q_EMIT downloadFinished();
}

void HttpDownloader::httpReadyRead()
{
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpDownloader::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;

    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}

//void HttpDownloader::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
//{
//    QDialog dlg;
//    Ui::Dialog ui;
//    ui.setupUi(&dlg);
//    dlg.adjustSize();
//    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(url.host()));

//    // Did the URL have information? Fill the UI
//    // This is only relevant if the URL-supplied credentials were wrong
//    ui.userEdit->setText(url.userName());
//    ui.passwordEdit->setText(url.password());

//    if (dlg.exec() == QDialog::Accepted) {
//        authenticator->setUser(ui.userEdit->text());
//        authenticator->setPassword(ui.passwordEdit->text());
//    }
//}

//#ifndef QT_NO_OPENSSL
//void HttpDownloader::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
//{
//    QString errorString;
//    foreach (const QSslError &error, errors) {
//        if (!errorString.isEmpty())
//            errorString += ", ";
//        errorString += error.errorString();
//    }

//    if (QMessageBox::warning(this, tr("HTTP"),
//                             tr("One or more SSL errors has occurred: %1").arg(errorString),
//                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
//        reply->ignoreSslErrors();
//    }
//}
//#endif
