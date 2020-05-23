#include "restbuilder.h"
#include "restbuilder_p.h"
using namespace QtRest;

RestBuilder::RestBuilder(QUrl baseUrl, QNetworkAccessManager *nam) :
	d{new RestBuilderPrivate{}}
{
	d->baseUrl = std::move(baseUrl);
	d->nam = nam;
}

RestBuilder::RestBuilder(RestBuilder &&other) noexcept
{
	d.swap(other.d);
}

RestBuilder &RestBuilder::operator=(RestBuilder &&other) noexcept
{
	d.swap(other.d);
	return *this;
}

RestBuilder::~RestBuilder()
{

}

RestBuilder &&RestBuilder::setNetworkAccessManager(QNetworkAccessManager *nam) &&
{
	d->nam = nam;
	return std::move(*this);
}
