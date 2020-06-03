#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QList>

namespace QtRest {

template <typename T>
class ContentHandler
{
public:
    using WriteResult = std::pair<QByteArray, QByteArray>; // (data, contentType)

    virtual QByteArrayList contentTypes() const = 0;

    virtual WriteResult write(const T &data) = 0;
    virtual T read(const QByteArray &data, const QByteArray &contentType) = 0;
};

template <typename TFixed, typename TValue>
class FixedContentHandler : public ContentHandler<TFixed>
{
    static_assert (std::is_same_v<TValue, TFixed>, "A FixedContentHandler only supports a single, fixed type TFixed");
};
// usage: class MyFixedHandler<T> : public FixedContentHandler<MyType, T>

}
