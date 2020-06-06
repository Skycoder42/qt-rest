#pragma once

#include <optional>

#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QList>

namespace QtRest {

template <template <class> class THandler>
struct ContentHandlerArgs;

template <typename T>
class IByteArrayContentHandler
{
public:
    static constexpr bool IsStringHandler = false;

    using WriteResult = std::pair<QByteArray, QByteArray>; // (data, contentType)

    virtual QByteArrayList contentTypes() const = 0;

    virtual WriteResult write(const T &data) = 0;
    virtual T read(const QByteArray &data, const QByteArray &contentType, QTextCodec *codec = nullptr) = 0;
};

template <typename T>
class IStringContentHandler
{
public:
    static constexpr bool IsStringHandler = true;

    using WriteResult = std::pair<QString, QByteArray>; // (data, contentType)

    virtual QByteArrayList contentTypes() const = 0;

    virtual WriteResult write(const T &data) = 0;
    virtual T read(const QString &data, const QByteArray &contentType) = 0;
};

}
