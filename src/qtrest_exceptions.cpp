#include "qtrest_exceptions.h"
#include <QtCore/QMetaType>
using namespace QtRest;

#define DEFINE_EXCEPTION_METHODS(Type) \
	void Type::raise() const \
	{ \
		throw *this; \
	} \
	 \
	ExceptionBase *Type::clone() const \
	{ \
		return new Type{*this}; \
	}

const char *Exception::what() const noexcept
{
	return _what.constData();
}

DEFINE_EXCEPTION_METHODS(Exception)

Exception::Exception(QByteArray what) :
	_what{std::move(what)}
{}



MissingContentHandlerException::MissingContentHandlerException(const QByteArray &contentType) :
	Exception {
		QByteArrayLiteral("No content handler found that can handle the Content-Type \"") +
		contentType +
		'"'
	}
{}

DEFINE_EXCEPTION_METHODS(MissingContentHandlerException)



UnconvertibleVariantException::UnconvertibleVariantException(int oldType, int newType) :
	Exception {
		QByteArrayLiteral("Unable to convert QVariant of type \"") +
		QMetaType::typeName(oldType) +
		QByteArrayLiteral("\" to target type \"") +
		QMetaType::typeName(newType) +
		QByteArrayLiteral("\" - register QVariant conversions via QMetaType::registerConverter!")
	}
{}

DEFINE_EXCEPTION_METHODS(UnconvertibleVariantException)



UnsupportedCodecException::UnsupportedCodecException(const QByteArray &codec) :
	Exception {
		QByteArrayLiteral("Unsupported content type codec \"") +
		codec +
		'"'
	}
{}

DEFINE_EXCEPTION_METHODS(UnsupportedCodecException)
