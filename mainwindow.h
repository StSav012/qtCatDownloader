#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>
#include <QDesktopWidget>
#include <QDebug>

#include "downloader.h"
#include "catalogentry.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void processSpeciesList(const QByteArray &text);

	void processCatalogFile(const QString &fileName, const QByteArray &text);

	void loadSettings();
	void saveSettings();

	void downloadedAll();

private slots:
	void on_button_download_clicked();

	void downloaded(QUrl url, QNetworkReply::NetworkError error, unsigned int filesLeft);

	void on_spin_frrange_from_valueChanged(double arg1);

	void on_spin_frrange_to_valueChanged(double arg1);

	void on_button_browse_clicked();

private:
	Ui::MainWindow *ui;

	Downloader dl;

	QJsonArray speciesList;
	int speciesListLength;

	QJsonArray catalogData;
};

#endif // MAINWINDOW_H
