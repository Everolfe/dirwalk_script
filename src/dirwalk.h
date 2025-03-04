#ifndef DIRWALK_H
#define DIRWALK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <locale.h>
#include <errno.h>
#include <unistd.h>

// Максимальная длина пути
#define PATH_MAX_LENGTH 4096

// Структура для хранения списка путей
typedef struct {
    char **items;    // Массив строк (путей к файлам/директориям)
    size_t count;    // Текущее количество элементов
    size_t capacity; // Текущая емкость массива
} file_list_t; // Именование с суффиксом _t

// Структура для хранения опций фильтрации и сортировки
typedef struct {
    int show_links;  // Показывать символические ссылки
    int show_dirs;   // Показывать директории
    int show_files;  // Показывать файлы
    int sort;        // Сортировать результаты
} filter_options_s; // Именование с суффиксом _s

/**
 * Добавляет путь в список файлов.
 * 
 * @param list Указатель на список файлов.
 * @param path Путь к файлу, который нужно добавить.
 */
void add_to_file_list(file_list_t *list, const char *path);

/**
 * Освобождает память, занятую списком файлов.
 * 
 * @param list Указатель на список файлов, который нужно освободить.
 */
void free_file_list(file_list_t *list);

/**
 * Функция сравнения для сортировки.
 * 
 * @param a Указатель на первый элемент для сравнения.
 * @param b Указатель на второй элемент для сравнения.
 * @return Результат сравнения двух элементов.
 */
int compare_file_names(const void *a, const void *b);

/**
 * Проверяет, соответствует ли файл заданным фильтрам.
 * 
 * @param file_info Указатель на информацию о файле.
 * @param options Указатель на опции фильтрации.
 * @return 1, если файл соответствует фильтру; 0 в противном случае.
 */
int matches_filter(const struct stat *file_info, const filter_options_s *options);

/**
 * Рекурсивно обходит директорию.
 * 
 * @param base_path Путь к базовой директории.
 * @param options Указатель на опции фильтрации.
 * @param file_list Указатель на список файлов, в который будут добавлены найденные файлы.
 */
void traverse_directory(const char *base_path, const filter_options_s *options, file_list_t *file_list);

#endif // DIRWALK_H
