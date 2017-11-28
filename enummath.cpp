#include "enummath.h"

EnumMath::EnumMath()
{
}

ValueType EnumMath::Min(ValueType a, ValueType b)
{
    qint32 ai = static_cast<qint32>(a);
    qint32 bi = static_cast<qint32>(b);
    if(ai<bi)
        return static_cast<ValueType>(ai);
    else
        return static_cast<ValueType>(bi);
}

ValueType EnumMath::Max(ValueType a, ValueType b)
{
    qint32 ai = static_cast<qint32>(a);
    qint32 bi = static_cast<qint32>(b);
    if(ai>bi)
        return static_cast<ValueType>(ai);
    else
        return static_cast<ValueType>(bi);
}
