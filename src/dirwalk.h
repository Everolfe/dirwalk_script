#ifndef DIRWALK_H
#define DIRWALK_H


#include <sys/types.h> // Библиотека для определения различных типов данных
#include <sys/stat.h> // Библиотека для работы с информацией о файлах


// Функция для определения, нужно ли печатать запись в зависимости от параметров
int should_print(const struct stat *statbuf, int opt_l, int opt_d, int opt_f);

// Функция для вывода информации о записи (файле или директории)
void print_entry(const char *dir, const char *name);

// Основная функция для обхода директории
void dirwalk(const char *dir, int opt_l, int opt_d, int opt_f, int opt_s);

#endif 
