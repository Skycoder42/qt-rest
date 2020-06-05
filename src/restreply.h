#pragma once

#include "qtrest_global.h"

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtNetwork/QNetworkReply>

namespace QtRest {

class RestReplyData;
class QTREST_EXPORT RestReply
{
    Q_GADGET

    Q_PROPERTY(bool successful READ wasSuccessful STORED false CONSTANT)
    Q_PROPERTY(int status READ status CONSTANT)
    Q_PROPERTY(QByteArray contentType READ contentType CONSTANT)
    Q_PROPERTY(QTextCodec contentCodec READ contentCodec CONSTANT)
    Q_PROPERTY(qint64 contentLength READ contentLength CONSTANT)

    Q_PROPERTY(QNetworkReply* reply READ reply CONSTANT)

public:
    RestReply(QNetworkReply *reply = nullptr);
    RestReply(const RestReply &other);
    RestReply(RestReply &&other) noexcept;
    RestReply &operator=(const RestReply &other);
    RestReply &operator=(RestReply &&other) noexcept;
    ~RestReply();

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

private:
    QExplicitlySharedDataPointer<RestReplyData> d;
};

}

