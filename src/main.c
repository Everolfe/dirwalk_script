#define _XOPEN_SOURCE 700 // Включение расширенных возможностей POSIX для совместимости
#include "dirwalk.h"

/**
 * Главная функция программы.
 * 
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return 0 при успешном выполнении, иначе код ошибки.
 */
int main(int argc, char *argv[]) {
    // Установка локали для корректной сортировки
    setlocale(LC_COLLATE, "");

    // Инициализация структуры для хранения списка файлов
    file_list_t file_list = {0};
    file_list.items = NULL;
    file_list.count = 0;
    file_list.capacity = 0;

    // Инициализация структуры для хранения опций фильтрации и сортировки
    filter_options_s options = {0, 0, 0, 0};

    int opt; // Переменная для хранения текущей опции командной строки
    struct stat file_info; // Структура для хранения информации о файле
    opterr = 0; // Отключаем стандартное сообщение об ошибке getopt

    // Обработка опций командной строки
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l': options.show_links = 1; break; // Включить показ символических ссылок
            case 'd': options.show_dirs = 1; break;  // Включить показ директорий
            case 'f': options.show_files = 1; break; // Включить показ файлов
            case 's': options.sort = 1; break;       // Включить сортировку результатов
            default:
                // Вывод сообщения об ошибке, если опция неизвестна
                fprintf(stderr, "Usage: %s [options] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Определение начального каталога: если аргумент остался, используем его, иначе текущий каталог "."
    const char *start_dir = (optind < argc) ? argv[optind] : ".";

    // Проверка существования начального каталога
    if (lstat(start_dir, &file_info) < 0) {
        fprintf(stderr, "Ошибка при lstat '%s': %s\n", start_dir, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Если начальный каталог удовлетворяет фильтру, добавляем его в список
    if (matches_filter(&file_info, &options)) {
        add_to_file_list(&file_list, start_dir);
    }

    // Если начальный каталог является директорией, рекурсивно обходим её
    if (S_ISDIR(file_info.st_mode)) {
        traverse_directory(start_dir, &options, &file_list);
    }

    // Если включена сортировка, сортируем список файлов
    if (options.sort) {
        qsort(file_list.items, file_list.count, sizeof(char *), compare_file_names);
    }

    // Выводим список файлов
    for (size_t i = 0; i < file_list.count; i++) {
        printf("%s\n", file_list.items[i]);
    }

    // Освобождаем память, выделенную для списка файлов
    free_file_list(&file_list);

    return 0; // Успешное завершение программы
}
