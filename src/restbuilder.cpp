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


SendBodyVisitor::SendBodyVisitor(QNetworkRequest &request, const QByteArray &verb, QNetworkAccessManager *nam) :
	request{request},
	verb{verb},
	nam{nam}
{}

QNetworkReply *SendBodyVisitor::operator()(const QByteArray &data) const
{
	if (data.isEmpty())
		return nam->sendCustomRequest(request, verb);
	else
		return nam->sendCustomRequest(request, verb, data);
}

QNetworkReply *SendBodyVisitor::operator()(QIODevice *device) const
{
	return nam->sendCustomRequest(request, verb, device);
}

QNetworkReply *SendBodyVisitor::operator()(const QUrlQuery &postParams) const
{
	request.setRawHeader(RestBuilderData::ContentTypeHeader.latin1(),
						 RestBuilderData::ContentTypeUrlEncoded);
	return nam->sendCustomRequest(request, verb, postParams.query().toUtf8());
}
