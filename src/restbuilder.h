#pragma once

#include "qtrest_global.h"
#include "restreply.h"

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>
#include <QtCore/QMimeType>
#include <QtCore/QVariant>
#include <QtCore/QXmlStreamWriter>

#include <QtNetwork/QNetworkAccessManager>

#include <qtjson.h>

namespace QtRest {

struct RestBuilderData;

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

    template<template <class> class THandler, typename... TArgs>
    GenericRestBuilder<THandler> addContentTypeHandler(TArgs&&... args);

    Builder &setNetworkAccessManager(QNetworkAccessManager *nam);
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
    inline Builder &addParameter(const QString &name, const T &value) {
        return addParameter(name, QVariant::fromValue(value));
    }
    Builder &addParameters(QUrlQuery parameters, bool replace = false);
    Builder &setFragment(QVariant fragment);
    template <typename T>
    inline Builder &setFragment(const T &fragment) {
        return setFragment(QVariant::fromValue(fragment));
    }

    Builder &addHeader(const QLatin1String &name, QVariant value);
    template <typename T>
    inline Builder &addHeader(const QLatin1String &name, const T &value) {
        return addHeader(name, QVariant::fromValue(value));
    }
    Builder &addHeaders(HeaderMap headers, bool replace = false);
    Builder &setAccept(const QByteArray &mimeType);
    Builder &setAccept(const QByteArrayList &mimeTypes);
    Builder &setAccept(const QMimeType &mimeType);
    Builder &setAccept(const QList<QMimeType> &mimeTypes);

    Builder &updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags = MergeFlag::None);

    Builder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
    template <typename T>
    inline Builder &setAttribute(QNetworkRequest::Attribute attribute, const T &value) {
        return setAttribute(attribute, QVariant::fromValue(value));
    }
    Builder &setAttributes(AttributeMap attributes, bool replace = false);
#ifndef QT_NO_SSL
    Builder &setSslConfig(QSslConfiguration sslConfig);
#endif

    Builder &setBody(QByteArray body, const QByteArray &contentType, bool setAccept = true);
    Builder &setBody(QByteArray body, const QMimeType &contentType, bool setAccept = true);
    Builder &setBody(QIODevice *body, const QByteArray &contentType, bool setAccept = true);
    Builder &setBody(QIODevice *body, const QMimeType &contentType, bool setAccept = true);

    Builder &setVerb(QByteArray verb);

    Builder &addPostParameter(const QString &name, QVariant value);
    template <typename T>
    inline Builder &addPostParameter(const QString &name, const T &value) {
        return addPostParameter(name, QVariant::fromValue(value));
    }
    Builder &addPostParameters(QUrlQuery parameters, bool replace = false);

    Builder &onResult(std::function<void(RawRestReply)> callback);

    QUrl buildUrl() const;
    QNetworkRequest build() const;
    QNetworkReply *send() const;

protected:
    RawRestBuilder(const QSharedDataPointer<RestBuilderData> &d);

private:
    QSharedDataPointer<RestBuilderData> d;
};

class QTREST_EXPORT RestBuilder : public RawRestBuilder<RestBuilder> {};

template <template <class> class... THandlers>
class GenericRestBuilder : public RawRestBuilder<GenericRestBuilder<THandlers...>>
{
public:
    using Builder = GenericRestBuilder<THandlers...>;

    template<template <class> class THandler, typename... TArgs>
    GenericRestBuilder<THandlers..., THandler> addContentTypeHandler(TArgs&&... args);

    template <template <class> class THandler, typename T>
    inline Builder& setBody(T &&body, bool setAccept = true) {
        using TContent = std::decay_t<T>;
        const THandler<TContent> handler{};
        auto [data, contentType] = handler.write(std::forward<T>(body));
        if constexpr (THandler<TContent>::IsStringHandler)
            return setBody(data.toUtf8(), std::move(contentType), setAccept);
        else
            return setBody(std::move(data), std::move(contentType), setAccept);
    }

    inline Builder &onResult(std::function<void(RestReply<THandlers...>)> callback) {
        return onResult([=](const RawRestReply &reply) {
            callback(reply.toGeneric<THandlers...>());
        });
    }

private:
    std::tuple<ContentHandlerArgs<THandlers>...> _contentHandlerArgs;
};

}

#define Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(Flags, ...) \
    template <__VA_ARGS__> \
    Q_DECL_CONSTEXPR inline QFlags<typename Flags::enum_type> operator|(typename Flags::enum_type f1, typename Flags::enum_type f2) noexcept \
    { return QFlags<typename Flags::enum_type>(f1) | f2; } \
    template <__VA_ARGS__> \
    Q_DECL_CONSTEXPR inline QFlags<typename Flags::enum_type> operator|(typename Flags::enum_type f1, QFlags<typename Flags::enum_type> f2) noexcept \
    { return f2 | f1; } \
    template <__VA_ARGS__> \
    Q_DECL_CONSTEXPR inline QIncompatibleFlag operator|(typename Flags::enum_type f1, int f2) noexcept \
    { return QIncompatibleFlag(int(f1) | f2); }

Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(QtRest::RawRestBuilder<T>::MergeFlags, typename T)
Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(QtRest::RawRestBuilder<T>::VersionFlags, typename T)
