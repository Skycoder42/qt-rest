#pragma once

#include <QtCore/QUrl>
#include <QtCore/QScopedPointer>

#include <QtNetwork/QNetworkAccessManager>

namespace QtRest {

struct RestBuilderPrivate;
class QTREST_EXPORT RestBuilder
{
	Q_DISABLE_COPY(RestBuilder)

public:
	RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam = nullptr);
	RestBuilder(RestBuilder &&other) noexcept;
	RestBuilder &operator=(RestBuilder &&other) noexcept;
	virtual ~RestBuilder();

	RestBuilder &&setNetworkAccessManager(QNetworkAccessManager *nam) &&;

private:
	QScopedPointer<RestBuilderPrivate> d;
};

}

