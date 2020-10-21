#pragma once
#include "etl/limits.h"
#include "etl/string.h"
#include <stdint.h>
#include <ctype.h>

namespace etl
{
enum class string_to_real_err
{
    none,
    index_out_of_range,
    invalid_argument,
    exponent_out_of_range
};

template<typename real_t, typename TIString>
string_to_real_err string_to_real(real_t &result, TIString const& s, size_t idx=0)
{
    result = real_t(0);
    const size_t sz(s.size());
    if (idx >= sz)
    {
        return string_to_real_err::index_out_of_range;
    }

    char const* start = s.data() + idx;
    char const* end = start + sz - idx;
    char const* ptr = start;

    // skip leading whitespace
    while (iswspace(*ptr) && ptr != end)
        ++ptr;

    if (ptr == end)
    {
        return string_to_real_err::invalid_argument;
    }

    // get the sign
    int32_t sign = 1;
    if (*ptr == '-')
    {
        sign = -1;
        ++ptr;
    }
    if (*ptr == '+')
    {
        ++ptr;
    }

    int32_t int_part(0);
    // get digits before decimal point
    while (ptr != end && isdigit(*ptr))
    {
        int_part = (int_part * 10) + (*ptr++ - '0');
    }

    result = real_t(int_part * sign);

    if (ptr != end && *ptr == '.')
    {
        ++ptr;

        // get digits after decimal point
        int32_t frac_scale(1);
        int32_t frac_part(0);
        while (ptr != end && isdigit(*ptr))
        {
            frac_scale *= 10;
            frac_part = (frac_part * 10) + (*ptr++ - '0');
        }
        result += real_t(frac_part) / frac_scale;
    }

    if (ptr != end && (*ptr == 'e' || *ptr == 'E'))
    {
        ++ptr;
        // get exponent
        int esign = 1;
        if (*ptr == '-')
        {
            esign = -1;
            ++ptr;
        }
        if (*ptr == '+')
        {
            ++ptr;
        }

        if (ptr == end)
        {
            result = real_t(0);
            return string_to_real_err::invalid_argument;
        }

        int32_t exp(0);
        while (isdigit(*ptr) && ptr != end)
        {
            exp = (exp * 10) + (*ptr++ - '0');
        }

        if (esign < 0)
        {
            if (exp < etl::numeric_limits<real_t>::min_exponent10)
            {
                result = real_t(0);
                return string_to_real_err::exponent_out_of_range;
            }
        }
        else
        {
            if (exp > etl::numeric_limits<real_t>::max_exponent10)
            {
                result = real_t(0);
                return string_to_real_err::exponent_out_of_range;
            }
        }

        int32_t escale(1);
        while (exp--)
            escale *= 10;

        result = esign < 0 ? result / escale : result * escale;
    }

    return string_to_real_err::none;
}
}
