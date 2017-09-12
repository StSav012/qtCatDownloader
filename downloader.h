#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QMessageBox>
#include <QInputDialog>

class Downloader : public QObject
{
	Q_OBJECT
public:
	explicit Downloader(QObject *parent = 0);
	virtual ~Downloader();
	QByteArray data() const;
	void get(QUrl url);
	void post(QUrl url, QByteArray &data);
	QList<QNetworkProxy> systemProxy(QUrl url);
	void setProxy(QNetworkProxy & proxy);

signals:
	void downloaded(QUrl url, QNetworkReply::NetworkError error, unsigned int filesLeft);
	void progress(qint64 bytesRead, qint64 bytesTotal, unsigned int filesLeft);

private slots:
	void finished(QNetworkReply* reply);
	void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
	void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
	void error(QNetworkReply::NetworkError e);
	void updateProgress(qint64 read, qint64 total);
	void finished();

private:
	QNetworkAccessManager webCtrl;
	QByteArray downloadedData;
	unsigned int dlCount;
};

#endif // DOWNLOADER_H
