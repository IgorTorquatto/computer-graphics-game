#pragma once

#pragma region System Includes
    #if defined(_WIN32) || defined(_WIN64)
        #include <io.h>
        #define access _access
        #define F_OK 0
    #else
        #include <unistd.h>
    #endif
#pragma endregion
#pragma region Local Includes
    #include "basics.h" // size_t, bool
#pragma endregion

/* Verifica se path é absoluto (Unix ou Windows) */
bool path_is_absolute(const char *p);

/**
 * @brief Junta base + sep + name; se name for absoluto copia name.
 * 
 * @param base_dir diretório base
 * @param sep separador (ex: '/' ou '\')
 * @param name caminho relativo
 * @param out buffer de saída
 * @param outsize capacidade do buffer
 * 
 * @note Se name for absoluto, copia name.
 * @note Garante que o buffer de saída não seja excedido.
 **/
void path_join(const char *base_dir, const char *sep,
        const char *name, char *out, size_t outsize);

/* Trim leading/trailing whitespace and surrounding quotes */
void trim_and_strip_quotes(char *s);

/**
 * @brief Retorna o diretório base do filename, sem trailing slash
 * @cond base_dir deve suportar pelo menos 1024 caracteres.
 *
 * @note Se filename for um caminho relativo,
 * base_dir fica vazio e indica o diretório atual.
 * 
 * @note Se path não contém separador, out fica vazio string ("").
 * 
 * @note Se filename for um caminho absoluto,
 * base_dir é o diretório pai do filename.
 * 
 * @param filename caminho do arquivo.
 * @param base_dir diretório base encontrado para filename.
 * @param n tamanho de base_dir - deve ser pelo menos 1024.
 * 
 * @example
 * - filename = "foo/bar", base_dir = "foo"
 * - filename = "foo\\bar", base_dir = "foo"
 * - filename = "bar", base_dir = "" (indica o diretório atual)
 */
void directory_base(const char *filename, char *base_dir, size_t n);


/**
 * @brief Tenta encontrar 'name' no diretório base e subindo até `max_levels` pais.
 *
 * @param start_dir diretório base para começar a busca
 * @param name nome do arquivo a ser procurado
 * @param out buffer de saída para o caminho do arquivo encontrado
 * @param outsize tamanho do buffer de saída
 * @param max_levels número máximo de diretórios a subir
 *
 * @return true se o arquivo foi encontrado (preenche out) ou false caso contrário
 */
bool find_file_in_ancestors(
        const char *start_dir, const char *name,
        char *out, size_t outsize, int max_levels);
