#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "c:\Users\koner\Documents\GitHub\sc_ch\sqlite3.h"

int main(){
    SetConsoleOutputCP(CP_UTF8);
    
    FILE *rep = fopen("Repchik.txt", "w");
    if(rep == NULL){
        printf("Проблема! Файл не создан!\n");
        return 1;
    }
    int compl;
    sqlite3 *table;
    compl = sqlite3_open("receipts.db", &table);
    if(compl != SQLITE_OK){
        printf("Ошибка! Таблица не открыта\n");
        fclose(rep);
        return 1;
    }
    fprintf(rep,"Итоги\n");

    sqlite3_stmt *stmt;
    char *result_sql = "SELECT date, shop , amount FROM receipts ORDER BY date";
    compl = sqlite3_prepare_v2(table, result_sql, -1, &stmt, NULL);
    if(compl == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW){
            fprintf(rep, "%s | %s | %.2f денег\n",sqlite3_column_text(stmt, 0),sqlite3_column_text(stmt, 1),sqlite3_column_double(stmt, 2));
        }
        sqlite3_finalize(stmt);
    }
    fprintf(rep, "\n");
    fprintf(rep, "Месяцы\n");
    char *monthly_sql = "SELECT month, total FROM months ORDER BY month";
    compl = sqlite3_prepare_v2(table, monthly_sql, -1, &stmt, NULL);
    if(compl == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW){
            fprintf(rep, "За %s: %.2f денег\n", sqlite3_column_text(stmt, 0), sqlite3_column_double(stmt, 1));
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(table);
    fclose(rep);
    return 0;
}