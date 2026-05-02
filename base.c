#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "c:\Users\koner\Documents\GitHub\sc_ch\sqlite3.h"
//библиотека SQLite, типо база с чеками или типо того
// Отдельно скачана и распакована в нашу папку с компилируемым файлом base.c, типо datebase
void read_json(char *json, char *name, char *output, int size){// Нам нужно обратно прочитать json. По именам переменных пройтись и вывести в output строку размером size
    char *p = strstr(json, name);// Указатель на имя переменной
    if(p == NULL){//Проверка наличия переменной, то есть указатель "pointer - p" будет пуст "NULL"
        output[0] = '\0';
        return;
    }
    p += strlen(name) + 3;//Пропускаем длину имени переменной, а потом ":" между и именем и значением, как тут, но тут ошибка есть с кавычками в названии ООО {"date":"23.03.2026","shop":"ООО "Продукты у дома"","amount":426.37}
    int i = 0; // 
    while (*p && i < size - 1){
        if(*p == '\\'){//Пропуск "\" в json входном перед кавычкой и прочим
            p++;
            if(*p){//Далее записываем символ после "\"
                output[i++] = *p++;
            }
        }
        else if(*p == '"'){//Без "\" кавычка сама по себе означает конец строки str, которую мы меняем на "\0"
            break;
        }
        else{
            output[i++] = *p++;//В аином сслучае просто записываем символ.
        }
    }
    output[i] = '\0';
}

