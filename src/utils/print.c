#include "basics.h"
#include <stdio.h>
#include <stdarg.h>

#if defined(NDEBUG) // Not in debug mode.
    void print_warning(const char *msg, ...) {}
    void print_error(const char *msg, ...) {}
    void print_info(const char *msg, ...) {}
    void print_success(const char *msg, ...) {}
#elif defined(_WIN32) || defined(_WIN64) // Windows
    #include <windows.h>
	HANDLE _hConsole;
	WORD _saved_attributes;
	#define SET_CLI_RED() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY)
	#define SET_CLI_GREEN() SetConsoleTextAttribute(_hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_YELLOW() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_BLUE() SetConsoleTextAttribute(_hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY)
	#define RESET_CLI() SetConsoleTextAttribute(_hConsole, _saved_attributes)

    static void _init()
    {
        static bool initialized = false;
        if (initialized)
            return;
        // Get the console handle
        _hConsole = GetStdHandle(STD_ERROR_HANDLE);

        // Save the current text attributes
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(_hConsole, &consoleInfo);
        _saved_attributes = consoleInfo.wAttributes;
    }

    void print_warning(const char *msg, ...) {
        va_list args;
        _init();
        SET_CLI_YELLOW();
        fprintf(stderr, "[WARNING]: ");
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        RESET_CLI();
        fputc('\n', stderr);
    }

    void print_error(const char *msg, ...) {
        va_list args;
        _init();
        SET_CLI_RED();
        fprintf(stderr, "[ERROR]: ");
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        RESET_CLI();
        fputc('\n', stderr);
    }

    void print_info(const char *msg, ...) {
        va_list args;
        _init();
        fprintf(stderr, "[INFO]: ");
        SET_CLI_BLUE();
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        RESET_CLI();
        fputc('\n', stderr);
    }

    void print_success(const char *msg, ...) {
        va_list args;
        _init();
        fprintf(stderr, "[SUCCESS]: ");
        SET_CLI_GREEN();
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        RESET_CLI();
        fputc('\n', stderr);
    }

#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__) \
|| defined(__unix__) || defined(unix) || defined(__unix) // Unix
	#define RED "\033[31m"
	#define GREEN "\033[32m"
	#define YELLOW "\033[33m"
	#define BLUE "\033[34m"
	#define RESET "\033[m"

    void print_warning(const char *msg, ...) {
        va_list args;
        fprintf(stderr, YELLOW);
        fprintf(stderr, "[WARNING]: ");
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
        fprintf(stderr, RESET);
        fputc('\n', stderr);
    }

    void print_error(const char *msg, ...) {
        va_list args;
        fprintf(stderr, RED);
        fprintf(stderr, "[ERROR]: ");
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
        fprintf(stderr, RESET);
        fputc('\n', stderr);
    }

    void print_info(const char *msg, ...) {
        va_list args;
        fprintf(stderr, BLUE);
        fprintf(stderr, "[INFO]: ");
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
        fprintf(stderr, RESET);
        fputc('\n', stderr);
    }

    void print_success(const char *msg, ...) {
        va_list args;
        fprintf(stderr, GREEN);
        fprintf(stderr, "[SUCCESS]: ");
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
        fprintf(stderr, RESET);
        fputc('\n', stderr);
    }
#else
	static_assert(false, "Sistema operacional desconhecido");
#endif
