
#ifndef MUMYSQL_STRING_FORMAT_H
#define MUMYSQL_STRING_FORMAT_H

#include "fmt/fmt/printf.h"

    /// Default TC string format function.
    template<typename Format, typename... Args>
    inline std::string StringFormat(Format&& fmt, Args&&... args)
    {
        return fmt::sprintf(std::forward<Format>(fmt), std::forward<Args>(args)...);
    }

    /// Returns true if the given char pointer is null.
    inline bool IsFormatEmptyOrNull(char const* fmt)
    {
        return fmt == nullptr;
    }

    /// Returns true if the given std::string is empty.
    inline bool IsFormatEmptyOrNull(std::string const& fmt)
    {
        return fmt.empty();
    }


#endif
