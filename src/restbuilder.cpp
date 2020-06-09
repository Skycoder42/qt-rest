#include "restbuilder.h"
#include "restbuilder_data.h"
#include <QtCore/QBuffer>
using namespace QtRest;
using namespace QtRest::__private;

Q_LOGGING_CATEGORY(QtRest::__private::logBuilder, "qtrest.RestBuilder", QtInfoMsg)

const QByteArray Verbs::GET = "GET";
const QByteArray Verbs::POST = "POST";
const QByteArray Verbs::PUT = "PUT";
const QByteArray Verbs::DELETE = "DELETE";
const QByteArray Verbs::PATCH = "PATCH";
const QByteArray Verbs::HEAD = "HEAD";

const QLatin1String RestBuilderData::AcceptHeader {"Accept"};
const QLatin1String RestBuilderData::ContentTypeHeader {"Content-Type"};
const QByteArray RestBuilderData::ContentTypeUrlEncoded {"application/x-www-form-urlencoded"};


SendBodyVisitor::SendBodyVisitor(QNetworkRequest &&request, const QByteArray &verb, QNetworkAccessManager *nam) :
	request{std::move(request)},
	verb{verb},
	nam{nam}
{}

QNetworkReply *SendBodyVisitor::operator()(const QByteArray &data)
{
	if (data.isEmpty())
		return nam->sendCustomRequest(request, verb);
	else
		return nam->sendCustomRequest(request, verb, data);
}

QNetworkReply *SendBodyVisitor::operator()(QIODevice *device)
{
	return nam->sendCustomRequest(request, verb, device);
}

QNetworkReply *SendBodyVisitor::operator()(const QUrlQuery &postParams)
{
	request.setRawHeader(RestBuilderData::ContentTypeHeader.latin1(),
						 RestBuilderData::ContentTypeUrlEncoded);
	return nam->sendCustomRequest(request, verb, postParams.query().toUtf8());
}



RawRestReplyRunnable::RawRestReplyRunnable(std::function<void(RawRestReply)> callback, RawRestReply &&reply) :
	_callback{std::move(callback)},
	_reply{std::move(reply)}
{}

void RawRestReplyRunnable::run()
{
	_callback(_reply);
}
