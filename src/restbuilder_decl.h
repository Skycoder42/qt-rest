#pragma once

#include "qtrest_global.h"
#include "restreply.h"
#include "irestextender.h"

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>
#include <QtCore/QMimeType>
#include <QtCore/QVariant>
#ifdef QT_REST_USE_ASYNC
#include <QtCore/QThreadPool>
#include <QtCore/QFuture>
#endif

#include <QtNetwork/QNetworkAccessManager>

#include <qtjson.h>

namespace QtRest::__private {

struct RestBuilderData;

}

namespace QtRest {

template <template <class> class... THandlers>
class GenericRestBuilder;

template <typename TBuilder>
class RawRestBuilder
{
public:
	using Builder = TBuilder;

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

	RawRestBuilder();
	RawRestBuilder(QUrl baseUrl, QNetworkAccessManager *nam = nullptr);
	RawRestBuilder(const RawRestBuilder &other) = default;
	RawRestBuilder(RawRestBuilder &&other) noexcept = default;
	RawRestBuilder &operator=(const RawRestBuilder &other) = default;
	RawRestBuilder &operator=(RawRestBuilder &&other) noexcept = default;
	virtual ~RawRestBuilder() = default;

	template <template <class> class THandler, typename... TArgs>
	GenericRestBuilder<THandler> addContentTypeHandler(TArgs&&... args);

	Builder &setNetworkAccessManager(QNetworkAccessManager *nam);
	Builder &addExtender(IRestExtender *extender);
	template <typename TExtender>
	Builder &addExtender();

	Builder &setBaseUrl(QUrl baseUrl);
	Builder &setScheme(const QString &scheme);
	Builder &setUser(const QString &user);
	Builder &setPassword(const QString &password);
	Builder &setCredentials(const QString &user, const QString &password = {});
	Builder &setHost(const QString &host);
	Builder &setPort(quint16 port);

	Builder &addPath(const QString &pathSegment);
	Builder &addPath(const QVersionNumber &version, VersionFlags versionFlags = VersionFlag::Standard);
	Builder &addPath(const QStringList &pathSegments);
	Builder &trailingSlash(bool enable = true);

	Builder &addParameter(const QString &name, QVariant value);
	template <typename T>
	inline Builder &addParameter(const QString &name, const T &value);
	Builder &addParameters(QUrlQuery parameters, bool replace = false);
	Builder &setFragment(QVariant fragment);
	template <typename T>
	inline Builder &setFragment(const T &fragment);

	Builder &addHeader(const QLatin1String &name, QVariant value);
	template <typename T>
	inline Builder &addHeader(const QLatin1String &name, const T &value);
	Builder &addHeaders(HeaderMap headers, bool replace = false);
	Builder &setAccept(const QByteArray &mimeType);
	Builder &setAccept(const QByteArrayList &mimeTypes);
	Builder &setAccept(const QMimeType &mimeType);
	Builder &setAccept(const QList<QMimeType> &mimeTypes);

	Builder &updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags = MergeFlag::None);

	Builder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
	template <typename T>
	inline Builder &setAttribute(QNetworkRequest::Attribute attribute, const T &value);
	Builder &setAttributes(AttributeMap attributes, bool replace = false);
#ifndef QT_NO_SSL
	Builder &setSslConfig(QSslConfiguration sslConfig);
#endif

	Builder &setBody(QByteArray body, const QByteArray &contentType, bool setAccept = true);
	Builder &setBody(QByteArray body, const QMimeType &contentType, bool setAccept = true);
	Builder &setBody(QIODevice *body, const QByteArray &contentType, bool setAccept = true);
	Builder &setBody(QIODevice *body, const QMimeType &contentType, bool setAccept = true);
    QIODevice *createBodyDevice(const QByteArray &contentType, bool setAccept = true);

	Builder &addPostParameter(const QString &name, QVariant value);
	template <typename T>
	inline Builder &addPostParameter(const QString &name, const T &value);
	Builder &addPostParameters(QUrlQuery parameters, bool replace = false);

	Builder &setVerb(QByteArray verb);

	Builder &onResult(std::function<void(RawRestReply)> callback);
#ifdef QT_REST_USE_ASYNC
	Builder &onResultAsync(std::function<void(RawRestReply)> callback);
	Builder &onResultAsync(QThreadPool *threadPool, std::function<void(RawRestReply)> callback);
#endif

	QUrl buildUrl() const;
	QNetworkRequest build() const;
	QNetworkReply *send(QObject *context = nullptr) const;
    QNetworkReply *get(QObject *context = nullptr);
    QNetworkReply *post(QObject *context = nullptr);
    QNetworkReply *put(QObject *context = nullptr);
    QNetworkReply *deleteResource(QObject *context = nullptr);
    QNetworkReply *patch(QObject *context = nullptr);
    QNetworkReply *head(QObject *context = nullptr);

#ifdef QT_REST_USE_ASYNC
    QFuture<RawRestReply> sendAsync();
    QFuture<RawRestReply> getAsync();
    QFuture<RawRestReply> postAsync();
    QFuture<RawRestReply> putAsync();
    QFuture<RawRestReply> deleteResourceAsync();
    QFuture<RawRestReply> patchAsync();
    QFuture<RawRestReply> headAsync();
#endif

protected:
	RawRestBuilder(const QSharedDataPointer<__private::RestBuilderData> &d);

	QSharedDataPointer<__private::RestBuilderData> d;
};

class QTREST_EXPORT RestBuilder : public RawRestBuilder<RestBuilder> {};

template <template <class> class... THandlers>
class GenericRestBuilder : public RawRestBuilder<GenericRestBuilder<THandlers...>>
{
public:
	using Builder = GenericRestBuilder<THandlers...>;
	using RestReply = QtRest::RestReply<THandlers...>;

	template<template <class> class THandler, typename... TArgs>
	GenericRestBuilder<THandlers..., THandler> addContentTypeHandler(TArgs&&... args);

	template <template <class> class THandler, typename T>
	Builder& setBody(T &&body, bool setAccept = true);

	Builder &onResult(std::function<void(RestReply)> callback);
#ifdef QT_REST_USE_ASYNC
	Builder &onResultAsync(std::function<void(RestReply)> callback);
	Builder &onResultAsync(QThreadPool *threadPool, std::function<void(RestReply)> callback);
#endif

#ifdef QT_REST_USE_ASYNC
    QFuture<RestReply> sendAsync();
    QFuture<RestReply> getAsync();
    QFuture<RestReply> postAsync();
    QFuture<RestReply> putAsync();
    QFuture<RestReply> deleteResourceAsync();
    QFuture<RestReply> patchAsync();
    QFuture<RestReply> headAsync();
#endif

protected:
	GenericRestBuilder(const QSharedDataPointer<__private::RestBuilderData> &d,
					   std::tuple<ContentHandlerArgs<THandlers>...> &&contentHandlerArgs);

private:
	std::tuple<ContentHandlerArgs<THandlers>...> _contentHandlerArgs;
};

}

Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(QtRest::RawRestBuilder<T>::MergeFlags, typename T)
Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(QtRest::RawRestBuilder<T>::VersionFlags, typename T)
