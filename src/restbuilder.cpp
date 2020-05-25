#include "restbuilder.h"
#include "restbuilder_p.h"
using namespace QtRest;

RestBuilder::RestBuilder() :
    d{new RestBuilderPrivate{}}
{}

RestBuilder::RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam) :
    RestBuilder{}
{
	d->baseUrl = std::move(baseUrl);
    d->nam = nam;
}

RestBuilder::RestBuilder(const RestBuilder &other) = default;

RestBuilder::RestBuilder(RestBuilder &&other) noexcept = default;

RestBuilder &RestBuilder::operator=(const RestBuilder &other) = default;

RestBuilder &RestBuilder::operator=(RestBuilder &&other) noexcept = default;

RestBuilder::~RestBuilder() = default;

RestBuilder &RestBuilder::setNetworkAccessManager(QNetworkAccessManager *nam)
{
	d->nam = nam;
    return *this;
}

RestBuilder &RestBuilder::setBaseUrl(QUrl baseUrl)
{
    d->baseUrl = std::move(baseUrl);
    return *this;
}
