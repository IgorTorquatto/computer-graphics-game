#pragma region STL
    #include <string.h>
    #include <stdio.h>
    #include <ctype.h>
#pragma endregion
#pragma region Local Includes
    #include "basics.h"
#pragma endregion
#include "file_system.h"


bool path_is_absolute(const char *p)
{
    if (!p || !*p)
        return false;

#if defined(_WIN32) || defined(_WIN64)
    // Windows: // UNC \\server\path ou //server/path
    if ((p[0] == '\\' && p[1] == '\\') || (p[0] == '/' && p[1] == '/'))
        return true
    // Drive letter: C:\  ou C:/
    if (isalpha((unsigned char)p[0]) && p[1] == ':' && (p[2] == '\\' || p[2] == '/'))
        return true;
    return false;
#else
    // Unix: começa com '/'
    if (p[0] == '/')
        return true;

    // também considere "C:\..." vindo de MTL windows: trate como absoluto para não fazer join errado
    return (isalpha((unsigned char)p[0]) && p[1] == ':' );
#endif
}

void path_join(const char *base, const char *sep,
        const char *name, char *out, size_t outsize)
{
    if (!name || !*name) {
        if (out && outsize) out[0] = '\0';
            return;
    }
    if (path_is_absolute(name)) { // se name já for absoluto, apenas copie
        strncpy(out, name, outsize-1);
        out[outsize-1] = '\0';
        return;
    }
    if (!base || !*base) {
        // sem base, usar name diretamente
        strncpy(out, name, outsize-1);
        out[outsize-1] = '\0';
        return;
    }

    // concatenar com separador, cuidando se base_dir já termina com separador
    size_t bl = strlen(base);
    int base_has_sep = (bl > 0 && (base[bl-1] == '/' || base[bl-1] == '\\'));
    const char *s = (sep && sep[0]) ? sep : "/";
    if (base_has_sep) {
        snprintf(out, outsize, "%s%s", base, name);
    } else {
        /* use sep se foi passado (ex: "/" ou "\\") senão usa '/'. */
        const char *s = (sep && sep[0]) ? sep : "/";
        snprintf(out, outsize, "%s%s%s", base, s, name);
    }
    out[outsize-1] = '\0';
}

void trim_and_strip_quotes(char *s)
{
    // trim left
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p)+1);

    // trim right
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) s[--len] = '\0';

    // remove surrounding quotes if present
    if (len >= 2 && ((s[0] == '"' && s[len-1] == '"') || (s[0] == '\'' && s[len-1] == '\''))) {
        memmove(s, s+1, len-2);
        s[len-2] = '\0';
    }
}

void directory_base(const char *path, char *base_dir, size_t n)
{
    assert(n >= 1024);

    if (!path || !*path) {
        if (base_dir && n) base_dir[0] = '\0';
            return;
    }
    // procurar por '/' ou '\' (portável)
    const char *p1 = strrchr(path, '/');
    const char *p2 = strrchr(path, '\\');
    const char *last = (p1 > p2) ? p1 : p2;

    if (!last) {
        if (base_dir && n)
            // sem diretório: base_dir fica vazio -> indica diretório atual
            base_dir[0] = '\0';
        return;
    }
    size_t len = (size_t)(last - path);
    if (len >= n)
        len = n - 1;

    memcpy(base_dir, path, len);
    base_dir[len] = '\0';
}

bool find_file_in_ancestors(const char *start_dir, const char *name, char *out, size_t outsize, int max_levels)
{
    if (!name || !out)
        return false;
    char candidate[1024];

    // try start_dir/name
    path_join(start_dir, "/", name, candidate, sizeof(candidate));
    if (access(candidate, F_OK) == 0) {
        strncpy(out, candidate, outsize-1);
        out[outsize-1]=0;
        return true;
    }

    // climb up directories
    if (!start_dir || !*start_dir) return false;
    char up[1024];
    strncpy(up, start_dir, sizeof(up)-1); up[sizeof(up)-1]=0;

    for (int level = 0; level < max_levels; ++level) {
        // remove last segment
        char *last1 = strrchr(up, '/');
        char *last2 = strrchr(up, '\\');
        char *last = (last1 && last2) ? ((last1 > last2) ? last1 : last2) : (last1 ? last1 : last2);
        if (!last)
            break;

        *last = '\0';
        path_join(up, "/", name, candidate, sizeof(candidate));
        if (access(candidate, F_OK) == 0) {
            strncpy(out, candidate, outsize-1);
            out[outsize-1]=0;
            return true;
        }
    }
    return false;
}
