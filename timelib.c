#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define SECONDS_PER_DAY 86400
#define SECONDS_PER_LEAP_YEAR 31622400
#define SECONDS_PER_NON_LEAP_YEAR 31536000
#define LEAP_YEAR 366
#define NORMAL_YEAR 365
#define FIRST_YEAR 1970
#define FIRS_MONTH 1
#define FIRST_DAY 1
#define NORMAL_FEBRUARY 28
#define LEAP_FEBRUARY 29
#define NORMAL_MONTH 30
#define LONG_MONTH 31
#define IUNIE 6
#define SEPTEMBRIE 9
#define NOIEMBRIE 11
#define ONEHUNDRED 100
#define FOURHUNDRED 400



// TODO(Rrapo Resuli) Task 1
TTime convertUnixTimestampToTime(unsigned int timestamp) {
    TTime result;
    result.sec = timestamp % SECONDS_PER_MINUTE;
    timestamp /= SECONDS_PER_MINUTE;
    result.min = timestamp % MINUTES_PER_HOUR;
    timestamp /= MINUTES_PER_HOUR;
    result.hour = timestamp % HOURS_PER_DAY;
    return result;
}

// TODO(Rrapo Resuli) Task 2
TDate convertUnixTimestampToDateWithoutLeapYears(unsigned int timestamp) {
    TDate result;

    // Calculate the number of days since 1970
    unsigned int total_days = timestamp / SECONDS_PER_DAY;

    // Initialize variables for year, month, and day
    unsigned int year = FIRST_YEAR;
    unsigned int month = FIRS_MONTH;
    unsigned int day = FIRST_DAY;

    // Calculate the year
    while (total_days >= NORMAL_YEAR) {
        total_days -= NORMAL_YEAR;
        year++;
    }

    // Determine the month and day within the current year
    while (total_days > 0) {
        unsigned int days_in_month = LONG_MONTH;  // Initialize with maximum days
        if (month == 4 || month == IUNIE || month == SEPTEMBRIE || month == NOIEMBRIE) {
            days_in_month = NORMAL_MONTH;
        } else if (month == 2) {
            days_in_month = NORMAL_FEBRUARY;  // Non-leap year February
        }
        // Check if the remaining days fit in the current month
        if (total_days >= days_in_month) {
            total_days -= days_in_month;
            month++;
        } else {
            day += total_days;
            total_days = 0;  // Exit the loop
        }
    }

    result.year = year;
    result.month = month;
    result.day = day;

    return result;
}
// TODO(Rrapo Resuli) Task 3
TDate convertUnixTimestampToDate(unsigned int timestamp) {
    TDate result;

    // Calculate the number of days since 1970
    unsigned int total_days = timestamp / SECONDS_PER_DAY;

    // Initialize variables for year, month, and day
    unsigned int year = FIRST_YEAR;
    unsigned int month = FIRS_MONTH;
    unsigned int day = FIRST_DAY;

    // Calculate the year, considering leap years
    while (total_days >= NORMAL_YEAR) {
        if ((year % 4 == 0 && year % ONEHUNDRED != 0) || (year % FOURHUNDRED == 0)) {
            // Leap year
            if (total_days >= LEAP_YEAR) {
                total_days -= LEAP_YEAR;
                year++;
            } else {
                break;
            }
        } else {
            total_days -= NORMAL_YEAR;
            year++;
        }
    }

    // Determine the month and day within the current year, considering leap years
    while (total_days > 0) {
        unsigned int days_in_month = LONG_MONTH;  // Initialize with maximum days
        if (month == 4 || month == IUNIE || month == SEPTEMBRIE || month == NOIEMBRIE) {
            days_in_month = NORMAL_MONTH;
        } else if (month == 2) {
            if ((year % 4 == 0 && year % ONEHUNDRED != 0) || (year % FOURHUNDRED == 0)) {
                days_in_month = LEAP_FEBRUARY;  // Leap year February
            } else {
                days_in_month = NORMAL_FEBRUARY;  // Non-leap year February
            }
        }
        if (total_days >= days_in_month) {
            total_days -= days_in_month;
            month++;
        } else {
            day += total_days;
            break;
        }
    }
    result.year = year;
    result.month = month;
    result.day = day;

    return result;
}


// TODO(Rrapo Resuli) Task 4
TDateTimeTZ convertUnixTimestampToDateTimeTZ(unsigned int timestamp, TTimezone *timezones, int timezone_index) {
    TDateTimeTZ result;

    // Convert the Unix Timestamp to the specified timezone
    int utc_hour_difference = timezones[timezone_index].utc_hour_difference;
    unsigned int adjusted_timestamp = timestamp + (utc_hour_difference * SECONDS_PER_HOUR);  // Convert hours to seconds

    // Get the date and time in the adjusted timezone
    result.date = convertUnixTimestampToDate(adjusted_timestamp);
    result.time = convertUnixTimestampToTime(adjusted_timestamp);
    result.tz = &timezones[timezone_index];

    return result;
}

// TODO(Rrapo Resuli) Task 5
unsigned int convertDateTimeTZToUnixTimestamp(TDateTimeTZ datetime) {
    unsigned int timestamp = 0;
    unsigned int total_days = 0;
    int year = 0, month = 0;

    // Calculate the total number of days from the year
    for (year = FIRST_YEAR; year < datetime.date.year; ++year) {
        total_days += NORMAL_YEAR;
        if ((year % 4 == 0 && year % ONEHUNDRED != 0) || (year % FOURHUNDRED == 0)) {
            // Add an extra day for leap years
            total_days++;
        }
    }

    // Calculate the total number of days for the months
    static const unsigned int daysInMonths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (month = 0; month < datetime.date.month - 1; ++month) {
        total_days += daysInMonths[month];
        if (month == 1 && ((datetime.date.year % 4 == 0 && datetime.date.year % ONEHUNDRED != 0)
         || (datetime.date.year % FOURHUNDRED == 0))) {
            // Add an extra day for February in leap years
            total_days++;
        }
    }

    // Add the days in the current month
    total_days += datetime.date.day - 1;

    // Convert the total days to seconds
    timestamp = total_days * SECONDS_PER_DAY;

    // Add the time in seconds
    timestamp += datetime.time.hour * SECONDS_PER_HOUR;
    timestamp += datetime.time.min * SECONDS_PER_MINUTE;
    timestamp += datetime.time.sec;

    // Adjust for the timezone difference to get UTC
    timestamp -= datetime.tz->utc_hour_difference * SECONDS_PER_HOUR;

    return timestamp;
}

// TODO(Rrapo Resuli) Task 6
void printDateTimeTZ(TDateTimeTZ datetimetz) {
    const char* monthNames[] = {
    "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
    "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"
};
    printf("%02d %s %d, %02d:%02d:%02d %s (UTC%+d)\n",
           datetimetz.date.day,
           monthNames[datetimetz.date.month - 1],
           datetimetz.date.year,
           datetimetz.time.hour,
           datetimetz.time.min,
           datetimetz.time.sec,
           datetimetz.tz->name,
           datetimetz.tz->utc_hour_difference);
}
