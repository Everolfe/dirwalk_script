#define _XOPEN_SOURCE 700  // Включение расширенных возможностей POSIX для совместимости
#include "dirwalk.h"  

void add_to_file_list(file_list_t *list, const char *path) {
    // Проверка, нужно ли увеличивать емкость списка
    if (list->count == list->capacity) {
        // Увеличиваем емкость вдвое или устанавливаем начальную емкость
        size_t new_capacity = (list->capacity == 0) ? 16 : list->capacity * 2;
        // Перераспределение памяти
        char **tmp = realloc(list->items, new_capacity * sizeof(char*));
        
        // Проверка на успешное перераспределение
        if (!tmp) { 
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        
        list->items = tmp; // Обновляем указатель на массив
        list->capacity = new_capacity; // Обновляем емкость
    }
    
    // Копируем строку пути и добавляем в список
    list->items[list->count] = strdup(path);
    
    // Проверка на успешное дублирование строки
    if (!list->items[list->count]) { 
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    
    list->count++; // Увеличиваем счетчик элементов
}

void free_file_list(file_list_t *list) {
    // Проверка, есть ли элементы для освобождения
    if (list->items) {
        for (size_t i = 0; i < list->count; i++) {
            free(list->items[i]);
        }
        free(list->items);
        list->items = NULL; // Обнуляем указатель
    }
    list->count = 0;
    list->capacity = 0;
}

int compare_file_names(const void *a, const void *b) {
    // Приведение типов для аргументов
    const char * const *pa = a;
    const char * const *pb = b;
    
    if (!pa || !pb || !*pa || !*pb) {
        return 0; // Если данные некорректны, считаем их равными
    }
    
    return strcoll(*pa, *pb); // Сравнение строк с учетом локали
}

int matches_filter(const struct stat *file_info, const filter_options_s *options) {
    // Если данные некорректны, считаем, что файл не подходит
    if (!file_info || !options) return 0; 

    // Проверяем, задан ли хотя бы один фильтр
    int any_filter = options->show_links || options->show_dirs || options->show_files;

    // Проверка на соответствие типу файла
    if (options->show_links && S_ISLNK(file_info->st_mode)) return 1; // Символическая ссылка
    if (options->show_dirs && S_ISDIR(file_info->st_mode)) return 1; // Директория
    if (options->show_files && S_ISREG(file_info->st_mode)) return 1; // Обычный файл

    // Если не указаны специфичные фильтры, разрешаем все типы
    if (!any_filter) return 1;

    return 0; // Файл не соответствует фильтрам
}

void traverse_directory(const char *base_path, const filter_options_s *options, file_list_t *file_list) {
    DIR *dir = opendir(base_path); // Открываем директорию
    
    // Проверка на успешное открытие
    if (!dir) {
        fprintf(stderr, "Ошибка при открытии каталога '%s': %s\n", base_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем "." и ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[PATH_MAX_LENGTH]; // Массив для хранения полного пути
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name); // Формируем полный путь

        struct stat file_info;
        // Используем lstat, чтобы не следовать символическим ссылкам
        if (lstat(path, &file_info) < 0) {
            fprintf(stderr, "Ошибка при lstat '%s': %s\n", path, strerror(errno));
            continue;
        }

        // Если объект удовлетворяет фильтру, сохраняем его
        if (matches_filter(&file_info, options)) {
            add_to_file_list(file_list, path);
        }

        // Если это каталог, за исключением символических ссылок, заходим рекурсивно
        if (S_ISDIR(file_info.st_mode)) {
            traverse_directory(path, options, file_list);
        }
    }
    
    closedir(dir); // Закрываем директорию
}
