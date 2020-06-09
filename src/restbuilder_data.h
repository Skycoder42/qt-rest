#pragma once

#include "restbuilder_decl.h"

#include <variant>

#include <QtCore/QLoggingCategory>

namespace QtRest::__private {

class QTREST_EXPORT SendBodyVisitor
{
public:
	SendBodyVisitor(QNetworkRequest &request,
					const QByteArray &verb,
					QNetworkAccessManager *nam);

	QNetworkReply *operator()(const QByteArray &data) const;
	QNetworkReply *operator()(QIODevice *device) const;
	QNetworkReply *operator()(const QUrlQuery &postParams) const;

private:
	QNetworkRequest &request;
	const QByteArray &verb;
	QNetworkAccessManager *nam;
};

struct QTREST_EXPORT RestBuilderData : public QSharedData
{

	static const QLatin1String AcceptHeader;
	static const QLatin1String ContentTypeHeader;
	static const QByteArray ContentTypeUrlEncoded;

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
	std::function<void(RawRestReply)> resultCallback;

#ifndef QT_NO_SSL
	QSslConfiguration sslConfig;
#endif
};

Q_DECLARE_LOGGING_CATEGORY(logBuilder)

}
