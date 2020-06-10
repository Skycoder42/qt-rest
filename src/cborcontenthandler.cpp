#include "cborcontenthandler.h"
using namespace QtRest;

const QByteArray ContentHandlerArgs<CborContentHandler>::ContentType = "application/cbor";



CborContentHandler<QCborValue>::CborContentHandler(ContentHandlerArgs<CborContentHandler> args) :
    _options{std::move(args.options)}
{}

QByteArrayList CborContentHandler<QCborValue>::contentTypes() const
{
    return {ContentHandlerArgs<CborContentHandler>::ContentType};
}

CborContentHandler<QCborValue>::WriteResult QtRest::CborContentHandler<QCborValue>::write(const QCborValue &data)
{
    return std::make_pair(QtJson::writeCbor(data, _options),
                          ContentHandlerArgs<CborContentHandler>::ContentType);
}

QCborValue CborContentHandler<QCborValue>::read(const QByteArray &data, const QByteArray &contentType, QTextCodec *)
{
    Q_UNUSED(contentType)
    return QtJson::readCbor(data);
}



CborContentHandler<QCborMap>::CborContentHandler(ContentHandlerArgs<CborContentHandler> args) :
    _options{std::move(args.options)}
{}

QByteArrayList CborContentHandler<QCborMap>::contentTypes() const
{
    return {ContentHandlerArgs<CborContentHandler>::ContentType};
}

CborContentHandler<QCborMap>::WriteResult QtRest::CborContentHandler<QCborMap>::write(const QCborMap &data)
{
    return std::make_pair(QtJson::writeCbor(data, _options),
                          ContentHandlerArgs<CborContentHandler>::ContentType);
}

QCborMap CborContentHandler<QCborMap>::read(const QByteArray &data, const QByteArray &contentType, QTextCodec *)
{
    Q_UNUSED(contentType)
    const auto cbor = QtJson::readCbor(data);
    if (!cbor.isMap())
        throw QtJson::InvalidValueTypeException{cbor.type(), {QCborValue::Map}};
    return cbor.toMap();
}



CborContentHandler<QCborArray>::CborContentHandler(ContentHandlerArgs<CborContentHandler> args) :
    _options{std::move(args.options)}
{}

QByteArrayList CborContentHandler<QCborArray>::contentTypes() const
{
    return {ContentHandlerArgs<CborContentHandler>::ContentType};
}

CborContentHandler<QCborArray>::WriteResult QtRest::CborContentHandler<QCborArray>::write(const QCborArray &data)
{
    return std::make_pair(QtJson::writeCbor(data, _options),
                          ContentHandlerArgs<CborContentHandler>::ContentType);
}

QCborArray CborContentHandler<QCborArray>::read(const QByteArray &data, const QByteArray &contentType, QTextCodec *)
{
    Q_UNUSED(contentType)
    const auto cbor = QtJson::readCbor(data);
    if (!cbor.isArray())
        throw QtJson::InvalidValueTypeException{cbor.type(), {QCborValue::Array}};
    return cbor.toArray();
}
