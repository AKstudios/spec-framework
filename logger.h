//==============================================================================
// logger.h - Defines a convenient logger for apps to log to a file
//==============================================================================
#pragma once

#include <syslog.h>

class CLogger
{
public:

    // Call this at startup to initialize logger settings
    void init(const char* app_name, const char* filename);

    // Save a message to log file with a log level
    void log(int log_level, const char* message);

protected:

    // Variables to store app and logfile name
    const char* m_app_name;
    const char* m_filename;

    // Flag to track whether init has already been called
    bool m_is_initialized, m_is_syslog;

    // Define a struct for text color and formatting
    struct color_t
    {
        const char* reset   = "\033[0m";
        const char* black   = "\033[30m";
        const char* red     = "\033[31m";
        const char* green   = "\033[32m";
        const char* yellow  = "\033[33m";
        const char* blue    = "\033[34m";
        const char* magenta = "\033[35m";
        const char* cyan    = "\033[36m";
        const char* white   = "\033[37m";

        const char* bold_black   = "\033[1;30m";
        const char* bold_red     = "\033[1;31m";
        const char* bold_green   = "\033[1;32m";
        const char* bold_yellow  = "\033[1;33m";
        const char* bold_blue    = "\033[1;34m";
        const char* bold_magenta = "\033[1;35m";
        const char* bold_cyan    = "\033[1;36m";
        const char* bold_white   = "\033[1;37m";

        const char* bold      = "\033[1m";
        const char* dim       = "\033[2m";
        const char* underline = "\033[4m";
        const char* blink     = "\033[5m";
        const char* reverse   = "\033[7m";
        const char* hidden    = "\033[8m";
    } color;
};
//==============================================================================