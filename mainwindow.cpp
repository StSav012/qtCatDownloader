#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	loadSettings();
	connect(&dl, SIGNAL (downloaded(QUrl, QNetworkReply::NetworkError, unsigned int)), this, SLOT (downloaded(QUrl, QNetworkReply::NetworkError, unsigned int)));
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete ui;
}

void MainWindow::loadSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatDownloader");
	s.beginGroup("connection");
	ui->text_url->setText(s.value("url", ui->text_url->text()).toString());
	s.beginGroup("proxy");
	QUrl url(ui->text_url->text());
	QNetworkProxy proxy;
	if (url.isValid()) {
		QList<QNetworkProxy> proxies = dl.systemProxy(url);
		foreach (proxy, proxies) {
			qInfo() << "detected proxy:" << proxy.hostName() << proxy.port() << proxy.user();
		}
		if (!proxies.isEmpty()) {
			proxy = proxies.at(0);
		}
	}
	ui->box_proxy->setChecked(s.value("use", proxy.type() != QNetworkProxy::NoProxy).toBool());
	ui->text_proxy_host->setText(s.value("host", proxy.hostName()).toString());
	ui->spin_proxy_port->setValue(s.value("port", proxy.port()).toInt());
	ui->text_proxy_user->setText(s.value("user", proxy.user()).toString());
	ui->text_proxy_pwd->setText(proxy.password());
	s.endGroup();
	s.endGroup();
	s.beginGroup("frequency");
	ui->spin_frrange_from->setValue(s.value("from", ui->spin_frrange_from->value()).toDouble());
	ui->spin_frrange_to->setValue(s.value("to", ui->spin_frrange_to->value()).toDouble());
	s.endGroup();
	s.beginGroup("saving");
	ui->text_saveTo->setText(s.value("location", "catalog.qbjsz").toString());
	s.endGroup();
	s.beginGroup("window");
	QDesktopWidget* desktop = QApplication::desktop();
	this->move(0.5*(desktop->width()-this->size().width()),
			   0.5*(desktop->height()-this->size().height()));		// Fallback: Center the window
	QVariant window;
	window = s.value("geometry");
	if (!window.isNull()) {
		restoreGeometry(window.toByteArray());
	}
	window = s.value("state");
	if (!window.isNull()) {
		restoreState(window.toByteArray());
	}
	s.endGroup();
}

void MainWindow::saveSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatDownloader");
	s.beginGroup("connection");
	s.setValue("url", ui->text_url->text());
	s.beginGroup("proxy");
	s.setValue("use", ui->box_proxy->isChecked());
	s.setValue("host", ui->text_proxy_host->text());
	s.setValue("port", ui->spin_proxy_port->value());
	s.setValue("user", ui->text_proxy_user->text());
	s.endGroup();
	s.endGroup();
	s.beginGroup("frequency");
	s.setValue("from", ui->spin_frrange_from->value());
	s.setValue("to", ui->spin_frrange_to->value());
	s.endGroup();
	s.beginGroup("saving");
	s.setValue("location", ui->text_saveTo->text());
	s.endGroup();
	s.beginGroup("window");
	s.setValue("geometry", saveGeometry());
	s.setValue("state", saveState());
	s.endGroup();
}

