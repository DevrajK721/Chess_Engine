#include "move.h"
#include <string>

std::string Move::toString() const
{
    std::string result;
    result += char('a' + sourceSquare % 8);
    result += char('1' + sourceSquare / 8);
    result += char('a' + targetSquare % 8);
    result += char('1' + targetSquare / 8);
    if (isPromotion) {
        result += "q";
    }
    return result;
}