#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

typedef struct receipt{//Структура чека
    char shop[128];
    char date[24];
    double amount;
}receipt;

int main(int argc, char *argv[]){
    SetConsoleOutputCP(CP_UTF8);
    //Файлы не загружены - прекращаем выполнение
    if(argc<2){
        printf("Ошибка! Файлы не загружены\n");
        return 1;
    }

    receipt check;
    FILE *f1 = fopen(argv[argc-1],"r");
    //чтение магазина
    fgets(check.shop,sizeof(check.shop),f1);
    //чтение даты
    fgets(check.date,sizeof(check.date),f1);

    char line[24];//temp строка
    //чтение суммы
    fgets(line,sizeof(line),f1);
    line[strcspn(line,"\n")]='\0';
    check.date[strcspn(check.date,"\n")]='\0';
    check.shop[strcspn(check.shop,"\n")]='\0';


    //Проверяем числа и пробел в строке с Total
    char *p = strchr(line,'=');//Находим указатель на =
    if(!p){
        p = line;
        while(*p && !isdigit(*p)) p++;//Не нашли - идём просто до числа
    }
    else{
        p++;//нашли =, значит передвинем указатель с =, потому что atof() числа преобразует, а не чары.
    }

    if(*p){
        check.amount = atof(p);//Переводим строку в число
    }
    else{
        check.amount = 0;//Нет чисел - ноль
    }
    
    fclose(f1);
    printf("{\"date\":\"%s\",\"shop\":\"%s\",\"amount\":%.2lf}\n",check.date,check.shop,check.amount);
    return 0;
}