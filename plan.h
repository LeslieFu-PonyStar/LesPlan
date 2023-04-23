#ifndef PLAN_H
#define PLAN_H
#define NAME_MAX 128
#define DESCRIPTION_MAX 1024
#include <ctime>
#include<iostream>
#include<fstream>
#include<ctime>
#include<cstring>
enum progress{UNDO, PROGRESS, DONE};

struct DayTime{
    int year;
    int month;
    int day;
};

struct Plan
{
    unsigned long int id;  //8B
    char name[NAME_MAX]; //128B
    char description[DESCRIPTION_MAX];  //1024B
    struct DayTime deadline;
    progress status;  //4B
};
void get_current_day(DayTime* local_day);
void create_day(int year, int month, int day, DayTime* init_day);
void daytime_to_tm(DayTime* day, tm* time);
void print_day(DayTime* day);
int interval_days(DayTime* day2, DayTime* day1);
void printPlan(Plan* plan);
bool writePlan(std::string name_str, std::string description_str, int year, int month, int day);
bool changeStatus(unsigned long int id, progress status);
void printAllPlans();
#endif