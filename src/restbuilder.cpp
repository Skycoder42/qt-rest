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

const QLatin1String RestBuilderPrivate::AcceptHeader {"Accept"};
const QLatin1String RestBuilderPrivate::ContentTypeHeader {"Content-Type"};
const QByteArray RestBuilderPrivate::ContentTypeJson {"application/json"};
const QByteArray RestBuilderPrivate::ContentTypeCbor {"application/cbor"};
const QByteArray RestBuilderPrivate::ContentTypeXml1 {"application/xml"};
const QByteArray RestBuilderPrivate::ContentTypeXml2 {"text/xml"};

RestBuilder::RestBuilder() :
    d{new RestBuilderPrivate{}}
{}

RestBuilder::RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam) :
    RestBuilder{}
{
	d->baseUrl = std::move(baseUrl);
    d->nam = nam;
}

RestBuilder::RestBuilder(const RestBuilder &other) = default;

RestBuilder::RestBuilder(RestBuilder &&other) noexcept = default;

RestBuilder &RestBuilder::operator=(const RestBuilder &other) = default;

RestBuilder &RestBuilder::operator=(RestBuilder &&other) noexcept = default;

RestBuilder::~RestBuilder() = default;

RestBuilder &RestBuilder::setNetworkAccessManager(QNetworkAccessManager *nam)
{
	d->nam = nam;
    return *this;
}

RestBuilder &RestBuilder::setBaseUrl(QUrl baseUrl)
{
    d->baseUrl = std::move(baseUrl);
    return *this;
}

RestBuilder &RestBuilder::setScheme(const QString &scheme)
{
    d->baseUrl.setScheme(scheme);
    return *this;
}

RestBuilder &RestBuilder::setUser(const QString &user)
{
    d->baseUrl.setUserName(user);
    return *this;
}

RestBuilder &RestBuilder::setPassword(const QString &password)
{
    d->baseUrl.setPassword(password);
    return *this;
}

RestBuilder &RestBuilder::setCredentials(const QString &user, const QString &password)
{
    return setUser(user).setPassword(password);
}

RestBuilder &RestBuilder::setHost(const QString &host)
{
    d->baseUrl.setHost(host);
    return *this;
}

RestBuilder &RestBuilder::setPort(quint16 port)
{
    d->baseUrl.setPort(port);
    return *this;
}

RestBuilder &RestBuilder::addPath(const QString &pathSegment)
{
    d->pathSegments.append(pathSegment);
    return *this;
}

RestBuilder &RestBuilder::addPath(const QVersionNumber &version, VersionFlags versionFlags)
{
    auto pathSegment = versionFlags.testFlag(VersionFlag::Normalize) ?
        version.normalized().toString() :
        version.toString();
    if (versionFlags.testFlag(VersionFlag::UseVPrefix))
        pathSegment.prepend(QLatin1Char('v'));
    return addPath(pathSegment);
}

RestBuilder &RestBuilder::addPath(const QStringList &pathSegments)
{
    d->pathSegments.append(pathSegments);
    return *this;
}

RestBuilder &RestBuilder::trailingSlash(bool enable)
{
    d->trailingSlash = enable;
    return *this;
}

RestBuilder &RestBuilder::addParameter(const QString &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    d->query.addQueryItem(name, value.toString());
    return *this;
}

RestBuilder &RestBuilder::addParameters(QUrlQuery parameters, bool replace)
{
    if (replace)
        d->query = std::move(parameters);
    else {
        for (const auto &item : parameters.queryItems())
            d->query.addQueryItem(item.first, item.second);
    }
    return *this;
}

RestBuilder &RestBuilder::setFragment(QVariant fragment)
{
    if (!fragment.convert(QMetaType::QString))
        throw this;  // TODO
    d->fragment = fragment.toString();
    return *this;
}

RestBuilder &RestBuilder::addHeader(const QLatin1String &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    d->headers.insert(name.latin1(), value.toString().toLatin1());
    return *this;
}

