#pragma once

#include "qtrest_global.h"
#include "contenthandler.h"

#include <tuple>
#include <variant>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QLoggingCategory>

#include <QtNetwork/QNetworkReply>

namespace QtRest {

template <template<class> class... THandlers>
class RestReply;

class RestReplyData;
class QTREST_EXPORT RawRestReply
{
    Q_GADGET

    Q_PROPERTY(bool successful READ wasSuccessful STORED false CONSTANT)
    Q_PROPERTY(int status READ status CONSTANT)
    Q_PROPERTY(QByteArray contentType READ contentType CONSTANT)
    Q_PROPERTY(QTextCodec contentCodec READ contentCodec CONSTANT)
    Q_PROPERTY(qint64 contentLength READ contentLength CONSTANT)

    Q_PROPERTY(QNetworkReply* reply READ reply CONSTANT)

public:
    RawRestReply(QNetworkReply *reply = nullptr);
    RawRestReply(const RawRestReply &other);
    RawRestReply(RawRestReply &&other) noexcept;
    RawRestReply &operator=(const RawRestReply &other);
    RawRestReply &operator=(RawRestReply &&other) noexcept;
    ~RawRestReply();

    Q_INVOKABLE bool hasHeader(const QLatin1String &name) const;
    Q_INVOKABLE QString header(const QLatin1String &name) const;
    template <typename T>
    inline T header(const QLatin1String &name) const {
        return QVariant{header(name)}.template value<T>();
    }

    Q_INVOKABLE QVariant attribute(QNetworkRequest::Attribute attrib) const;
    template <typename T>
    inline T attribute(QNetworkRequest::Attribute attrib) const {
        return attribute(attrib).template value<T>();
    }

    Q_INVOKABLE QIODevice *bodyDevice() const;
    Q_INVOKABLE QByteArray body();
    Q_INVOKABLE QString bodyString();

    bool wasSuccessful() const;
    int status() const;
    QByteArray contentType() const;
    QTextCodec *contentCodec() const;
    qint64 contentLength() const;
    QNetworkReply* reply() const;


    template <template<class> class... THandlers>
    inline RestReply<THandlers...> toGeneric(ContentHandlerArgs<THandlers>... args) const {
        return RestReply<THandlers...>{std::move(args)..., *this};
    }

private:
    QExplicitlySharedDataPointer<RestReplyData> d;
};

template <template<class> class... THandlers>
class RestReply : public RawRestReply
{
private:
    friend class RawRestReply;

    template <typename T>
    using HandlerVariant = std::variant<THandlers<T>...>;

public:
    RestReply(ContentHandlerArgs<THandlers>... args, QNetworkReply *reply = nullptr) :
        RawRestReply{reply},
        _initArgs{std::make_tuple(std::move(args)...)}
    {}

    template <typename T>
    T body() {
        auto handler = findHandler<T, THandlers...>();
        return std::visit([this](auto &handler) -> T {
            if constexpr (std::decay_t<decltype(handler)>::IsStringHandler)
                return handler.read(this->bodyString(), this->contentType());
            else
                return handler.read(this->RawRestReply::body(), this->contentType(), this->contentCodec());
        }, handler);
    }

private:
    std::tuple<ContentHandlerArgs<THandlers>...> _initArgs;

    RestReply(ContentHandlerArgs<THandlers>... args, const RawRestReply &base) :
        RawRestReply{base},
        _initArgs{std::make_tuple(std::move(args)...)}
    {}

    template <typename T>
    HandlerVariant<T> findHandler() const {
        throw nullptr; // TODO
    }

    template <typename T, template<class> class THandler, template<class> class... TOthers>
    HandlerVariant<T> findHandler() const {
        THandler<T> handler(std::get<ContentHandlerArgs<THandler>>(_initArgs));
        if (handler.contentTypes().contains(this->contentType()))
            return handler;
        else
            return findHandler<T, TOthers...>();
    }
};

Q_DECLARE_LOGGING_CATEGORY(logReply)

}

