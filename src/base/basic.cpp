#include "basic.h"

namespace zh
{

/***********************************************
 * Error
 ***********************************************/

Error::Error(const wchar_t *_description)
{
    description = _description;
}

const wchar_t *Error::Description() const
{
    return description;
}

/***********************************************
 * NonCopyable
 ***********************************************/

NonCopyable::NonCopyable() {}

NonCopyable::NonCopyable(const NonCopyable &) {}

NonCopyable &NonCopyable::operator=(const NonCopyable &) { return *this; }
}
