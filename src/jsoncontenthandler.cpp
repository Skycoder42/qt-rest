#include "jsoncontenthandler.h"
using namespace QtRest;

const QByteArray ContentHandlerArgs<JsonContentHandler>::ContentType = "application/json";



JsonContentHandler<QJsonValue>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList JsonContentHandler<QJsonValue>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

JsonContentHandler<QJsonValue>::WriteResult JsonContentHandler<QJsonValue>::write(const QJsonValue &data)
{
    return std::make_pair(QtJson::writeJson(data, _format),
                          ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonValue JsonContentHandler<QJsonValue>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    return QtJson::readJson(data);
}



JsonContentHandler<QJsonObject>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList JsonContentHandler<QJsonObject>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

JsonContentHandler<QJsonObject>::WriteResult JsonContentHandler<QJsonObject>::write(const QJsonObject &data)
{
    return std::make_pair(QtJson::writeJson(data, _format), ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonObject JsonContentHandler<QJsonObject>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    const auto json = QtJson::readJson(data);
    if (!json.isObject())
        throw QtJson::InvalidValueTypeException{json.type(), {QJsonValue::Object}};
    return json.toObject();
}



JsonContentHandler<QJsonArray>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList JsonContentHandler<QJsonArray>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

JsonContentHandler<QJsonArray>::WriteResult JsonContentHandler<QJsonArray>::write(const QJsonArray &data)
{
    return std::make_pair(QtJson::writeJson(data, _format), ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonArray JsonContentHandler<QJsonArray>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    const auto json = QtJson::readJson(data);
    if (!json.isArray())
        throw QtJson::InvalidValueTypeException{json.type(), {QJsonValue::Array}};
    return json.toArray();
}
