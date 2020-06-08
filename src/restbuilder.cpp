#include "restbuilder.h"
#include "restbuilder_p.h"
#include <QtCore/QBuffer>
using namespace QtRest;

Q_LOGGING_CATEGORY(QtRest::logBuilder, "qtrest.RestBuilder")

const QByteArray Verbs::GET = "GET";
const QByteArray Verbs::POST = "POST";
const QByteArray Verbs::PUT = "PUT";
const QByteArray Verbs::DELETE = "DELETE";
const QByteArray Verbs::PATCH = "PATCH";
const QByteArray Verbs::HEAD = "HEAD";

const QLatin1String RestBuilderData::AcceptHeader {"Accept"};
const QLatin1String RestBuilderData::ContentTypeHeader {"Content-Type"};
const QByteArray RestBuilderData::ContentTypeXml1 {"application/xml"};
const QByteArray RestBuilderData::ContentTypeXml2 {"text/xml"};

RawRestBuilder::RawRestBuilder() :
    d{new RestBuilderData{}}
{}

RawRestBuilder::RawRestBuilder(QUrl baseUrl, QNetworkAccessManager *nam) :
    RawRestBuilder{}
{
	d->baseUrl = std::move(baseUrl);
    d->nam = nam;
}

RestBuilder::RestBuilder(const RestBuilder &other) = default;

RestBuilder::RestBuilder(RestBuilder &&other) noexcept = default;

RestBuilder &RestBuilder::operator=(const RestBuilder &other) = default;

RestBuilder &RestBuilder::operator=(RestBuilder &&other) noexcept = default;

RestBuilder::~RestBuilder() = default;

RawRestBuilder &RawRestBuilder::setNetworkAccessManager(QNetworkAccessManager *nam)
{
	d->nam = nam;
    return *this;
}

RawRestBuilder &RawRestBuilder::setBaseUrl(QUrl baseUrl)
{
    d->baseUrl = std::move(baseUrl);
    return *this;
}

RawRestBuilder &RawRestBuilder::setScheme(const QString &scheme)
{
    d->baseUrl.setScheme(scheme);
    return *this;
}

RawRestBuilder &RawRestBuilder::setUser(const QString &user)
{
    d->baseUrl.setUserName(user);
    return *this;
}

RawRestBuilder &RawRestBuilder::setPassword(const QString &password)
{
    d->baseUrl.setPassword(password);
    return *this;
}

RawRestBuilder &RawRestBuilder::setCredentials(const QString &user, const QString &password)
{
    return setUser(user).setPassword(password);
}

RawRestBuilder &RawRestBuilder::setHost(const QString &host)
{
    d->baseUrl.setHost(host);
    return *this;
}

RawRestBuilder &RawRestBuilder::setPort(quint16 port)
{
    d->baseUrl.setPort(port);
    return *this;
}

RawRestBuilder &RawRestBuilder::addPath(const QString &pathSegment)
{
    d->pathSegments.append(pathSegment);
    return *this;
}

RawRestBuilder &RawRestBuilder::addPath(const QVersionNumber &version, VersionFlags versionFlags)
{
    auto pathSegment = versionFlags.testFlag(VersionFlag::Normalize) ?
        version.normalized().toString() :
        version.toString();
    if (versionFlags.testFlag(VersionFlag::UseVPrefix))
        pathSegment.prepend(QLatin1Char('v'));
    return addPath(pathSegment);
}

RawRestBuilder &RawRestBuilder::addPath(const QStringList &pathSegments)
{
    d->pathSegments.append(pathSegments);
    return *this;
}

RawRestBuilder &RawRestBuilder::trailingSlash(bool enable)
{
    d->trailingSlash = enable;
    return *this;
}

RawRestBuilder &RawRestBuilder::addParameter(const QString &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    d->query.addQueryItem(name, value.toString());
    return *this;
}

RawRestBuilder &RawRestBuilder::addParameters(QUrlQuery parameters, bool replace)
{
    if (replace)
        d->query = std::move(parameters);
    else {
        for (const auto &item : parameters.queryItems())
            d->query.addQueryItem(item.first, item.second);
    }
    return *this;
}

RawRestBuilder &RawRestBuilder::setFragment(QVariant fragment)
{
    if (!fragment.convert(QMetaType::QString))
        throw this;  // TODO
    d->fragment = fragment.toString();
    return *this;
}

RawRestBuilder &RawRestBuilder::addHeader(const QLatin1String &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    d->headers.insert(name.latin1(), value.toString().toLatin1());
    return *this;
}

RawRestBuilder &RawRestBuilder::addHeaders(HeaderMap headers, bool replace)
{
    if (replace)
        d->headers = std::move(headers);
    else {
        for (auto it = headers.begin(), end = headers.end(); it != end; ++it)
            d->headers.insert(it.key(), it.value());
    }
    return *this;
}

