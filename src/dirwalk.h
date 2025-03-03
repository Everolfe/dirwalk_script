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
#define PATH_MAX 4096

// Структура для хранения списка путей
typedef struct {
    char **items;    // Массив строк (путей к файлам/директориям)
    size_t count;    // Текущее количество элементов
    size_t capacity; // Текущая емкость массива
} FileList;

// Структура для хранения опций фильтрации и сортировки
typedef struct {
    int show_links;  // Показывать символические ссылки
    int show_dirs;   // Показывать директории
    int show_files;  // Показывать файлы
    int sort;        // Сортировать результаты
} FilterOptions;

// Функции для работы со списком путей
void add_to_file_list(FileList *list, const char *path);
void free_file_list(FileList *list);

// Функция сравнения для сортировки
int compare_file_names(const void *a, const void *b);

// Функция для проверки, соответствует ли файл фильтру
int matches_filter(const struct stat *file_info, const FilterOptions *options);

// Рекурсивный обход директории
void traverse_directory(const char *base_path, const FilterOptions *options, FileList *file_list);


#endif 
