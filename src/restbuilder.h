#pragma once

#include <QtCore/QUrl>
#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>
#include <QtCore/QHash>
#include <QtCore/QMimeType>

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

    using HeaderMap = QHash<QByteArray, QByteArray>;
    using AttributeMap = QHash<QNetworkRequest::Attribute, QVariant>;

    RestBuilder();
    RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam = nullptr);
    RestBuilder(const RestBuilder &other);
    RestBuilder(RestBuilder &&other) noexcept;
    RestBuilder &operator=(const RestBuilder &other);
    RestBuilder &operator=(RestBuilder &&other) noexcept;
	virtual ~RestBuilder();

    RestBuilder &setNetworkAccessManager(QNetworkAccessManager *nam);
    RestBuilder &setBaseUrl(QUrl baseUrl);

    RestBuilder &setScheme(QString scheme);
    RestBuilder &setUser(QString user);
    RestBuilder &setPassword(QString password);
    RestBuilder &setCredentials(QString user, QString password = {});
    RestBuilder &setHost(QString scheme);
    RestBuilder &setPort(quint16 port);

    RestBuilder &addPath(const QString &pathSegment);
    RestBuilder &addPath(const QVersionNumber &pathSegment);
    RestBuilder &addPath(const QStringList &pathSegments);
    RestBuilder &trailingSlash(bool enable = true);

    RestBuilder &addParameter(const QString &name, const QString &value);
    RestBuilder &addParameters(const QUrlQuery &parameters);
    RestBuilder &setFragment(QString fragment);

    RestBuilder &addHeader(const QByteArray &name, const QByteArray &value);
    RestBuilder &addHeaders(const HeaderMap &headers);

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
    RestBuilder &setVerb(QByteArray verb);
    RestBuilder &setAccept(const QByteArray &mimeType);
    RestBuilder &setAccept(const QMimeType &mimeType);

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
Q_DECLARE_METATYPE(QtRest::RestBuilder)
Q_DECLARE_TYPEINFO(QtRest::RestBuilder, Q_MOVABLE_TYPE);