void MainWindow::on_button_download_clicked()
{
	ui->button_download->setDisabled(true);
	// cleanup
	while (!speciesList.isEmpty()) {
		speciesList.removeLast();
	}
	while (!catalogData.isEmpty()) {
		catalogData.removeLast();
	}
	/*
	 * curl
	 *		'http://cdms.ph1.uni-koeln.de/cdms/portal/json_list/species/'
	 *		--1.0
	 *		-H 'Host: cdms.ph1.uni-koeln.de'
	 *		-H 'User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0'
	 *		-H 'Accept: application/json, text/javascript, * / *; q=0.01'
	 *		-H 'Accept-Language: en-US,en;q=0.5'
	 *		--compressed
	 *		-H 'Content-Type: application/x-www-form-urlencoded; charset=UTF-8'
	 *		-H 'X-Requested-With: XMLHttpRequest'
	 *		-H 'Referer: http://cdms.ph1.uni-koeln.de/cdms/portal/queryForm'
	 *		-H 'Proxy-Authorization: Basic YW5mZXJ0ZXY6NzM5NTgwNjEy'
	 *		-H 'Connection: keep-alive'
	 *		--data 'database=-1'
	*/
	//qDebug() << "trying to download";
	QUrl url(ui->text_url->text());
	if (!url.isValid()) {
		qWarning() << "invalid URL" << url.path();
		return;
	}
	QNetworkProxy proxy;
	if (ui->box_proxy->isChecked()) {
		proxy.setHostName(ui->text_proxy_host->text());
		proxy.setPort(ui->spin_proxy_port->value());
		proxy.setUser(ui->text_proxy_user->text());
		proxy.setPassword(ui->text_proxy_pwd->text());
		proxy.setType(QNetworkProxy::HttpProxy);
	}
	else {
		proxy.setType(QNetworkProxy::NoProxy);
	}
	dl.setProxy(proxy);
	QByteArray data;
	data.append("database=-1");
	dl.post(url, data);
//	qDebug() << "waiting";
}

void MainWindow::downloaded(QUrl url, QNetworkReply::NetworkError error, unsigned int filesLeft) {
	static int dlCount = 0;
	//qDebug() << "received" << dl.data().length();
	if (url == QUrl(ui->text_url->text())) {
//		QFile f("species.json");
//		f.open(QFile::WriteOnly);
//		f.write(dl.data());
//		f.close();
//		qInfo() << "received" << url;
		processSpeciesList(dl.data());
	}
	else if (url.fileName().endsWith(QString(".cat"))) {
		if (error != QNetworkReply::NoError) { // html page of an error
			qWarning() << url.url() << "returned error:" << error;
		}
		else {
//			qInfo() << url.fileName() << "received";
			processCatalogFile(url.fileName(), dl.data());
		}
		++dlCount;
	}
	if (dlCount == speciesListLength) {
		dlCount = 0;
		downloadedAll();
	}
	else {
		ui->statusBar->showMessage(tr("%1 of %2 files left to download")
								   .arg(filesLeft)
								   .arg(speciesListLength));
	}
}

void MainWindow::downloadedAll() {
	ui->statusBar->showMessage(tr("All downloads finished. Saving the result…"));
	QString fileName = ui->text_saveTo->text();
	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		qWarning() << "couldn't open the file to save" << fileName;
		ui->statusBar->showMessage(tr("Could not open the file to save."));
		return;
	}
	QJsonObject catalog;
	catalog.insert("catalog", catalogData);
	if (fileName.endsWith(QString(".qbjsz"))) {
		f.write(qCompress(QJsonDocument(catalog).toBinaryData()));
	}
	else if (fileName.endsWith(QString(".qbjs"))) {
		f.write(QJsonDocument(catalog).toBinaryData());
	}
	else {
		f.write(QJsonDocument(catalog).toJson());
	}
	ui->statusBar->showMessage(tr("All downloads finished. The result is stored."));
	QMessageBox::information(this, ui->centralWidget->windowTitle(), tr("All downloads finished. The result is stored."));
	ui->button_download->setEnabled(true);
}

