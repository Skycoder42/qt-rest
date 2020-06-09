#pragma once

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

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

}
