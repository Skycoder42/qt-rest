#pragma once

#include "contenthandler.h"
#include <qtjson.h>

namespace QtRest {

template <typename T>
class CborContentHandler;

template <>
struct QTREST_EXPORT ContentHandlerArgs<CborContentHandler> {
    static const QByteArray ContentType;

    QtJson::Configuration config = {};
    QCborValue::EncodingOptions options = QCborValue::NoTransformation;
};

template <typename T>
class CborContentHandler : public IByteArrayContentHandler<T>
{
public:
    using WriteResult = typename IByteArrayContentHandler<T>::WriteResult;

    CborContentHandler(ContentHandlerArgs<CborContentHandler> args) :
        _config{std::move(args)}
    {}

    QByteArrayList contentTypes() const override {
        return {ContentHandlerArgs<CborContentHandler>::ContentType};
    }

    WriteResult write(const T &data) override {
        return std::make_pair(QtJson::binarify(data, _config.config, _config.options),
                              ContentHandlerArgs<CborContentHandler>::ContentType);
    }

    T read(const QByteArray &data, const QByteArray &contentType, QTextCodec *) override {
        Q_UNUSED(contentType)
        return QtJson::parseBinary<T>(data, _config.config);
    }

private:
    ContentHandlerArgs<CborContentHandler> _config;
};

template <>
class QTREST_EXPORT CborContentHandler<QCborValue> : public IByteArrayContentHandler<QCborValue>
{
public:
    using WriteResult = typename IByteArrayContentHandler<QCborValue>::WriteResult;

    CborContentHandler(ContentHandlerArgs<CborContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QCborValue &data) override;
    QCborValue read(const QByteArray &data, const QByteArray &contentType, QTextCodec *) override;

private:
    QCborValue::EncodingOptions _options;
};

template <>
class QTREST_EXPORT CborContentHandler<QCborMap> : public IByteArrayContentHandler<QCborMap>
{
public:
    using WriteResult = typename IByteArrayContentHandler<QCborMap>::WriteResult;

    CborContentHandler(ContentHandlerArgs<CborContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QCborMap &data) override;
    QCborMap read(const QByteArray &data, const QByteArray &contentType, QTextCodec *) override;

private:
    QCborValue::EncodingOptions _options;
};

template <>
class QTREST_EXPORT CborContentHandler<QCborArray> : public IByteArrayContentHandler<QCborArray>
{
public:
    using WriteResult = typename IByteArrayContentHandler<QCborArray>::WriteResult;

    CborContentHandler(ContentHandlerArgs<CborContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QCborArray &data) override;
    QCborArray read(const QByteArray &data, const QByteArray &contentType, QTextCodec *) override;

private:
    QCborValue::EncodingOptions _options;
};

}

