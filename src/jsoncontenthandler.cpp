#include "jsoncontenthandler.h"
using namespace QtRest;

QtRest::JsonContentHandler<QJsonValue>::JsonContentHandler(QJsonDocument::JsonFormat format) :
    _format{format}
{}

QByteArrayList QtRest::JsonContentHandler<QJsonValue>::contentTypes() const
{
    return {"application/json"};
}

QtRest::JsonContentHandler<QJsonValue>::WriteResult QtRest::JsonContentHandler<QJsonValue>::write(const QJsonValue &data)
{
    return std::make_pair(QtJson::writeJson(data, _format).toUtf8(), "application/json");
}

QJsonValue QtRest::JsonContentHandler<QJsonValue>::read(const QString &data, const QByteArray &contentType)
{
    Q_UNUSED(contentType)
    return QtJson::readJson(data);
}
