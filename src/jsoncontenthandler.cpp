#include "jsoncontenthandler.h"
using namespace QtRest;

const QByteArray ContentHandlerArgs<JsonContentHandler>::ContentType = "application/json";

QtRest::JsonContentHandler<QJsonValue>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList QtRest::JsonContentHandler<QJsonValue>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

QtRest::JsonContentHandler<QJsonValue>::WriteResult QtRest::JsonContentHandler<QJsonValue>::write(const QJsonValue &data)
{
    return std::make_pair(QtJson::writeJson(data, _format), ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonValue QtRest::JsonContentHandler<QJsonValue>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    return QtJson::readJson(data);
}



QtRest::JsonContentHandler<QJsonObject>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList QtRest::JsonContentHandler<QJsonObject>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

QtRest::JsonContentHandler<QJsonObject>::WriteResult QtRest::JsonContentHandler<QJsonObject>::write(const QJsonObject &data)
{
    return std::make_pair(QtJson::writeJson(data, _format), ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonObject QtRest::JsonContentHandler<QJsonObject>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    return QtJson::readJson(data).toObject();  // TODO assert is object
}



QtRest::JsonContentHandler<QJsonArray>::JsonContentHandler(ContentHandlerArgs<JsonContentHandler> args) :
    _format{std::move(args.format)}
{}

QByteArrayList QtRest::JsonContentHandler<QJsonArray>::contentTypes() const
{
    return {ContentHandlerArgs<JsonContentHandler>::ContentType};
}

QtRest::JsonContentHandler<QJsonArray>::WriteResult QtRest::JsonContentHandler<QJsonArray>::write(const QJsonArray &data)
{
    return std::make_pair(QtJson::writeJson(data, _format), ContentHandlerArgs<JsonContentHandler>::ContentType);
}

QJsonArray QtRest::JsonContentHandler<QJsonArray>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    return QtJson::readJson(data).toArray();  // TODO assert is object
}
