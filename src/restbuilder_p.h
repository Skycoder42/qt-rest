#pragma once

#include "restbuilder.h"

namespace QtRest {

struct QTREST_EXPORT RestBuilderPrivate : public QSharedData
{
	QUrl baseUrl;
	QNetworkAccessManager *nam = nullptr;
};

}
