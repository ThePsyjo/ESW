#ifndef WEBDOC_H
#define WEBDOC_H

#include <QtXml>
#include <QMessageBox>
#include <QBuffer>
#include <QtNetwork/QHttp>
#include <QWidget>

class WebDoc : public QObject
{
Q_OBJECT
public:
	WebDoc(QString);
	virtual ~WebDoc();
	void get(QString);
	QDomDocument* document();
private:
	QHttp *http;
	QUrl *url;
	QBuffer *buf;
	QDomDocument *doc;
private slots:
	void httpGetDone(bool);
signals:
	void done();
};

#endif

