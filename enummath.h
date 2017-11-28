#ifndef ENUMMATH_H
#define ENUMMATH_H

#include "valuetype.h"
#include<QIntegerForSize>

class EnumMath
{
public:
    EnumMath();
    static ValueType Min(ValueType a, ValueType b);
    static ValueType Max(ValueType a, ValueType b);
};

#endif // ENUMMATH_H
