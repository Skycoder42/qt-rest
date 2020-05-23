#pragma once

#include "restbuilder.h"

namespace QtRest {

struct QTREST_EXPORT RestBuilderPrivate
{
	QUrl baseUrl;
	QNetworkAccessManager *nam = nullptr;
};

}
