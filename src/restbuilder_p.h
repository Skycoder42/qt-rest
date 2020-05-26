#pragma once

#include "restbuilder.h"

namespace QtRest {

struct QTREST_EXPORT RestBuilderPrivate : public QSharedData
{
    using HeaderMap = RestBuilder::HeaderMap;

	QUrl baseUrl;
	QNetworkAccessManager *nam = nullptr;
    QStringList pathSegments;
    bool trailingSlash = false;
    QUrlQuery query;
    QString fragment;
    HeaderMap headers;
};

}
