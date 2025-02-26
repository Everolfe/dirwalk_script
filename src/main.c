#define _XOPEN_SOURCE 700

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<locale.h>
#include "dirwalk.h"

int main(int argc, char *argv[]) {
    int opt;
    int opt_l = 0, opt_d = 0, opt_f = 0, opt_s = 0;
    char *dir = ".";

    // Обработка опций командной строки
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l':
                opt_l = 1;
                break;
            case 'd':
                opt_d = 1;
                break;
            case 'f':
                opt_f = 1;
                break;
            case 's':
                opt_s = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [dir] [-l] [-d] [-f] [-s]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Если указан каталог, используем его
    if (optind < argc) {
        dir = argv[optind];
    }

    // Устанавливаем локаль для сортировки (если указана опция -s)
    if (opt_s) {
        setlocale(LC_COLLATE, "");
    }

    // Запуск обхода директории
    dirwalk(dir, opt_l, opt_d, opt_f, opt_s);

    return 0;
}
