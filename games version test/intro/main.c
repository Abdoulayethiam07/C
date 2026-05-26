#include<stdio.h>
#define true 1
#define false 0 
int days_in_month[] = {31, 28, 31, 30,31,30,31,30,31,30,31,30};
char*month_name[]=
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};
int inputyear()
{
    int year;
    printf("entrer une annnee comme exemple 2024: ");
    scanf("%d", &year);
    return year;
}
int determine_leap_year(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        days_in_month[2]= 29;
        return true;
    }
    else
    {
        return false;
    }
}
void print_calendar(int year)
{
    printf("Calendar for the year %d:\n", year);
    for (int month = 0; month < 12; month++)
    {
        printf("%s: %d days\n", month_name[month], days_in_month[month]);
    }
}
int main()
{
    int year = inputyear();
    determine_leap_year(year);
    print_calendar(year);
    return 0;
}
//Description:
//This C program prompts the user to enter a year, determines if it is a leap year, and then prints the calendar for that year, showing the number of days in each month. The program uses arrays to store the number of days in each month and the names of the months. The leap year calculation is done using the standard rules for leap years.

windowsshou
