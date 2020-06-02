#pragma once

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>
#include <QtCore/QHash>
#include <QtCore/QMimeType>
#include <QtCore/QVariant>
#include <QtCore/QXmlStreamWriter>

#include <QtNetwork/QNetworkAccessManager>

namespace QtRest {

struct RestBuilderPrivate;
class QTREST_EXPORT RestBuilder
{
public:
    enum class MergeFlag {
        None = 0x00,
        MergeQuery = 0x01,
        KeepFragment = 0x02,
    };
    Q_DECLARE_FLAGS(MergeFlags, MergeFlag)

    enum class VersionFlag {
        None = 0x00,
        UseVPrefix = 0x01,
        Normalize = 0x02,
        Standard = (UseVPrefix | Normalize)
    };
    Q_DECLARE_FLAGS(VersionFlags, VersionFlag)

    using HeaderMap = QHash<QByteArray, QByteArray>;
    using AttributeMap = QHash<QNetworkRequest::Attribute, QVariant>;

    static void registerContentTypeHandler(const QByteArray &contentType);

    RestBuilder();
    RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam = nullptr);
    RestBuilder(const RestBuilder &other);
    RestBuilder(RestBuilder &&other) noexcept;
    RestBuilder &operator=(const RestBuilder &other);
    RestBuilder &operator=(RestBuilder &&other) noexcept;
	virtual ~RestBuilder();

    RestBuilder &setNetworkAccessManager(QNetworkAccessManager *nam);
    RestBuilder &setBaseUrl(QUrl baseUrl);

    RestBuilder &setScheme(const QString &scheme);
    RestBuilder &setUser(const QString &user);
    RestBuilder &setPassword(const QString &password);
    RestBuilder &setCredentials(const QString &user, const QString &password = {});
    RestBuilder &setHost(const QString &host);
    RestBuilder &setPort(quint16 port);

    RestBuilder &addPath(const QString &pathSegment);
    RestBuilder &addPath(const QVersionNumber &version, VersionFlags versionFlags = VersionFlag::Standard);
    RestBuilder &addPath(const QStringList &pathSegments);
    RestBuilder &trailingSlash(bool enable = true);

    RestBuilder &addParameter(const QString &name, QVariant value);
    template <typename T>
    inline RestBuilder &addParameter(const QString &name, const T &value) {
        return addParameter(name, QVariant::fromValue(value));
    }
    RestBuilder &addParameters(const QUrlQuery &parameters, bool replace = false);
    RestBuilder &setFragment(QVariant fragment);
    template <typename T>
    inline RestBuilder &setFragment(const T &fragment) {
        return setFragment(QVariant::fromValue(fragment));
    }

    RestBuilder &addHeader(const QLatin1String &name, QVariant value);
    template <typename T>
    inline RestBuilder &addHeader(const QLatin1String &name, const T &value) {
        return addHeader(name, QVariant::fromValue(value));
    }
    RestBuilder &addHeaders(const HeaderMap &headers, bool replace = false);
    RestBuilder &setAccept(const QByteArray &mimeType);
    RestBuilder &setAccept(const QByteArrayList &mimeTypes);
    RestBuilder &setAccept(const QMimeType &mimeType);
    RestBuilder &setAccept(const QList<QMimeType> &mimeTypes);

    RestBuilder &updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags = MergeFlag::None);

    RestBuilder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
    RestBuilder &setAttributes(const AttributeMap &attributes);
#ifndef QT_NO_SSL
    RestBuilder &setSslConfig(QSslConfiguration sslConfig);
#endif

    RestBuilder &setBody(QByteArray body, const QByteArray &contentType, bool setAccept = true);
    RestBuilder &setBody(QByteArray body, const QMimeType &contentType, bool setAccept = true);
    RestBuilder &setBody(QJsonValue body, bool setAccept = true);
    RestBuilder &setBody(QCborValue body, bool setAccept = true);

    QXmlStreamWriter createXmlBody(bool setAccept = true);
    void completeXmlBody(QXmlStreamWriter &writer);

    template <typename T>
    RestBuilder &setJsonBody(const T &body, bool setAccept = true);
    template <typename T>
    RestBuilder &setCborBody(const T &body, bool setAccept = true);

    RestBuilder &setVerb(QByteArray verb);

    RestBuilder &addPostParameter(const QString &name, const QString &value);
    RestBuilder &addPostParameters(const QUrlQuery &parameters);

    QUrl buildUrl() const;
    QNetworkRequest build() const;
    QNetworkReply *send() const;

private:
    QSharedDataPointer<RestBuilderPrivate> d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtRest::RestBuilder::MergeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QtRest::RestBuilder::VersionFlags)
Q_DECLARE_METATYPE(QtRest::RestBuilder)
Q_DECLARE_TYPEINFO(QtRest::RestBuilder, Q_MOVABLE_TYPE);
