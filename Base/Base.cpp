#include "Base.h"

//////////////////////////////////////////////////////////////////////////
STATIC_ASSERT(sizeof(tint8) == 1);
STATIC_ASSERT(sizeof(tuint8) == 1);
STATIC_ASSERT(sizeof(tchar) == 1);
STATIC_ASSERT(sizeof(tbyte) == 1);

STATIC_ASSERT(sizeof(tint16) == 2);
STATIC_ASSERT(sizeof(tuint16) == 2);
STATIC_ASSERT(sizeof(tword) == 2);

STATIC_ASSERT(sizeof(tint32) == 4);
STATIC_ASSERT(sizeof(tuint32) == 4);
STATIC_ASSERT(sizeof(tdword) == 4);

STATIC_ASSERT(sizeof(tint64) == 8);
STATIC_ASSERT(sizeof(tuint64) == 8);

STATIC_ASSERT(sizeof(tfloat32) == 4);
STATIC_ASSERT(sizeof(tfloat64) == 8);

STATIC_ASSERT(sizeof(tintptr) == sizeof(void *));
STATIC_ASSERT(sizeof(tfloatptr) == sizeof(void *));

STATIC_ASSERT(sizeof(tchar *) == sizeof(void *));
STATIC_ASSERT(sizeof(const tchar *) == sizeof(void *));

//////////////////////////////////////////////////////////////////////////
STATIC_ASSERT(sizeof(time_t) == 8);
STATIC_ASSERT(sizeof(size_t) == 8);

//////////////////////////////////////////////////////////////////////////
STATIC_ASSERT(sizeof(Guid64) == 8);