int main(int argc, char *argv[]){
    SetConsoleOutputCP(CP_UTF8);
//    if(argc < 2){// проверка передачи значений в наш файл Base.c
//        printf("JSON не передан");
//        return 1;
//    }
//    char *input = argv[1];// Из питона берутся аргументы, мы из в переменную кладём.

//Через тот самый stdin берём, вместо аргумента
    char input[512];
    if(!fgets(input, sizeof(input), stdin)){
        printf("Ошибка! Вход пустой\n");
        return 1;
    }
    input[strcspn(input, "\r\n")] = '\0'; //Удаление переноса строки 

    char date[12], shop[128];
    double amount = 0.0;

    read_json(input, "date", date, sizeof(date));
    read_json(input, "shop", shop, sizeof(shop));
    
    char *p = strstr(input, "\"amount\":");//Внимание на кавычки
    if(p){
        p += 9; // Перепрыгиваем (двигаем указатель) как раз нашу подстроку "amount": - 9 символов
        amount = atof(p);
    }
    
    //vvvv Работа с SQLite vvvv
    sqlite3 *table = NULL; //Структура таблицы
    char *errror = NULL; //Вывод ошибок. Нужен для запросов
    int compl; // Возвращаемое значение, типо return. 0 - хорошо, остальное - ошибки. Обозначим, как compl(ete), типо завершилось или нет
    sqlite3_stmt *call = NULL; //Структура запроса на обработку таблицы. stmt - высказывание ~ запрос

    compl = sqlite3_open("receipts.db", &table); //Создание файла таблицы. db - расширение datebase
    if(compl != 0){ //Проверка выполнения
        printf("Ошибка! Не удалось создать файл под таблицу (%s)\n",sqlite3_errmsg(table)); //Уведомление об ошибке и код ошибки в скобках. sqlite3_errmsg возвращает код ошибки, который сохраняется в структуру каждый раз, когда и происходит ошибка.
        return 1;
    }

    char *create_sql = "CREATE TABLE IF NOT EXISTS receipts ("//Запрос, на создание, проверку существования. Задание имени, значений в строках 
    "id INTEGER PRIMARY KEY AUTOINCREMENT,""date TEXT NOT NULL,"
    "shop TEXT NOT NULL,""amount REAL NOT NULL)"; 
    //not null - запреж на создание пустых строк, real - аналог double? Primary key - создание столбца уникальных значений для сортировки.
    //autoincerement - каждый раз прибавляет значение для новой строки, 
    compl = sqlite3_exec(table, create_sql, NULL, NULL, &errror); //Создаём таблицу со строками, как в вышесоставленном запросе.
    if(compl != 0){ //Проверка выполнения
        printf("Ошибка! Не удалось создать таблицу (%s)\n", errror);
        sqlite3_free(errror);
        sqlite3_close(table);
        return 1;
    }

    //Таблица за месяц
    char *create_month_sql = "CREATE TABLE IF NOT EXISTS months(month TEXT PRIMARY KEY, total REAL NOT NULL)";
    compl = sqlite3_exec(table, create_month_sql, NULL, NULL, &errror);
    if(compl != SQLITE_OK){
        printf("Ошибка! Months не удалось создать %s\n", errror);
        sqlite3_free(errror);
        sqlite3_close(table);
        return 1;
    }

    // СОЗДАНИЕ(много шагов) запросика на создание строчечки.
    char *add_values = "INSERT INTO receipts (date, shop, amount) VALUES (?,?,?);"; //вставляем значения в таблицу "receipts" по адресам "d,s,a" с помощью VALUES и ячеек (?). Это делается для того, чтобы значения воспринимались как "строки", ни в коем случае не команды.
    compl = sqlite3_prepare_v2(table, add_values, -1, &call, NULL);// Создание запроса на вставку. Обращение к структуре table, запрос на вставку, -1 - чтение до конца строки '\0', записываем в call запрос, пятый аргумент не используется - NULL
    if(compl != 0){
        printf("Ошибка! Не удалось задать запрос (%s)", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }

    //Добавление значений в пременные в VALUES[1], на место (?).
    compl = sqlite3_bind_text(call, 1, date, -1, SQLITE_TRANSIENT); // Четвёртый аргумент - указывает, как работать с данными строки из второго аргумента. можно static, можно transient, первый берёт напрямую из переменной, второй делает копию.
    if(compl !=0){
        printf("Ошибка! Не удалось вставить значение date в запрос (%s)\n", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }

    //Тоже самое для shop, индекс в VALUES - 2
    compl = sqlite3_bind_text(call, 2, shop, -1, SQLITE_TRANSIENT);
    if(compl !=0){
        printf("Ошибка! Не удалось вставить значение shop в запрос (%s)\n", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }

    //Добавляем amount, так как это double, никаких объёмов для чтения, как со стрингом(text), transient не нужны.
    //Индекс в VALUES - 3
    compl = sqlite3_bind_double(call, 3, amount);
    if(compl !=0){
        printf("Ошибка! Не удалось вставить значение amount в запрос (%s)\n", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }

    compl = sqlite3_step(call);
    if(compl !=101){ // 101 = sqlite_done - верное исполнение запроса
        printf("Ошибка! Не удалось исполнить запрос вставки (%s)\n", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }
    else{
        printf("Значения занесены в таблицу\n");
    }

    sqlite3_finalize(call); //Завершение - finalize запроса(стерание?)

    //Вывод суммы за месяц последнего обработанного чека
    char mm_yyyy[8]; //Дата месяц и год (mm.yyyy\0). 8 значений
    strncpy(mm_yyyy, date+3, 7); //Сдвигаем указатель и берём 7 символов месяца и года
    mm_yyyy[7] = '\0';

/*    char date_form[12];
    sprintf(date_form, "%%.%s", mm_yyyy); // "%%" - выдаст "%" без интерпретации как служебный символ. sprintf \0 автоматом добавляет
    //Оброз строки. На месте % sqlite любое значение берёт, как в Экселе.*/

    //Обновим таблицу с месяцем каждый новый раз
    char update_month_sql[512];
    //Конструируем запрос, через sprintf - в ставляем в него значения mm_yyyy. Если такая позиция сеществует (conflict), то обновим и вставим excluded.total 
    snprintf(update_month_sql, sizeof(update_month_sql), "INSERT INTO months(month, total)"
    "VALUES('%s', (SELECT SUM(amount) FROM receipts WHERE date LIKE '%%.%s')) ON CONFLICT(month) DO UPDATE SET total = excluded.total;", mm_yyyy, mm_yyyy);

    compl = sqlite3_exec(table, update_month_sql, NULL, NULL, &errror);
    if(compl != SQLITE_OK){
        printf("Ошибка! Обновление сумммы за месяц(%s)\n", errror);
        sqlite3_free(errror);
    }
    else{
        printf("Итог за месяц обновлён. Месяц - %s\n", mm_yyyy);
    }


    //Сумма
    char sum_table[128];//Запрос суммы
    //Ограничим объём - sizeof(sum_table)
    snprintf(sum_table, sizeof(sum_table), "SELECT SUM(amount) FROM receipts WHERE date LIKE '%%.%s'", mm_yyyy); //Запрос суммы всех ячеек amount в строках с датой, как у нашего чека.
    
    sqlite3_stmt *sum_call;
    compl = sqlite3_prepare_v2(table, sum_table, -1, &sum_call, NULL);
    if(compl == 0){
    //    printf("Ошибка! Не удалось сформировать запрос суммирования (%s)\n", sqlite3_errmsg(table));
    //    sqlite3_close(table);
    //    return 1;
        compl = sqlite3_step(sum_call);//Ронять всё в ответ на SQLITE_DONE и ошибки - слишком круто. Переделываем
        if(compl == SQLITE_ROW){ // SQLITE_ROW Чтение строки первой, в данном случае со значением суммы
            double month_sum = sqlite3_column_double(sum_call, 0);
            printf("Семма за %s: %.2lf\n", mm_yyyy, month_sum);
        }
        else{
            printf("Сумма за %s не получена\n", mm_yyyy);
        }
        sqlite3_finalize(sum_call);
    }
    else{
        printf("Ошибка! Запрос суммы (%s)\n", sqlite3_errmsg(table));
    }
    sqlite3_close(table);
    return 0;
            //printf("Ошибка! Не удалось исполнить запрос суммы (%s)\n", sqlite3_errmsg(table));
            //sqlite3_close(table);
            //return 1;
            
    /*compl = sqlite3_step(sum_call);
    if(compl != SQLITE_ROW && compl != SQLITE_DONE){ // SQLITE_ROW Чтение строки первой, в данном случае со значением суммы
        printf("Ошибка! Не удалось исполнить запрос суммы (%s)\n", sqlite3_errmsg(table));
        sqlite3_close(table);
        return 1;
    }

    if(compl == SQLITE_ROW){
        double month = sqlite3_column_double(sum_call, 0); // sqlite3_column_double вернёт значение с первым "0" и единственным значение из запроса sum_call в формате double
        printf("Сумма за месяц %s: %.2lf\n", mm_yyyy, month);
    }
    else{//Строки(row) нет, сразу done - суммы не будет
        printf("Нет значений суммы за данный месяц (%s)", mm_yyyy);
    }
    sqlite3_finalize(sum_call);
    sqlite3_close(table); //Закрытие таблицы.

    return 0;*/
}