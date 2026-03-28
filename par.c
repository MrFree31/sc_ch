#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

typedef struct receipt{//Структура чека
    char shop[128];
    char date[24];
    double amount;
}receipt;

void magazin(char *in, char *out){//Выделение магазина
    int i = 0;
    while(in[i]&& in[i]!= '\n' && i<127){
        out[i] = in[i];
        i++;
    }
    out[i] = '\0';
}

int main(int argc, char *argv[]){
    SetConsoleOutputCP(CP_UTF8);
    //Файлы не загружены - прекращаем выполнение
    if(argc<2){
        printf("Ошибка! Файлы не загружены\n");
        return 1;
    }

    receipt check = {"","",0.0};
    FILE *f1 = fopen(argv[argc-1],"r");
    if(f1 == NULL){
        printf("ФАЙЛ ПУСТ ИЛИ ЕЩЁ ЧТО-ТО\n");
        return 1;
    }

    //чтение магазина
    char line_s[128];//temp строка магазин
        while(fgets(line_s,sizeof(line_s),f1)){//Читаем строку из файла
        if((strstr(line_s,"ООО"))||(strstr(line_s,"МЕСТО РАСЧЁТОВ"))||
        (strstr(line_s,"ИП"))||(strstr(line_s,"АО"))||(strstr(line_s,"МАГАЗИН"))){//Возвращает указатель, а не NULL - хорошо. 
            magazin(line_s,check.shop);//strstr - поиск "Подстроки" и возврат указателя
            break;
        }//Если в чеках нет названий, есть только ип, ооо и тд. Их и берём.
    }
    rewind(f1);//Перечитываем файл по новой.(Указатель убежал при предыдущих проверке)
    
    //чтение даты
    //fgets(check.date,sizeof(check.date),f1);
    char *p;
    char line[256];//temp строка даты и суммы
    int date_flag = 0,total_flag = 0;//Большая девятка
    while(fgets(line,sizeof(line),f1)){
        if(date_flag == 0){
            p = line;
            while(*p){
                if(isdigit(p[0]) && isdigit(p[1]) && p[2] == '.' &&
                isdigit(p[3]) && isdigit(p[4]) && p[5] == '.' &&
                isdigit(p[6]) && isdigit(p[7]) && isdigit(p[8]) && isdigit(p[9])){
                    if((p == line || !isdigit(*(p-1))) && (!isdigit(p[10]) || p[10]=='\0' || p[10]=='\n')){//проверка, что указатель строке, что до строки нет чисел и после найденных чисел нет.
                        strncpy(check.date,p,10);
                        date_flag = 1;
                        break;
                    }
                }
                p++;
            }
        }
        //Проверяем числа и пробел в строке с Total
        if(!total_flag && (strstr(line,"ИТОГО")||strstr(line,"ИТОГ")||strstr(line,"Итого")||strstr(line,"Итог"))){
            char *summ = strchr(line,'=');//Находим указатель на =
            if(summ){
                summ++;//Пропускаем =
                while(*summ == ' '){
                    summ++; //пропуск пробелов
                }
            }   
            else{    
                summ = line;
                while(*summ && !isdigit(*summ)) summ++;//Не нашли - идём просто до числа
            }   
            if(*summ){
                check.amount = atof(summ);//Переводим строку в число
                total_flag = 1;
            }
        }
    }
    
    fclose(f1);
    printf("{\"date\":\"%s\",\"shop\":\"%s\",\"amount\":%.2lf}\n",check.date,check.shop,check.amount);
    return 0;
}