#include "restreply.h"
#include <optional>
#include <QtCore/QPointer>
using namespace QtRest;

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

RestReply::RestReply(QNetworkReply *reply) :
    d{new RestReplyData{}}
{
    d->reply = reply;
}

RestReply::RestReply(const RestReply &other) = default;

RestReply::RestReply(RestReply &&other) noexcept = default;

RestReply &RestReply::operator=(const RestReply &other) = default;

RestReply &RestReply::operator=(RestReply &&other) noexcept = default;

RestReply::~RestReply() = default;

bool RestReply::hasHeader(const QLatin1String &name) const
{
    return d->reply->hasRawHeader(name.latin1());
}

QString RestReply::header(const QLatin1String &name) const
{
    return QString::fromLatin1(d->reply->rawHeader(name.latin1()));
}

QVariant RestReply::attribute(QNetworkRequest::Attribute attribute) const
{
    throw nullptr;
}

QIODevice *RestReply::bodyDevice() const
{
    throw nullptr;
}

QByteArray RestReply::body()
{
    throw nullptr;
}

QString RestReply::bodyString()
{
    throw nullptr;
}

bool RestReply::wasSuccessful() const
{
    throw nullptr;
}

int RestReply::status() const
{
    throw nullptr;
}

QByteArray RestReply::contentType() const
{
    throw nullptr;
}

QTextCodec *RestReply::contentCodec() const
{
    throw nullptr;
}

qint64 RestReply::contentLength() const
{
    throw nullptr;
}

QNetworkReply *RestReply::reply() const
{
    throw nullptr;
}
