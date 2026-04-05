#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>//библиотека SQLite, типо база с чеками или типо того.
                    // Отдельно скачана и распакована в нашу папку с компилируемым файлом base.c, типо datebase
void read_json(char *json, char *name, char *output, int size){// Нам нужно обратно прочитать json. По именам переменных пройтись и вывести в output строку размером size
    char *p = strstr(json, name);// Указатель на имя переменной
    if(p == NULL){//Проверка наличия переменной, то есть указатель "pointer - p" будет пуст "NULL"
        output[0] = '\0';
        return;
    }
    p += strlen(name) + 3;//Пропускаем длину имени переменной, а потом ":" между и именем и значением, как тут, но тут ошибка есть с кавычками в названии ООО {"date":"23.03.2026","shop":"ООО "Продукты у дома"","amount":426.37}
    int i = 0; // 
    while (*p && *p != '"' && i < size - 1){
        output[i] = *p++;
        i++;
    }
    output[i] = '\0';
}

int main(int argc, char *argv[]){
    if(argc < 2){// проверка передачи значений в наш файл Base.c
        printf("JSON не передан");
        return 1;
    }
    char *input = argv[1];// Из питона берутся аргументы, мы из в переменную кладём.
    char date[12], shop[128];
    double amount = 0.0;

    read_json(input, "date", date, sizeof(date));
    read_json(input, "shop", shop, sizeof(shop));
    
    char *p = strstr(input, "\"amount\":");//Внимание на кавычки
    if(p){
        p += 9; // Перепрыгиваем (двигаем указатель) как раз нашу подстроку "amount": - 9 символов
        amount = atof(p);
    }
    return 0;
}