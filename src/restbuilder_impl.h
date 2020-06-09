#pragma once

#include "restbuilder_decl.h"
#include "restbuilder_data.h"

#include "qtrest_exceptions.h"

namespace QtRest {

template <typename TBuilder>
RawRestBuilder<TBuilder>::RawRestBuilder() :
	d{new __private::RestBuilderData{}}
{}

template <typename TBuilder>
RawRestBuilder<TBuilder>::RawRestBuilder(QUrl baseUrl, QNetworkAccessManager *nam) :
	RawRestBuilder{}
{
	d->baseUrl = std::move(baseUrl);
	d->nam = nam;
}

template <typename TBuilder>
template <template <class> class THandler, typename... TArgs>
GenericRestBuilder<THandler> RawRestBuilder<TBuilder>::addContentTypeHandler(TArgs&&... args)
{
	return GenericRestBuilder<THandler>{
		d,
		std::tie(ContentHandlerArgs<THandler>{std::forward<TArgs>(args)...})
	};
}


template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setNetworkAccessManager(QNetworkAccessManager *nam)
{
	d->nam = nam;
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setBaseUrl(QUrl baseUrl)
{
	d->baseUrl = std::move(baseUrl);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setScheme(const QString &scheme)
{
	d->baseUrl.setScheme(scheme);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setUser(const QString &user)
{
	d->baseUrl.setUserName(user);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setPassword(const QString &password)
{
	d->baseUrl.setPassword(password);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setCredentials(const QString &user, const QString &password)
{
	return setUser(user).setPassword(password);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setHost(const QString &host)
{
	d->baseUrl.setHost(host);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setPort(quint16 port)
{
	d->baseUrl.setPort(port);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPath(const QString &pathSegment)
{
	d->pathSegments.append(pathSegment);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPath(const QVersionNumber &version, VersionFlags versionFlags)
{
	auto pathSegment = versionFlags.testFlag(VersionFlag::Normalize) ?
		version.normalized().toString() :
		version.toString();
	if (versionFlags.testFlag(VersionFlag::UseVPrefix))
		pathSegment.prepend(QLatin1Char('v'));
	return addPath(pathSegment);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPath(const QStringList &pathSegments)
{
	d->pathSegments.append(pathSegments);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::trailingSlash(bool enable)
{
	d->trailingSlash = enable;
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addParameter(const QString &name, QVariant value)
{
	if (const auto oldType = value.userType(); !value.convert(QMetaType::QString))
		throw UnconvertibleVariantException{oldType, value.userType()};
	d->query.addQueryItem(name, value.toString());
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
template <typename T>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addParameter(const QString &name, const T &value)
{
	return addParameter(name, QVariant::fromValue(value));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addParameters(QUrlQuery parameters, bool replace)
{
	if (replace)
		d->query = std::move(parameters);
	else {
		for (const auto &item : parameters.queryItems())
			d->query.addQueryItem(item.first, item.second);
	}
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setFragment(QVariant fragment)
{
	if (const auto oldType = fragment.userType(); !fragment.convert(QMetaType::QString))
		throw UnconvertibleVariantException{oldType, fragment.userType()};
	d->fragment = fragment.toString();
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
template <typename T>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setFragment(const T &fragment)
{
	return setFragment(QVariant::fromValue(fragment));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addHeader(const QLatin1String &name, QVariant value)
{
	if (const auto oldType = value.userType(); !value.convert(QMetaType::QString))
		throw UnconvertibleVariantException{oldType, value.userType()};
	d->headers.insert(name.latin1(), value.toString().toLatin1());
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
template <typename T>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addHeader(const QLatin1String &name, const T &value)
{
	return addHeader(name, QVariant::fromValue(value));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addHeaders(HeaderMap headers, bool replace)
{
	if (replace)
		d->headers = std::move(headers);
	else {
		for (auto it = headers.begin(), end = headers.end(); it != end; ++it)
			d->headers.insert(it.key(), it.value());
	}
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAccept(const QByteArray &mimeType)
{
	return addHeader(__private::RestBuilderData::AcceptHeader, mimeType);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAccept(const QByteArrayList &mimeTypes)
{
	return addHeader(__private::RestBuilderData::AcceptHeader, mimeTypes.join(", "));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAccept(const QMimeType &mimeType)
{
	return setAccept(mimeType.name().toUtf8());
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAccept(const QList<QMimeType> &mimeTypes)
{
	QByteArrayList values;
	values.reserve(mimeTypes.size());
	for (const auto &mimeType : mimeTypes)
		values.append(mimeType.name().toUtf8());
	return setAccept(values);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::updateFromRelativeUrl(const QUrl &url, MergeFlags mergeFlags)
{
	auto cUrl = buildUrl();
	d->baseUrl = cUrl.resolved(url);
	if (d->baseUrl.host() != cUrl.host()) {
		qCWarning(__private::logBuilder) << "URL host changed from"
										 << cUrl.host()
										 << "to"
										 << d->baseUrl.host();
	}

	//clear all the rest
	d->pathSegments.clear();
	if (mergeFlags.testFlag(MergeFlag::MergeQuery)) {
		QUrlQuery query{url.query()};
		for (const auto &item : query.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
			d->query.addQueryItem(item.first, item.second);
	} else
		d->query = QUrlQuery{url.query()};

	if (!mergeFlags.testFlag(MergeFlag::KeepFragment))
		d->fragment.clear();

	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
	d->attributes.insert(attribute, value);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
template <typename T>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAttribute(QNetworkRequest::Attribute attribute, const T &value)
{
	return setAttribute(attribute, QVariant::fromValue(value));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setAttributes(AttributeMap attributes, bool replace)
{
	if (replace)
		d->attributes = std::move(attributes);
	else {
		for (auto it = attributes.begin(), end = attributes.end(); it != end; ++it)
			d->attributes.insert(it.key(), it.value());
	}
	return *static_cast<Builder*>(this);
}

#ifndef QT_NO_SSL
template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setSslConfig(QSslConfiguration sslConfig)
{
	d->sslConfig = std::move(sslConfig);
	return *static_cast<Builder*>(this);
}
#endif

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setBody(QByteArray body, const QByteArray &contentType, bool setAccept)
{
	d->body = body;
	if (setAccept)
		this->setAccept(contentType);
	return addHeader(__private::RestBuilderData::ContentTypeHeader, contentType);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setBody(QByteArray body, const QMimeType &contentType, bool setAccept)
{
	d->body = body;
	if (setAccept)
		this->setAccept(contentType);
	return addHeader(__private::RestBuilderData::ContentTypeHeader, contentType.name().toUtf8());
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setBody(QIODevice *body, const QByteArray &contentType, bool setAccept)
{
	d->body = body;
	if (setAccept)
		this->setAccept(contentType);
	return addHeader(__private::RestBuilderData::ContentTypeHeader, contentType);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setBody(QIODevice *body, const QMimeType &contentType, bool setAccept)
{
	d->body = body;
	if (setAccept)
		this->setAccept(contentType);
	return addHeader(__private::RestBuilderData::ContentTypeHeader, contentType.name().toUtf8());
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::setVerb(QByteArray verb)
{
	d->verb = std::move(verb);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPostParameter(const QString &name, QVariant value)
{
	if (const auto oldType = value.userType(); !value.convert(QMetaType::QString))
		throw UnconvertibleVariantException{oldType, value.userType()};
	if (!std::holds_alternative<QUrlQuery>(d->body))
		d->body = QUrlQuery{};
	std::get<QUrlQuery>(d->body).addQueryItem(name, value.toString());
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
template <typename T>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPostParameter(const QString &name, const T &value)
{
	return addPostParameter(name, QVariant::fromValue(value));
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::addPostParameters(QUrlQuery parameters, bool replace)
{
	if (replace)
		d->body = std::move(parameters);
	else {
		if (!std::holds_alternative<QUrlQuery>(d->body))
			d->body = QUrlQuery{};
		for (const auto &item : parameters.queryItems())
			std::get<QUrlQuery>(d->body).addQueryItem(item.first, item.second);
	}
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
typename RawRestBuilder<TBuilder>::Builder &RawRestBuilder<TBuilder>::onResult(std::function<void(RawRestReply)> callback)
{
	d->resultCallback = std::move(callback);
	return *static_cast<Builder*>(this);
}

template <typename TBuilder>
QUrl RawRestBuilder<TBuilder>::buildUrl() const
{
	auto url = d->baseUrl;

	auto pathList = url.path().split(QLatin1Char('/'), QString::SkipEmptyParts);
	pathList.append(d->pathSegments);
	url.setPath(QLatin1Char('/') +
				pathList.join(QLatin1Char('/')) +
				(d->trailingSlash ? QStringLiteral("/") : QString{}));

	if (!d->query.isEmpty())
		url.setQuery(d->query);
	if (!d->fragment.isNull())
		url.setFragment(d->fragment);

	qCDebug(__private::logBuilder) << "Built URL as"
								   << url.toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo);

	return url;
}

template <typename TBuilder>
QNetworkRequest RawRestBuilder<TBuilder>::build() const
{
	QNetworkRequest request{buildUrl()};

	for (auto it = d->headers.constBegin(); it != d->headers.constEnd(); it++)
		request.setRawHeader(it.key(), it.value());
	for (auto it = d->attributes.constBegin(); it != d->attributes.constEnd(); it++)
		request.setAttribute(it.key(), it.value());
#ifndef QT_NO_SSL
	request.setSslConfiguration(d->sslConfig);
#endif

	qCDebug(__private::logBuilder) << "Created request with headers"
								   << d->headers.keys()
								   << "and attributes" << d->attributes.keys();

	return request;
}

template <typename TBuilder>
QNetworkReply *RawRestBuilder<TBuilder>::send(QObject *context) const
{
	QNetworkRequest request{buildUrl()};
	const auto reply = std::visit(__private::SendBodyVisitor{request, d->verb, d->nam}, d->body);
	if (d->resultCallback) {
		QObject::connect(reply, &QNetworkReply::finished,
						 context ? context : reply,
						 [cb = d->resultCallback, reply]() {
							 cb(RawRestReply{reply});
						 });
	}
	return reply;
}

#ifdef QT_REST_USE_ASYNC
template<typename TBuilder>
QFuture<RawRestReply> RawRestBuilder<TBuilder>::sendAsync() const
{
	Q_ASSERT_X(!d->resultCallback, Q_FUNC_INFO, "Cannot use result callback with sendAsync");
	QFutureInterface<RawRestReply> fi;
	fi.reportStarted();
	onResult([fi](const RawRestReply &reply) mutable {
		fi.reportFinished(&reply);
	});
	send();
	return QFuture<RawRestReply>{&fi};
}
#endif

template<typename TBuilder>
RawRestBuilder<TBuilder>::RawRestBuilder(const QSharedDataPointer<__private::RestBuilderData> &d) :
	d{d}
{}



template <template <class> class... THandlers>
template <template <class> class THandler, typename... TArgs>
GenericRestBuilder<THandlers..., THandler> GenericRestBuilder<THandlers...>::addContentTypeHandler(TArgs&&... args)
{
	return GenericRestBuilder<THandler>{
		this->d,
		std::tuple_cat(_contentHandlerArgs, std::tie(ContentHandlerArgs<THandler>{std::forward<TArgs>(args)...}))
	};
}

template <template <class> class... THandlers>
template <template <class> class THandler, typename T>
typename GenericRestBuilder<THandlers...>::Builder &GenericRestBuilder<THandlers...>::setBody(T &&body, bool setAccept)
{
	static_assert (std::disjunction_v<std::is_same<THandler<T>, THandlers<T>>...>, "THandler must be one of the registered content handlers");
	THandler<T> handler {std::get<ContentHandlerArgs<THandler>>(_contentHandlerArgs)};
	auto [data, contentType] = handler.write(std::forward<T>(body));
	if constexpr (THandler<T>::IsStringHandler)
		return setBody(data.toUtf8(), std::move(contentType), setAccept);
	else
		return setBody(std::move(data), std::move(contentType), setAccept);
}

template <template <class> class... THandlers>
typename GenericRestBuilder<THandlers...>::Builder &GenericRestBuilder<THandlers...>::onResult(std::function<void(RestReply)> callback)
{
	return onResult([args = _contentHandlerArgs, callback](const RawRestReply &reply) {
		callback(reply.toGeneric<THandlers...>(std::tuple<ContentHandlerArgs<THandlers>...>{args}));
	});
}

#ifdef QT_REST_USE_ASYNC
template <template <class> class... THandlers>
QFuture<RestReply<THandlers...>> GenericRestBuilder<THandlers...>::sendAsync() const
{
	Q_ASSERT_X(!this->d->resultCallback, Q_FUNC_INFO, "Cannot use result callback with sendAsync");
	QFutureInterface<RestReply<THandlers...>> fi;
	fi.reportStarted();
	onResult([fi](const RestReply<THandlers...> &reply) mutable {
		fi.reportFinished(&reply);
	});
	this->send();
	return QFuture<RestReply<THandlers...>>{&fi};
}
#endif

template <template <class> class... THandlers>
GenericRestBuilder<THandlers...>::GenericRestBuilder(const QSharedDataPointer<__private::RestBuilderData> &d, std::tuple<ContentHandlerArgs<THandlers>...> &&contentHandlerArgs) :
	RawRestReply{d},
	_contentHandlerArgs(std::move(contentHandlerArgs))
{}

}
