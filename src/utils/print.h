#pragma once

void print_warning(const char *msg, ...);
void print_error(const char *msg, ...);
void print_success(const char *msg, ...);
void print_info(const char *msg, ...);


#if defined(__GNUC__) || defined(__clang__)
    /** Macro to call a function and print its location
     * __COLUMN__ is non-standard but widely supported by GCC/Clang
     * You might need to check your specific compiler's documentation
     * If __COLUMN__ is not available, you can omit this part
     * For this example, we assume it's available or define it to a default
     * Note: For maximum portability, only rely on __LINE__
     */
    #define trace_print_warning(message, ...) \
        print_warning(#message "at %s [line: %d, column: %d]", __FILE__, __LINE__, __COLUMN__ ## __VA_ARGS__)
    #define trace_print_error(message, ...) \
        print_error(#message "at %s [line: %d, column: %d]", __FILE__, __LINE__, __COLUMN__ ## __VA_ARGS__)
    #define trace_print_success(message, ...) \
        print_success(#message "at %s [line: %d, column: %d]", __FILE__, __LINE__, __COLUMN__ ## __VA_ARGS__)
    #define trace_print_info(message, ...) \
        print_info(#message "at %s [line: %d, column: %d]", __FILE__, __LINE__, __COLUMN__ ## __VA_ARGS__)
#else // Fallback if __COLUMN__ is not available
    #define trace_print_warning(message, ...) \
        print_warning(#message ## __VA_ARGS__)
    #define trace_print_error(message, ...) \
        print_error(#message ## __VA_ARGS__)
    #define trace_print_success(message, ...) \
        print_success(#message ## __VA_ARGS__)
    #define trace_print_info(message, ...) \
        print_info(#message ## __VA_ARGS__)
#endif
