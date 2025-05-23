/////////////////////////////
// Enums.h
//

#ifndef ENUMS_H_CF6AD5B7_507F_4DE4_8980_A3940530D108__INCLUDED
#define ENUMS_H_CF6AD5B7_507F_4DE4_8980_A3940530D108__INCLUDED

namespace Calc
{
    // Declare a function pointer for the math functions.
    using PFun = double (*)(double);

    enum eToken
    {
        tNumber,
        tPlus,
        tMinus,
        tMultiply,
        tDivide,
        tPower,
        tVariable,
        tFunction,
        tEqual,
        tLeftParenth,
        tRightParenth,
        tUnknown,
        tEnd
    };

    enum eStatus
    {
        st_ERROR,
        st_OK,                  // Invalid sequence of numbers and operators
        st_OVERFLOW             // Infinity, or invalid function argument
    };

} // namespace Calc

#endif // ENUMS_H_CF6AD5B7_507F_4DE4_8980_A3940530D108__INCLUDED