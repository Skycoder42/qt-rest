#pragma once

#include <optional>

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

#include <QtNetwork/QNetworkReply>

#if !defined(QT_NO_EXCEPTIONS) && QT_CONFIG(future)
#include <QtCore/qexception.h>
namespace QtRest {
using ExceptionBase = QException;
}
#else
#include <exception>
namespace QtRest {
using ExceptionBase = std::exception;
}
#endif

namespace QtRest {

class QTREST_EXPORT Exception : public ExceptionBase
{
public:
    char const* what() const noexcept final;

    virtual void raise() const;
    virtual ExceptionBase *clone() const;

protected:
    Exception(QByteArray what);

private:
    QByteArray _what;
};

class QTREST_EXPORT MissingContentHandlerException : public Exception
{
public:
    MissingContentHandlerException(const QByteArray &contentType);

    void raise() const override;
    ExceptionBase *clone() const override;
};

class QTREST_EXPORT UnconvertibleVariantException : public Exception
{
public:
    UnconvertibleVariantException(int oldType, int newType);

    void raise() const override;
    ExceptionBase *clone() const override;
};

class QTREST_EXPORT UnsupportedCodecException : public Exception
{
public:
    UnsupportedCodecException(const QByteArray &codec);

    void raise() const override;
    ExceptionBase *clone() const override;
};

template <typename TError>
class QTREST_EXPORT RequestFailedException : public Exception
{
public:
    RequestFailedException(int statusCode = 400,
                           QNetworkReply::NetworkError error = QNetworkReply::UnknownNetworkError,
                           std::optional<TError> body = std::nullopt);

    int statusCode() const;
    QNetworkReply::NetworkError error() const;
    std::optional<TError> body() const;

    void raise() const override;
    ExceptionBase *clone() const override;

private:
    int _statusCode;
    QNetworkReply::NetworkError _error;
    std::optional<TError> _body;
};



template<typename TError>
RequestFailedException<TError>::RequestFailedException(int statusCode, QNetworkReply::NetworkError error, std::optional<TError> body) :
    Exception {
        QByteArrayLiteral("Network request failed with status code ") +
        QByteArray::number(statusCode) +
        QByteArrayLiteral(" (Network error: ") +
        QByteArray::number(error) +
        body ?
            (QByteArrayLiteral(") - Error content: ") +
             QVariant{*body}.toString().toUtf8()) :
            QByteArrayLiteral(")")
    },
    _statusCode{statusCode},
    _error{error},
    _body{std::move(body)}
{}

template<typename TError>
int RequestFailedException<TError>::statusCode() const
{
    return _statusCode;
}

template<typename TError>
QNetworkReply::NetworkError RequestFailedException<TError>::error() const
{
    return _error;
}

template<typename TError>
std::optional<TError> RequestFailedException<TError>::body() const
{
    return _body;
}

template<typename TError>
void RequestFailedException<TError>::raise() const
{
    throw *this;
}

template<typename TError>
QtRest::ExceptionBase *RequestFailedException<TError>::clone() const
{
    return new RequestFailedException{*this};
}

}
