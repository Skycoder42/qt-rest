#pragma once

#include "restbuilder_decl.h"

#include <variant>

#include <QtCore/QLoggingCategory>
#ifdef QT_REST_USE_ASYNC
#include <QtCore/QRunnable>
#endif

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

#ifdef QT_REST_USE_ASYNC
class QTREST_EXPORT RawRestReplyRunnable : public QRunnable
{
public:
	RawRestReplyRunnable(std::function<void(RawRestReply)> callback,
						 RawRestReply &&reply);
	void run() override;

private:
	std::function<void(RawRestReply)> _callback;
	RawRestReply _reply;
};

template <template <class> class... THandlers>
class RestReplyRunnable : public QRunnable
{
public:
	inline RestReplyRunnable(std::function<void(RestReply<THandlers...>)> callback,
					  RestReply<THandlers...> &&reply) :
		_callback{std::move(callback)},
		_reply{std::move(reply)}
	{}

	void run() override {
		_callback(_reply);
	}

private:
	std::function<void(RestReply<THandlers...>)> _callback;
	RestReply<THandlers...> _reply;
};
#endif

Q_DECLARE_LOGGING_CATEGORY(logBuilder)

}
