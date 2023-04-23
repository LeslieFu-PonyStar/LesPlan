#include<iostream>
#include<fstream>
#include<ctime>
#include<cstring>
#include "plan.h"
#define DB "./lestodo.list"
using namespace std;
void get_current_day(DayTime* local_day){
    time_t current_time = time(0);
    tm* local_tm = localtime(&current_time);
    local_day->year = local_tm->tm_year + 1900;
    local_day->month = local_tm->tm_mon + 1;
    local_day->day = local_tm->tm_mday;
}
void create_day(int year, int month, int day, DayTime* init_day){
    init_day->year = year;
    init_day->month = month;
    init_day->day = day;
}
void daytime_to_tm(DayTime* day, tm* time){
    time->tm_year = day->year - 1900;
    time->tm_mon = day->month - 1;
    time->tm_mday = day->day;
}
void print_day(DayTime* day){
    cout<< day->year << "-" << day->month << "-" << day->day;
}
int interval_days(DayTime* day2, DayTime* day1){
    tm* time1 = new tm;
    tm* time2 = new tm;
    memset(time1, 0, sizeof(tm));
    memset(time2, 0, sizeof(tm));
    daytime_to_tm(day1, time1);
    daytime_to_tm(day2, time2);
    
    // 将 tm 结构体转换为时间戳
    time_t t1 = mktime(time1);
    time_t t2 = mktime(time2);

    // 计算时间差并转换为天数
    double diff_seconds = difftime(t2, t1);
    int diff_days = static_cast<int>(diff_seconds / 86400);
    delete(time1);
    delete(time2);
    return diff_days;
}
void printPlan(Plan* plan){
    cout<< plan->id << "\t" <<
           plan->name << "\t" <<
           plan->description << "\t";
    print_day(&(plan->deadline));
    cout << "\t" << plan->status << endl;

}
bool writePlan(string name_str, string description_str, int year, int month, int day){
    
    if(name_str.length() > NAME_MAX - 1){
        printf("Your plan's name is too long.\n");
        return false;
    }

    if(description_str.length() > DESCRIPTION_MAX - 1){
        printf("Your description's name is too long.\n");
        return false;
    }
    if(year < 0 || month < 1 || month > 12 || day >31 || day < 1){
        printf("Please input day correctly: year-mon-day.\n");
        return false;
    }

    DayTime* today = new DayTime;
    DayTime* deadline = new DayTime;

    get_current_day(today);
    create_day(year, month, day, deadline);

    if(interval_days(deadline, today) < 0){
        printf("Deadline is ahead today, illegal!.\n");
        return false;
    }
    // 及时释放内存空间
    delete(today);

    int plan_capacity = sizeof(struct Plan);
    struct Plan* plan = new Plan;
    memset(plan, 0, sizeof(Plan));
    unsigned long int id;


    strcpy(plan->name, name_str.c_str());
    strcpy(plan->description, description_str.c_str());
    plan->deadline = *deadline;
    plan->status = PROGRESS;


    FILE* read_dbfile;
    read_dbfile = fopen(DB, "a");
    if (read_dbfile == NULL) {
        printf("文件打开失败\n");
        return false;
    }
    fseek(read_dbfile, 0, SEEK_END);
    // 如果文件为空，则第一条记录的id为1
    if (ftell(read_dbfile) == 0) {
        id = 0;
    } else {// 如果不为空，则读取最后一条记录的id，并在此基础上+1
        fseek(read_dbfile, -plan_capacity, SEEK_END);
        fread(&id, sizeof(unsigned long int), 1, read_dbfile);
    }
    plan->id = ++id;

    fclose(read_dbfile);


    FILE* write_dbfile;
    write_dbfile = fopen(DB, "a+");
    if (write_dbfile == NULL) {
        printf("文件打开失败\n");
        return false;
    }
    fseek(write_dbfile, 0, SEEK_END);
    fwrite(plan, sizeof(Plan), 1, write_dbfile);
    fclose(write_dbfile);
    delete(plan);
    delete(deadline);
    return true;
}
bool changeStatus(unsigned long int id, progress status){
    unsigned long int cur_id;
    FILE* read_dbfile;

    read_dbfile = fopen(DB, "r");
    if (read_dbfile == NULL) {
        printf("文件打开失败\n");
        return false;
    }
    fseek(read_dbfile, 0, SEEK_SET);
    int i = 0;
    while(fread(&cur_id, sizeof(unsigned long int), 1, read_dbfile) > 0){
        i++;
        if(cur_id == id){
            fseek(read_dbfile, i*sizeof(Plan) - sizeof(progress), SEEK_SET);
            fwrite(&status, sizeof(progress), 1, read_dbfile);
            fclose(read_dbfile);
            return true;
        }
        fseek(read_dbfile, i*sizeof(Plan), SEEK_SET);
    }
    fclose(read_dbfile);
    printf("Not such plan.\n");
    return false;
}
void printAllPlans(){
    struct Plan* plan = new Plan;
    FILE* read_dbfile;

    read_dbfile = fopen(DB, "r");
    if (read_dbfile == NULL) {
        printf("文件打开失败\n");
        return;
    }
    fseek(read_dbfile, 0, SEEK_SET);
    while(fread(plan, sizeof(Plan), 1, read_dbfile) > 0){
        printPlan(plan);
        memset(plan, 0, sizeof(Plan));
    }
    fclose(read_dbfile);
    delete(plan);
}