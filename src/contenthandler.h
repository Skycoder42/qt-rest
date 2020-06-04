#pragma once

#include <optional>

#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QList>

namespace QtRest {

template <typename T>
class ContentHandler
{
public:
    using WriteResult = std::pair<QByteArray, QByteArray>; // (data, contentType)

    virtual QByteArrayList contentTypes() const = 0;

    virtual WriteResult write(const T &data) = 0;
    virtual T read(const QByteArray &data, const QByteArray &contentType, QTextCodec *codec = nullptr) = 0;
};

template <typename T>
class StringContentHandler : public ContentHandler<T>
{
public:
    virtual T read(const QString &data, const QByteArray &contentType) = 0;
    inline T read(const QByteArray &data, const QByteArray &contentType,  QTextCodec *codec) final {
        if (codec)
            return read(codec->toUnicode(data), contentType);
        else
            return read(QString::fromUtf8(data), contentType);
    }
};

}
