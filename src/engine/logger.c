#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

bool logger_init() {
    return true;
}

bool logger_shutdown() {
    return true;
}

void logger_output(log_level level, const char * message, ...) {
    const char * log_level_strings[6] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };

    char logger_formatted_message[32000];
    memset(logger_formatted_message, 0, sizeof(logger_formatted_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(logger_formatted_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char logger_output_message[32000];
    sprintf(logger_output_message, "[%s] %s\n", log_level_strings[level], logger_formatted_message);
    printf("%s", logger_output_message);
}

