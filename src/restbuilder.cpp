#include "restbuilder.h"
#include "restbuilder_p.h"
using namespace QtRest;

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

RestBuilder &RestBuilder::addParameters(const QUrlQuery &parameters, bool replace)
{
    if (replace)
        d->query = parameters;
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

RestBuilder &RestBuilder::addHeaders(const RestBuilder::HeaderMap &headers, bool replace)
{
    if (replace)
        d->headers = headers;
    else {
        for (auto it = headers.begin(), end = headers.end(); it != end; ++it)
            d->headers.insert(it.key(), it.value());
    }
    return *this;
}
