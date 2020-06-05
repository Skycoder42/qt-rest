#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtNetwork/QNetworkRequest>

namespace QtRest {

using HeaderMap = QHash<QByteArray, QByteArray>;
using AttributeMap = QHash<QNetworkRequest::Attribute, QVariant>;

struct QTREST_EXPORT Verbs
{
    static const QByteArray GET;
    static const QByteArray POST;
    static const QByteArray PUT;
    static const QByteArray DELETE;
    static const QByteArray PATCH;
    static const QByteArray HEAD;
};

}

Q_DECLARE_METATYPE(QtRest::HeaderMap)
Q_DECLARE_METATYPE(QtRest::AttributeMap)
