#pragma once

#include "restbuilder.h"

#include <variant>

#include <QtCore/QLoggingCategory>

namespace QtRest {

struct QTREST_EXPORT RestBuilderPrivate : public QSharedData
{
    static const QLatin1String AcceptHeader;

    static const QLatin1String ContentTypeHeader;
    static const QByteArray ContentTypeJson;
    static const QByteArray ContentTypeCbor;
    static const QByteArray ContentTypeXml1;
    static const QByteArray ContentTypeXml2;

    QNetworkAccessManager *nam = nullptr;
    QUrl baseUrl;
    QStringList pathSegments;
    bool trailingSlash = false;
    QUrlQuery query;
    QString fragment;
    HeaderMap headers;
    AttributeMap attributes;
    std::variant<QByteArray, QIODevice*, QUrlQuery> body;
    QByteArray verb = Verbs::GET;

#ifndef QT_NO_SSL
    QSslConfiguration sslConfig;
#endif
};

Q_DECLARE_LOGGING_CATEGORY(logBuilder)

}
