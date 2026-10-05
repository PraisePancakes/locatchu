#ifndef LOCATCHU_CONFIG_MACRO_H
#define LOCATCHU_CONFIG_MACRO_H

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define LOCATCHU_STR(arg) #arg
#define LOCATCHU_XSTR(arg) LOCATCHU_STR(arg)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#ifndef LOCATCHU_NDEBUG
#define LOCATCHU_ASSERT(condition, message)                                    \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define LOCATCHU_ASSERT(condition, message) \
    do {                                    \
    } while (false)
#endif