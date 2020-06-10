#pragma once

#include "contenthandler.h"
#include <qtjson.h>

namespace QtRest {

template <typename T>
class JsonContentHandler;

template <>
struct QTREST_EXPORT ContentHandlerArgs<JsonContentHandler> {
    static const QByteArray ContentType;

    QtJson::Configuration config = {};
    QJsonDocument::JsonFormat format = QJsonDocument::Indented;
};

template <typename T>
class JsonContentHandler : public IStringContentHandler<T>
{
public:
    using WriteResult = typename IStringContentHandler<T>::WriteResult;

    JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
        _config{std::move(args)}
    {}

    QByteArrayList contentTypes() const override {
        return {ContentHandlerArgs<JsonContentHandler>::ContentType};
    }

    WriteResult write(const T &data) override {
        return std::make_pair(QtJson::stringify(data, _config.config, _config.format),
                              ContentHandlerArgs<JsonContentHandler>::ContentType);
    }

    T read(const QString &data, const QByteArray &contentType) override {
        Q_UNUSED(contentType)
        return QtJson::parseString<T>(data, _config.config);
    }

private:
    ContentHandlerArgs<JsonContentHandler> _config;
};

template <>
class QTREST_EXPORT JsonContentHandler<QJsonValue> : public IStringContentHandler<QJsonValue>
{
public:
    using WriteResult = typename IStringContentHandler<QJsonValue>::WriteResult;

    JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QJsonValue &data) override;
    QJsonValue read(const QString &data, const QByteArray &contentType) override;

private:
    QJsonDocument::JsonFormat _format;
};

template <>
class QTREST_EXPORT JsonContentHandler<QJsonObject> : public IStringContentHandler<QJsonObject>
{
public:
    using WriteResult = typename IStringContentHandler<QJsonObject>::WriteResult;

    JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QJsonObject &data) override;
    QJsonObject read(const QString &data, const QByteArray &contentType) override;

private:
    QJsonDocument::JsonFormat _format;
};

template <>
class QTREST_EXPORT JsonContentHandler<QJsonArray> : public IStringContentHandler<QJsonArray>
{
public:
    using WriteResult = typename IStringContentHandler<QJsonArray>::WriteResult;

    JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args);

    QByteArrayList contentTypes() const override;
    WriteResult write(const QJsonArray &data) override;
    QJsonArray read(const QString &data, const QByteArray &contentType) override;

private:
    QJsonDocument::JsonFormat _format;
};

}
