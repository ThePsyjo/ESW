#include "webDoc.h"

WebDoc::WebDoc(QString u)
{
	buf  = new QBuffer(this);
	buf->open(QBuffer::ReadWrite);
	http = new QHttp(this);
	url = new QUrl(u);
	connect(http, SIGNAL(done(bool)), this, SLOT(httpGetDone(bool)));
	doc = new QDomDocument;
}

WebDoc::~WebDoc(){};

void WebDoc::get(QString urlargs)
{
	buf->reset();
	buf->buffer().clear();
	http->setHost(url->host());
	http->get(url->path() + urlargs, buf);
}

void WebDoc::httpGetDone(bool error)
{
bool ok = true;
	if(error)
	{
		QMessageBox::warning(0, tr("download error"),tr("error while downloading %3.\npage: \"%1\"\n\"%2\"")
								.arg(url->host() + url->path())
								.arg(http->errorString())
								.arg(url->toString()));
		ok = false;
	}

//puts("----------");
//qDebug() << buf->data();
//puts("----------");

	QString errorStr;
	int errorLine;
	int errorColumn;
        if (!doc->setContent(buf->data(), true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::warning(0, tr("parse error"),
		tr("Parse error in %4\nat line %1, column %2:\n\"%3\"")
		.arg(errorLine)
		.arg(errorColumn)
		.arg(errorStr)
		.arg(url->toString()));
		ok = false;
	}

	emit done(ok);
}

QDomDocument* WebDoc::document()
{	return doc;	}