RestBuilder &RestBuilder::addHeaders(HeaderMap headers, bool replace)
{
    if (replace)
        d->headers = std::move(headers);
    else {
        for (auto it = headers.begin(), end = headers.end(); it != end; ++it)
            d->headers.insert(it.key(), it.value());
    }
    return *this;
}

RestBuilder &RestBuilder::setAccept(const QByteArray &mimeType)
{
    return addHeader(RestBuilderPrivate::AcceptHeader, mimeType);
}

RestBuilder &RestBuilder::setAccept(const QByteArrayList &mimeTypes)
{
    return addHeader(RestBuilderPrivate::AcceptHeader, mimeTypes.join(", "));
}

RestBuilder &RestBuilder::setAccept(const QMimeType &mimeType)
{
    return addHeader(RestBuilderPrivate::AcceptHeader, mimeType.name().toUtf8());
}

RestBuilder &RestBuilder::setAccept(const QList<QMimeType> &mimeTypes)
{
    QByteArrayList values;
    values.reserve(mimeTypes.size());
    for (const auto &mimeType : mimeTypes)
        values.append(mimeType.name().toUtf8());
    return addHeader(RestBuilderPrivate::AcceptHeader, values.join(", "));
}

RestBuilder &RestBuilder::updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags)
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

RestBuilder &RestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
    d->attributes.insert(attribute, value);
    return *this;
}

RestBuilder &RestBuilder::setAttributes(AttributeMap attributes, bool replace)
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
RestBuilder &RestBuilder::setSslConfig(QSslConfiguration sslConfig)
{
    d->sslConfig = std::move(sslConfig);
    return *this;
}
#endif

RestBuilder &RestBuilder::setBody(QIODevice *body, const QByteArray &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderPrivate::ContentTypeHeader, contentType);
}

RestBuilder &RestBuilder::setBody(QIODevice *body, const QMimeType &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderPrivate::ContentTypeHeader, contentType.name().toUtf8());
}

RestBuilder &RestBuilder::setBody(QByteArray body, const QByteArray &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderPrivate::ContentTypeHeader, contentType);
}

RestBuilder &RestBuilder::setBody(QByteArray body, const QMimeType &contentType, bool setAccept)
{
    d->body = body;
    if (setAccept)
        this->setAccept(contentType);
    return addHeader(RestBuilderPrivate::ContentTypeHeader, contentType.name().toUtf8());
}

QXmlStreamWriter RestBuilder::createXmlBody(bool setAccept)
{
    addHeader(RestBuilderPrivate::ContentTypeHeader, RestBuilderPrivate::ContentTypeXml1);
    if (setAccept)
        this->setAccept(QByteArrayList{RestBuilderPrivate::ContentTypeXml1, RestBuilderPrivate::ContentTypeXml2});
    const auto buffer = new QBuffer{};
    buffer->open(QIODevice::WriteOnly | QIODevice::Text);
    return QXmlStreamWriter{buffer};
}

void RestBuilder::completeXmlBody(QXmlStreamWriter &writer)
{
    writer.device()->close();
}

RestBuilder &RestBuilder::setVerb(QByteArray verb)
{
    d->verb = std::move(verb);
    return *this;
}

RestBuilder &RestBuilder::addPostParameter(const QString &name, QVariant value)
{
    if (!value.convert(QMetaType::QString))
        throw this;  // TODO
    if (!std::holds_alternative<QUrlQuery>(d->body))
        d->body = QUrlQuery{};
    std::get<QUrlQuery>(d->body).addQueryItem(name, value.toString());
    return *this;
}

RestBuilder &RestBuilder::addPostParameters(QUrlQuery parameters, bool replace)
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

QUrl RestBuilder::buildUrl() const
{
    throw nullptr;
}

QNetworkRequest RestBuilder::build() const
{
    throw nullptr;
}

QNetworkReply *RestBuilder::send() const
{
    throw nullptr;
}
