#include "downloader.h"

Downloader::Downloader(QObject *parent) : QObject(parent) {
	connect(&webCtrl, SIGNAL (finished(QNetworkReply*)),
			this, SLOT (finished(QNetworkReply*))
			);
	connect(&webCtrl, SIGNAL (proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator*)),
			this, SLOT (proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator*))
			);
	connect(&webCtrl, SIGNAL (authenticationRequired(QNetworkReply*, QAuthenticator*)),
			this, SLOT (authenticationRequired(QNetworkReply*, QAuthenticator*))
			);
	dlCount = 0;
}

Downloader::~Downloader() {
}

void Downloader::get(QUrl url) {
#ifdef QDEBUG_H
//	qInfo() << "proxy set:" << webCtrl.proxy().hostName() << webCtrl.proxy().port() << webCtrl.proxy().user() << webCtrl.proxy().password();
#endif
	++dlCount;
	QNetworkRequest request(url);
	QNetworkReply *reply = webCtrl.get(request);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(error(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
			this, SLOT(updateProgress(qint64, qint64)));
	connect(reply, SIGNAL(finished()),
			this, SLOT(finished()));
}

void Downloader::post(QUrl url, QByteArray &data) {
#ifdef QDEBUG_H
//	qInfo() << "proxy set:" << webCtrl.proxy().hostName() << webCtrl.proxy().port() << webCtrl.proxy().user() << webCtrl.proxy().password();
#endif
	++dlCount;
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
	QNetworkReply *reply = webCtrl.post(request, data);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(error(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
			this, SLOT(updateProgress(qint64, qint64)));
	connect(reply, SIGNAL(finished()),
			this, SLOT(finished()));
}

void Downloader::setProxy(QNetworkProxy & proxy) {
	webCtrl.setProxy(proxy);
}

QList<QNetworkProxy> Downloader::systemProxy(QUrl url) {
	QNetworkProxyQuery npq(url);
	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(npq);
	return proxies;
}

void Downloader::finished(QNetworkReply* reply) {
	downloadedData = reply->readAll();
//	emit a signal
//	reply->deleteLater();
//#ifdef QDEBUG_H
//	if (reply->error()!=QNetworkReply::NoError) {
//		qWarning() << "network error: " << reply->error() << reply->errorString();
//	}
//#endif
	--dlCount;
	emit downloaded(reply->url(), reply->error(), dlCount);
//#ifdef QDEBUG_H
//	if (!dlCount) {
//		qInfo() << "all downloads finished";
//	}
//	else if (dlCount > 1) {
//		qInfo() << dlCount << "downloads are active";
//	}
//	else {
//		qInfo() << "a download is active";
//	}
//#endif
}

QByteArray Downloader::data() const {
	return downloadedData;
}

void Downloader::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator) {
#ifdef QDEBUG_H
	qWarning() << proxy.hostName() << "requires authentification";
#endif
	Q_UNUSED(proxy);
	Q_UNUSED(authenticator);
}

void Downloader::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
	Q_UNUSED(reply);
	Q_UNUSED(authenticator);
}

void Downloader::error(QNetworkReply::NetworkError e) {
	Q_UNUSED(e);
}

void Downloader::updateProgress(qint64 read, qint64 total) {
#ifdef QDEBUG_H
//	qInfo() << "read" << read << "out of" << total;
#endif
	emit progress(read, total, dlCount);
}

void Downloader::finished() {
#ifdef QDEBUG_H
//	qInfo() << "reply finished";
#endif
}
