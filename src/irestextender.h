#pragma once

#include <variant>

#include <QtCore/QtGlobal>
#include <QtCore/QUrl>
#include <QtCore/QIODevice>
#include <QtCore/QUrlQuery>

#include <QtNetwork/QNetworkRequest>

namespace QtRest {

class QTREST_EXPORT IRestExtender
{
	Q_DISABLE_COPY(IRestExtender);

public:
	IRestExtender();
	virtual ~IRestExtender();

	virtual void extendUrl(QUrl &url) const = 0;
	virtual void extendRequest(QNetworkRequest &request) const = 0;
	virtual void extendSend(QByteArray &verb, std::variant<QByteArray, QIODevice*, QUrlQuery> &body) const = 0;
};


}