void MainWindow::processSpeciesList(const QByteArray & text) {
	// qDebug() << "processing" << text.length();
	QJsonParseError err;
	QJsonDocument json = QJsonDocument::fromJson(text, &err);
	if (json.isNull()) {
		qWarning() << "JSON error:" << err.errorString();
		return;
	}
	QJsonObject root = json.object();
	QJsonValue species = root.value("species");
	if (species.isArray()) {
		speciesList = species.toArray();
		speciesListLength = speciesList.size();
//		qInfo() << speciesListLength << "catalog files to download";
		for (int i = 0; i < speciesListLength; ++i) {
			int num = speciesList.at(i).toObject().value("speciestag").toDouble();
			QString fn;
			fn = QString("c%1.cat").arg(num, 6, 10, QChar('0'));
			if (num%1000 > 500) {
				fn = QString("http://www.astro.uni-koeln.de/site/vorhersagen/catalog/") + fn;
			}
			else {
				fn = QString("https://spec.jpl.nasa.gov//ftp//pub/catalog/") + fn;
			}
			QUrl url(fn);
			if (url.isValid()) {
				dl.get(url);
			}
			else {
				qWarning() << "invalid URL:" << fn;
				--speciesListLength;
			}
		}
	}
}

void MainWindow::processCatalogFile(const QString &fileName, const QByteArray &text) {
	QStringList catalogLinesList = QString(text).split('\n', QString::SkipEmptyParts);
	QJsonArray catalogLinesArray;
	double fmin = ui->spin_frrange_from->value();
	double fmax = ui->spin_frrange_to->value();
	qint8 dr = -1;
	foreach (QString catalogLine, catalogLinesList) {
		CatalogEntry entry = CatalogEntry(catalogLine);
		if (entry.frequency() >= fmin && entry.frequency() <= fmax) {
			catalogLinesArray.append(QJsonValue(entry.toJsonObject()));
			dr = entry.degreesOfFreedom();
		}
	}
	if (!catalogLinesArray.isEmpty()) {
		QJsonObject catalogEntry;
		int tag = fileName.mid(1, 6).toInt();
		for (int i = 0; i < speciesList.size(); ++i) {
			int num = speciesList.at(i).toObject().value("speciestag").toDouble();
			if (tag == num) {
				catalogEntry.insert("speciestag", QJsonValue(tag));
//				catalogEntry.insert("moleculesymbol", speciesList.at(i).toObject().value("moleculesymbol"));
				catalogEntry.insert("name", speciesList.at(i).toObject().value("name"));
				catalogEntry.insert("stoichiometricformula", speciesList.at(i).toObject().value("stoichiometricformula"));
				catalogEntry.insert("structuralformula", speciesList.at(i).toObject().value("structuralformula"));
				catalogEntry.insert("trivialname", speciesList.at(i).toObject().value("trivialname"));
				catalogEntry.insert("degreesoffreedom", dr);
				catalogEntry.insert("lines", QJsonValue(catalogLinesArray));
				catalogData.append(QJsonValue(catalogEntry));
				break;
			}
		}
	}
}

void MainWindow::on_spin_frrange_from_valueChanged(double arg1) {
	ui->spin_frrange_to->setMinimum(arg1);
}

void MainWindow::on_spin_frrange_to_valueChanged(double arg1) {
	ui->spin_frrange_from->setMaximum(arg1);
}

void MainWindow::on_button_browse_clicked() {
	static QString saveToFileName = ui->text_saveTo->text();
	QFileDialog saveDialog(this);
	saveDialog.setFileMode(QFileDialog::AnyFile);
	saveDialog.setAcceptMode(QFileDialog::AcceptSave);
	QStringList extensions = QStringList() << QString(".qbjsz") << QString(".qbjs") << QString(".json");
	QStringList filters = QStringList() << tr("Compressed Qt Binary JSON") + QString(" (*%1)").arg(extensions.at(0))
										<< tr("Qt Binary JSON") + QString(" (*%1)").arg(extensions.at(1))
										<< tr("JSON") + QString(" (*%1)").arg(extensions.at(2));
	saveDialog.setNameFilters(filters);
	saveDialog.setDirectory(saveToFileName);
	if (saveDialog.exec()) {
		saveToFileName = saveDialog.selectedFiles().first();
		QString ext = extensions.at(filters.indexOf(saveDialog.selectedNameFilter()));
		if (!saveToFileName.endsWith(ext)) {
			saveToFileName.append(ext);
		}
		ui->text_saveTo->setText(saveToFileName);
	}
}
