#pragma once

#include "contenthandler.h"
#include <qtjson.h>

namespace QtRest {

template <typename T>
class JsonContentHandler : public ContentHandler<T>
{
public:
    using WriteResult = typename ContentHandler<T>::WriteResult;

    JsonContentHandler(QtJson::Configuration config = {},
                       QJsonDocument::JsonFormat format = QJsonDocument::Indented) :
        _config{std::move(config)},
        _format{format}
    {}

    QByteArrayList contentTypes() const override {
        return {"application/json"};
    }

    WriteResult write(const T &data) override {
        return std::make_pair(QtJson::stringify(data, _config, _format).toUtf8(), "application/json");
    }

    T read(const QByteArray &data, const QByteArray &contentType) override {
        Q_UNUSED(contentType)
        return QtJson::parseString(QString::fromUtf8(data), _config);
    }

private:
    QtJson::Configuration _config;
    QJsonDocument::JsonFormat _format;
};

template <>
class JsonContentHandler<QJsonValue> : public ContentHandler<QJsonValue>
{
public:
    using WriteResult = typename ContentHandler<QJsonValue>::WriteResult;

    JsonContentHandler(QJsonDocument::JsonFormat format = QJsonDocument::Indented) :
        _format{format}
    {}

    QByteArrayList contentTypes() const override {
        return {"application/json"};
    }

    WriteResult write(const QJsonValue &data) override {
        return std::make_pair(QtJson::writeJson(data, _format).toUtf8(), "application/json");
    }

    QJsonValue read(const QByteArray &data, const QByteArray &contentType) override {
        Q_UNUSED(contentType)
        return QtJson::readJson(QString::fromUtf8(data));
    }

private:
    QJsonDocument::JsonFormat _format;
};

}
