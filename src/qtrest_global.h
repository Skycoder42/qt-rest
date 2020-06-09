#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtNetwork/QNetworkRequest>

#if QT_CONFIG(thread) && QT_CONFIG(future)
#define QT_REST_USE_ASYNC
#endif

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

#define Q_DECLARE_GENERIC_OPERATORS_FOR_FLAGS(Flags, ...) \
	template <__VA_ARGS__> \
	Q_DECL_CONSTEXPR inline QFlags<typename Flags::enum_type> operator|(typename Flags::enum_type f1, typename Flags::enum_type f2) noexcept \
	{ return QFlags<typename Flags::enum_type>(f1) | f2; } \
	template <__VA_ARGS__> \
	Q_DECL_CONSTEXPR inline QFlags<typename Flags::enum_type> operator|(typename Flags::enum_type f1, QFlags<typename Flags::enum_type> f2) noexcept \
	{ return f2 | f1; } \
	template <__VA_ARGS__> \
	Q_DECL_CONSTEXPR inline QIncompatibleFlag operator|(typename Flags::enum_type f1, int f2) noexcept \
	{ return QIncompatibleFlag(int(f1) | f2); }
