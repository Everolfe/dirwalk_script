#define _XOPEN_SOURCE 700
#include <dirent.h>
#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dirwalk.h"

// Функция для проверки, нужно ли выводить файл/директорию/ссылку
int should_print(const struct stat *statbuf, int opt_l, int opt_d, int opt_f) {
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
    printf("%s/%s\n", dir, name);
}

// Функция сравнения для сортировки (использует LC_COLLATE)
int compare_entries(const struct dirent **a, const struct dirent **b) {
    return strcoll((*a)->d_name, (*b)->d_name);
}

// Рекурсивный обход директории
void dirwalk(const char *dir, int opt_l, int opt_d, int opt_f, int opt_s) {
    struct dirent **entries;
    int n_entries;
    struct stat statbuf;

    // Сканируем директорию с сортировкой, если указана опция -s
    if (opt_s) {
        n_entries = (scandir(dir, &entries, NULL, compare_entries));
    } else {
        n_entries = (scandir(dir, &entries, NULL, NULL));
    }

    if (n_entries < 0) {
        fprintf(stderr, "dirwalk: cannot open directory %s\n", dir);
        return;
    }

    chdir(dir); // Переходим в текущую директорию
    for (int i = 0; i < n_entries; i++) {
        // Пропускаем "." и ".."
        if (strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0) {
            free(entries[i]);
            continue;
        }

        // Получаем информацию о файле/директории/ссылке
        if ((lstat(entries[i]->d_name, &statbuf)) == -1) {
            fprintf(stderr, "dirwalk: cannot stat %s\n", entries[i]->d_name);
            free(entries[i]);
            continue;
        }

        // Проверяем, нужно ли выводить текущий элемент
        if (should_print(&statbuf, opt_l, opt_d, opt_f)) {
            print_entry(dir, entries[i]->d_name);
        }

        //if (S_ISDIR(statbuf.st_mode)) {
        //    dirwalk(entries[i]->d_name, opt_l, opt_d, opt_f, opt_s);
        //}

        free(entries[i]);
    }
    free(entries);

    chdir(".."); // Возвращаемся в родительскую директорию
} 
