#include "restreply.h"
#include <optional>
#include <QtCore/QPointer>
using namespace QtRest;

Q_LOGGING_CATEGORY(QtRest::logReply, "qtrest.RestReply")

namespace QtRest {

class RestReplyData : public QSharedData
{
public:
    QPointer<QNetworkReply> reply;

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
    d->reply = reply;
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

QIODevice *RawRestReply::bodyDevice() const
{
    return d->reply;
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
    return status() < 300;
}

int RawRestReply::status() const
{
    if (!d->statusCode)
        d->statusCode = d->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return *d->statusCode;
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
    if (!d->contentLength)
        d->contentLength = d->reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    return *d->contentLength;
}

QNetworkReply *RawRestReply::reply() const
{
    return d->reply;
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
                if (!contentCodec) {
                    // TODO throw
                }
            } else
                qCWarning(logReply) << "Unknown content type directive:" << args[0];
        }
    }
}
