#include "restbuilder.h"
#include "restbuilder_data.h"
#include <QtCore/QBuffer>
using namespace QtRest;
using namespace QtRest::__private;

Q_LOGGING_CATEGORY(QtRest::__private::logBuilder, "qtrest.RestBuilder")

const QByteArray Verbs::GET = "GET";
const QByteArray Verbs::POST = "POST";
const QByteArray Verbs::PUT = "PUT";
const QByteArray Verbs::DELETE = "DELETE";
const QByteArray Verbs::PATCH = "PATCH";
const QByteArray Verbs::HEAD = "HEAD";

const QLatin1String RestBuilderData::AcceptHeader {"Accept"};
const QLatin1String RestBuilderData::ContentTypeHeader {"Content-Type"};
const QByteArray RestBuilderData::ContentTypeXml1 {"application/xml"};
const QByteArray RestBuilderData::ContentTypeXml2 {"text/xml"};
