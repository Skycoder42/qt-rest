#include "restreply.h"
#include <optional>
#include <QtCore/QThread>
using namespace QtRest;

Q_LOGGING_CATEGORY(QtRest::logReply, "qtrest.RestReply", QtInfoMsg)

namespace QtRest {

void deleteReplyLater(QNetworkReply *reply)
{
	QMetaObject::invokeMethod(reply, "deleteLater", Qt::AutoConnection);
}

class RestReplyData : public QSharedData
{
public:
	QSharedPointer<QNetworkReply> reply;

	mutable std::optional<int> statusCode = std::nullopt;
	mutable std::optional<qint64> contentLength = std::nullopt;
	mutable std::optional<QByteArray> contentType = std::nullopt;
	mutable QTextCodec *contentCodec = nullptr;

	void parseContentType();
};

}

RawRestReply::RawRestReply(QNetworkReply *reply) :
	d{new RestReplyData{}}
{
	d->reply = QSharedPointer<QNetworkReply>{reply, deleteReplyLater};
}

RawRestReply::RawRestReply(const RawRestReply &other) = default;

RawRestReply::RawRestReply(RawRestReply &&other) noexcept = default;

RawRestReply &RawRestReply::operator=(const RawRestReply &other) = default;

RawRestReply &RawRestReply::operator=(RawRestReply &&other) noexcept = default;

RawRestReply::~RawRestReply() = default;

bool RawRestReply::hasHeader(const QLatin1String &name) const
{
	return d->reply->hasRawHeader(name.latin1());
}

QString RawRestReply::header(const QLatin1String &name) const
{
	return QString::fromLatin1(d->reply->rawHeader(name.latin1()));
}

QVariant RawRestReply::attribute(QNetworkRequest::Attribute attribute) const
{
	return d->reply->attribute(attribute);
}

bool RawRestReply::hasBody()
{
	if (statusCode() == 204)  // NO_CONTENT
		return false;
	if (contentLength() > 0)
		return true;
	else
		return d->reply->bytesAvailable() > 0 || d->reply->pos() > 0;
}

QIODevice *RawRestReply::bodyDevice() const
{
	return d->reply.data();
}

QByteArray RawRestReply::bodyData()
{
	return d->reply->readAll();
}

QString RawRestReply::bodyString()
{
	if (const auto codec = contentCodec(); codec)
		return codec->toUnicode(bodyData());
	else
		return QString::fromUtf8(bodyData());
}

bool RawRestReply::wasSuccessful() const
{
	return error() == QNetworkReply::NoError && statusCode() < 300;
}

int RawRestReply::statusCode() const
{
	if (!d->statusCode)
		d->statusCode = d->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	return *d->statusCode;
}

QNetworkReply::NetworkError RawRestReply::error() const
{
	return d->reply->error();
}

QByteArray RawRestReply::contentType() const
{
	if (!d->contentType)
		d->parseContentType();
	return *d->contentType;
}

QTextCodec *RawRestReply::contentCodec() const
{
	if (!d->contentType)
		d->parseContentType();
	return d->contentCodec;
}

qint64 RawRestReply::contentLength() const
{
	if (!d->contentLength) {
		auto ok = false;
		d->contentLength = d->reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(&ok);
		if (!ok)
			d->contentLength = -1;
	}
	return *d->contentLength;
}

QWeakPointer<QNetworkReply> RawRestReply::reply() const
{
	return d->reply;
}

RawRestReply RawRestReply::clone() const
{
	RawRestReply copy {*this};
	copy.d.detach();
	return copy;
}

void RestReplyData::parseContentType()
{
	contentCodec = nullptr;
	contentType = reply->header(QNetworkRequest::ContentTypeHeader).toByteArray().trimmed();
	if (const auto cList = contentType->split(';'); cList.size() > 1) {
		contentType = cList.first().trimmed();
		for (auto i = 1; i < cList.size(); ++i) {
			auto args = cList[i].trimmed().split('=');
			if (args.size() == 2 && args[0] == "charset") {
				contentCodec = QTextCodec::codecForName(args[1]);
				if (!contentCodec)
					throw UnsupportedCodecException{args[1]};
			} else
				qCWarning(logReply) << "Unknown content type directive:" << args[0];
		}
	}
}