RawRestBuilder &RawRestBuilder::setAccept(const QByteArray &mimeType)
{
    return addHeader(RestBuilderData::AcceptHeader, mimeType);
}

RawRestBuilder &RawRestBuilder::setAccept(const QByteArrayList &mimeTypes)
{
    return addHeader(RestBuilderData::AcceptHeader, mimeTypes.join(", "));
}

RawRestBuilder &RawRestBuilder::setAccept(const QMimeType &mimeType)
{
    return addHeader(RestBuilderData::AcceptHeader, mimeType.name().toUtf8());
}

RawRestBuilder &RawRestBuilder::setAccept(const QList<QMimeType> &mimeTypes)
{
    QByteArrayList values;
    values.reserve(mimeTypes.size());
    for (const auto &mimeType : mimeTypes)
        values.append(mimeType.name().toUtf8());
    return addHeader(RestBuilderData::AcceptHeader, values.join(", "));
}

RawRestBuilder &RawRestBuilder::updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags)
{
    auto cUrl = buildUrl();
    d->baseUrl = cUrl.resolved(url);
    if (d->baseUrl.host() != cUrl.host()) {
        qCWarning(logBuilder) << "URL host changed from"
                              << cUrl.host()
                              << "to"
                              << d->baseUrl.host();
    }

    //clear all the rest
    d->pathSegments.clear();
    if (mergeFlags.testFlag(MergeFlag::MergeQuery)) {
        QUrlQuery query{url.query()};
        for (const auto &item : query.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
            d->query.addQueryItem(item.first, item.second);
    } else
        d->query = QUrlQuery{url.query()};

    if (!mergeFlags.testFlag(MergeFlag::KeepFragment))
        d->fragment.clear();

    return *this;
}

RawRestBuilder &RawRestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
    d->attributes.insert(attribute, value);
    return *this;
}

RawRestBuilder &RawRestBuilder::setAttributes(AttributeMap attributes, bool replace)
{
    if (replace)
        d->attributes = std::move(attributes);
    else {
        for (auto it = attributes.begin(), end = attributes.end(); it != end; ++it)
            d->attributes.insert(it.key(), it.value());
    }
    return *this;
}

#ifndef QT_NO_SSL
RawRestBuilder &RawRestBuilder::setSslConfig(QSslConfiguration sslConfig)
{
    d->sslConfig = std::move(sslConfig);
    return *this;
}
#endif

RawRestBuilder &RawRestBuilder::setBody(QIODevice *body, const QByteArray &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderData::ContentTypeHeader, contentType);
}

RawRestBuilder &RawRestBuilder::setBody(QIODevice *body, const QMimeType &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderData::ContentTypeHeader, contentType.name().toUtf8());
}

RawRestBuilder &RawRestBuilder::setBody(QByteArray body, const QByteArray &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderData::ContentTypeHeader, contentType);
}

RawRestBuilder &RawRestBuilder::setBody(QByteArray body, const QMimeType &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderData::ContentTypeHeader, contentType.name().toUtf8());
}

QXmlStreamWriter RawRestBuilder::createXmlBody(bool setAccept)
{
    addHeader(RestBuilderData::ContentTypeHeader, RestBuilderData::ContentTypeXml1);
    if (setAccept)
        this->setAccept(QByteArrayList{RestBuilderData::ContentTypeXml1, RestBuilderData::ContentTypeXml2});
    const auto buffer = new QBuffer{};
    buffer->open(QIODevice::WriteOnly | QIODevice::Text);
    return QXmlStreamWriter{buffer};
}

void RawRestBuilder::completeXmlBody(QXmlStreamWriter &writer)
{
    writer.device()->close();
}

RawRestBuilder &RawRestBuilder::setVerb(QByteArray verb)
{
    d->verb = std::move(verb);
    return *this;
}

RawRestBuilder &RawRestBuilder::addPostParameter(const QString &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    if (!std::holds_alternative<QUrlQuery>(d->body))
        d->body = QUrlQuery{};
    std::get<QUrlQuery>(d->body).addQueryItem(name, value.toString());
    return *this;
}

RawRestBuilder &RawRestBuilder::addPostParameters(QUrlQuery parameters, bool replace)
{
    if (replace)
        d->body = std::move(parameters);
    else {
        if (!std::holds_alternative<QUrlQuery>(d->body))
            d->body = QUrlQuery{};
        for (const auto &item : parameters.queryItems())
            std::get<QUrlQuery>(d->body).addQueryItem(item.first, item.second);
    }
    return *this;
}

RawRestBuilder &RawRestBuilder::onResult(RawRestBuilder::RawResultCallback callback)
{
    d->resultCallbacks.append(std::move(callback));
    return *this;
}

QUrl RawRestBuilder::buildUrl() const
{
    throw nullptr;
}

QNetworkRequest RawRestBuilder::build() const
{
    throw nullptr;
}

QNetworkReply *RawRestBuilder::send() const
{
    throw nullptr;
}
