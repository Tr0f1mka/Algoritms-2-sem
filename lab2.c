#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_STR_LEN 100
#define MAX_SIZE 14


/*------Структуры------*/

typedef struct Elem {
    char key[MAX_STR_LEN];
    char data[MAX_STR_LEN];
} Elem;


typedef struct Table {
    Elem* elems;
    int size;
    int max_size;
} Table;


/*-------Функции-------*/

Table* Init(void);
void DeInit(Table* table);
int AddElem(Table* table, char* key, char* data);
void PrintTable(Table* table);
int FindIndElem(Table* table, char* key, int cur_size);
int BinaryInsertionSorting(Table* table);
Table* ReadFile(char* file);
void TestSort(char* file);


Table* Init(void) {
    /*
    Инициализация таблицы
    Вход: ничего
    Возврат: указатель на созданную таблицу или NULL при ошибке
    */

    Table* table = (Table*)malloc(sizeof(Table));
    if (table == NULL) {
        return NULL;
    }

    table->max_size = MAX_SIZE;
    table->size = 0;
    table->elems = (Elem*)malloc(sizeof(Elem) * table->max_size);
    if (table->elems == NULL) {
        free(table);
        return NULL;
    }
    return table;
}


void DeInit(Table* table) {
    /*
    Деинициализация таблицы
    */

    if (table == NULL) {
        return;
    }
    if (table->elems != NULL) {
        free(table->elems);
        table->elems = NULL;
    }
    free(table);
}


int AddElem(Table* table, char* key, char* data) {
    /*
    Добавляет элемент в таблицу
    Вход: таблица, ключ, значение
    Возврат: 0 - успех, 1 - ошибка
    */

    if (table == NULL || key == NULL || data == NULL) {
        return 1;
    }

    if (table->size == table->max_size) {
        table->max_size <<= 1;
        Elem* t = (Elem*)realloc(table->elems, sizeof(Elem) * table->max_size);
        if (t == NULL) {
            return 1;
        }
        table->elems = t;
    }

    strncpy(table->elems[table->size].key, key, MAX_STR_LEN);
    strncpy(table->elems[table->size].data, data, MAX_STR_LEN);
    table->size++;

    return 0;
}


void PrintTable(Table* table) {
    /*
    Выводит таблицу
    Вход: указатель на таблицу
    Возврат: ничего
    */

    if (table == NULL) {
        return;
    }
    printf("\n");
    if (table->size == 0) {
        printf("TABLE IS EMPTY\n\n");
        return;
    }

    printf("NUM   KEY            DATA\n");
    for (int i = 0; i < table->size; i++) {
        printf("%-3d   %-13s  %-s\n", i + 1, table->elems[i].key, table->elems[i].data);
    }
    printf("\n");
}


int FindIndElem(Table* table, char* key, int cur_size) {
    /*
    Находит индекс элемента в отсортированном массиве
    Вход: указатель на таблицу, указатель на ключ, размер отсортированной части
    Возврат: индекс элемента или -1 при ошибке
    */

    if (table == NULL || key == NULL) {
        return -1;
    }

    int left = 0,
        right = cur_size - 1,
        mid,
        cmp;

    while (left <= right) {
        mid = left + ((right - left) >> 1);
        cmp = strcmp(table->elems[mid].key, key);

        if (cmp < 0) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return left;
}


int BinaryInsertionSorting(Table* table) {
    /*
    Сортирует массив методом двоичной вставки
    Вход: таблица
    Возврат: 0 - успех, 1 - ошибка
    */

    if (table == NULL) {
        return 1;
    }

    for (int i = 1; i < table->size; i++) {
        int ind = FindIndElem(table, table->elems[i].key, i);
        if (ind == -1) {
            return 1;
        }
        if (ind == i) {
            continue;
        }
        Elem moving = table->elems[i];
        for (int j = i; j > ind; j--) {
            table->elems[j] = table->elems[j - 1];
        }
        table->elems[ind] = moving;
    }

    return 0;
}


Table* ReadFile(char* file) {
    /*
    Считывает таблицу из файла
    Вход: имя файла
    Возврат: указатель на таблицу
    */

    if (file == NULL) {
        return NULL;
    }

    FILE* f = fopen(file, "r");
    if (f == NULL) {
        return NULL;
    }
    Table* table = Init();
    if (table == NULL) {
        fclose(f);
        return NULL;
    }

    int i;
    char key[MAX_STR_LEN],
        data[MAX_STR_LEN];
    if (fscanf(f, "%d", &i) != 1) {
        fclose(f);
        DeInit(table);
        return NULL;
    }

    for (int j = 0; j < i; j++) {
        if ((fscanf(f, "%s %s", key, data)) != 2) {
            fclose(f);
            DeInit(table);
            return NULL;
        }
        if (AddElem(table, key, data)) {
            fclose(f);
            DeInit(table);
            return NULL;
        }
    }

    return table;
}


void TestSort(char* file) {
    /*
    Функция для тестирования
    Вход: имя файла с тестом
    Возврат: ничего
    */

    if (file == NULL) {
        return;
    }

    printf("\nTEST FILE: %s\n", file);

    Table* table = ReadFile(file);
    if (table == NULL) {
        return;
    }
    PrintTable(table);
    if (BinaryInsertionSorting(table)) {
        DeInit(table);
        return;
    }
    PrintTable(table);
}


int BinarySearch(Table* table, char* key) {
    /*
    Бинарный поиск элемента по ключу
    Вход: таблица, ключ
    Возврат: индекс элемента или -1 если не найден
    */

    if (table == NULL || key == NULL || table->size == 0) {
        return -1;
    }

    int left = 0;
    int right = table->size - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(table->elems[mid].key, key);

        if (cmp == 0) {
            return mid;  // нашли элемент
        }
        else if (cmp < 0) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}

void PrintData(Table* table, char* key) {
    /*
    Выводит данные по ключу
    Вход: таблица, ключ
    Возврат: ничего
    */

    if (table == NULL || key == NULL) {
        printf("ERROR\n");
        return;
    }

    int ind = BinarySearch(table, key);
    if (ind == -1) {
        printf("'%s' not found\n", key);
        return;
    }

    printf("DATA: %s\n", table->elems[ind].data);
}


void menu() {
    Table* table = ReadFile("test_random.txt");
    BinaryInsertionSorting(table);
    char input[MAX_STR_LEN];
    printf("INPUT KEY OR \"exit\":  ");
    scanf("%s", input);
    while (strcmp(input, "exit") != 0) {
        PrintData(table, input);
        printf("INPUT KEY OR \"exit\":  ");
        scanf("%s", input);
    }

    DeInit(table);
}



int main(void) {
    TestSort("test_sorted.txt");
    TestSort("test_reverse.txt");
    TestSort("test_random.txt");
    menu();

    return 0;
}