#define _XOPEN_SOURCE 700  // Включение расширенных возможностей POSIX для совместимости

#include <dirent.h>   // Библиотека для работы с директориями, включая структуру dirent
#include <getopt.h>   // Библиотека для обработки опций командной строки
#include <locale.h>   // Библиотека для работы с локалями, используется для установки локали сортировки
#include <stdio.h>    // Библиотека для ввода-вывода, например, для printf и fprintf
#include <stdlib.h>   // Библиотека для стандартных функций, таких как malloc, free и exit
#include <string.h>   // Библиотека для работы со строками, включая функции strcoll и strcmp
#include <sys/stat.h> // Библиотека для работы с информацией о файлах, включая struct stat
#include <unistd.h>   // Библиотека для работы с системными вызовами, такими как chdir и lstat
#include "dirwalk.h"  // Заголовочный файл для функций dirwalk и других

// Функция для проверки, нужно ли выводить файл/директорию/ссылку
int should_print(const struct stat *statbuf, int opt_l, int opt_d, int opt_f) {
    // Проверяем тип элемента и возвращаем 1, если нужно выводить
    if (S_ISLNK(statbuf->st_mode)) {
        return opt_l || (!opt_l && !opt_d && !opt_f); // Символическая ссылка
    } else if (S_ISDIR(statbuf->st_mode)) {
        return opt_d || (!opt_l && !opt_d && !opt_f); // Директория
    } else if (S_ISREG(statbuf->st_mode)) {
        return opt_f || (!opt_l && !opt_d && !opt_f); // Файл
    }
    return 0; // Неизвестный тип (не выводим)
}

// Функция для вывода информации о файле/директории/ссылке
void print_entry(const char *dir, const char *name) {
    printf("%s/%s\n", dir, name);  // Выводим полный путь к элементу
}

// Функция сравнения для сортировки (использует LC_COLLATE)
int compare_entries(const struct dirent **a, const struct dirent **b) {
    return strcoll((*a)->d_name, (*b)->d_name);  // Сравниваем имена записей для сортировки
}

// Рекурсивный обход директории
void dirwalk(const char *dir, int opt_l, int opt_d, int opt_f, int opt_s) {
    struct dirent **entries;  // Массив для хранения записей директории
    int n_entries;  // Количество записей
    struct stat statbuf;  // Структура для хранения информации о файле

    // Сканируем директорию с сортировкой, если указана опция -s
    if (opt_s) {
        n_entries = (scandir(dir, &entries, NULL, compare_entries));  // Сканируем с сортировкой
    } else {
        n_entries = (scandir(dir, &entries, NULL, NULL));  // Сканируем без сортировки
    }

    if (n_entries < 0) {
        fprintf(stderr, "dirwalk: cannot open directory %s\n", dir);  // Ошибка при открытии директории
        return;
    }

    chdir(dir);  // Переходим в текущую директорию
    for (int i = 0; i < n_entries; i++) {
        // Пропускаем "." и ".."
        if (strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0) {
            free(entries[i]);  // Освобождаем память
            continue;  // Переходим к следующему элементу
        }

        // Получаем информацию о файле/директории/ссылке
        if ((lstat(entries[i]->d_name, &statbuf)) == -1) {
            fprintf(stderr, "dirwalk: cannot stat %s\n", entries[i]->d_name);  // Ошибка получения статуса
            free(entries[i]);  // Освобождаем память
            continue;  // Переходим к следующему элементу
        }

        // Проверяем, нужно ли выводить текущий элемент
        if (should_print(&statbuf, opt_l, opt_d, opt_f)) {
            print_entry(dir, entries[i]->d_name);  // Выводим информацию о записи
        }

        if (S_ISDIR(statbuf.st_mode)) {
            dirwalk(entries[i]->d_name, opt_l, opt_d, opt_f, opt_s);
        }

        free(entries[i]);  // Освобождаем память для текущего элемента
    }
    free(entries);  // Освобождаем память для массива записей

    chdir("..");  // Возвращаемся в родительскую директорию
}
