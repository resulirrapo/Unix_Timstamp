#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "timelib.h"
#define SECONDS_PER_HOUR 3600
#define MAX_TIMEZONES 50
#define MAX_PEOPLE 100
#define MAX_NAME_LENGTH 11  // 10 characters + null terminator
#define MAX_TIMEZONE_NAME_LENGTH 5  // 4 characters + null terminator
#define MAX_AVAILABILITIES 50

typedef struct {
    unsigned int start_timestamp;
    unsigned int duration_hours;
} Availability;

typedef struct {
    char name[MAX_NAME_LENGTH];
    TTimezone *timezone;
    int num_availabilities;
    Availability availabilities[MAX_AVAILABILITIES];
} Person;

// Function to read timezones
void readTimezones(TTimezone timezones[], int numOfTimezones) {
    for (int i = 0; i < numOfTimezones; ++i) {
        scanf("%s %hhd", timezones[i].name, &timezones[i].utc_hour_difference);
    }
}

// Function to find timezone by name
TTimezone* findTimezoneByName(TTimezone timezones[], const char* name) {
    for (int i = 0; i < MAX_TIMEZONES; i++) {
        if (strcmp(timezones[i].name, name) == 0) {
            return &timezones[i];
        }
    }
    return NULL;  // Timezone not found
}

// Function to read person data and their availability
void readPeople(TTimezone timezones[], Person people[], int numOfPeople) {
    char timezoneName[MAX_TIMEZONE_NAME_LENGTH];
    for (int i = 0; i < numOfPeople; ++i) {
        scanf("%s %s %d", people[i].name, timezoneName, &people[i].num_availabilities);
        people[i].timezone = findTimezoneByName(timezones, timezoneName);
        // Read availabilities
        for (int j = 0; j < people[i].num_availabilities; j++) {
            TDateTimeTZ startDateTime;
            unsigned int year = 0, month = 0, day = 0, hour = 0;
            scanf("%u %u %u %u %u", &year, &month, &day, &hour, &people[i].availabilities[j].duration_hours);
            startDateTime.date.year = year;
            startDateTime.date.month = month;
            startDateTime.date.day = day;
            startDateTime.time.hour = hour;
            startDateTime.time.min = 0;
            startDateTime.time.sec = 0;
            startDateTime.tz = people[i].timezone;
            people[i].availabilities[j].start_timestamp = convertDateTimeTZToUnixTimestamp(startDateTime);
        }
    }
}
// Function to find the first suitable time interval
int findFirstSuitableTimeInterval(Person people[], int eventDuration, int minParticipants, int numOfPeople) {
    // Check for each availability of each person
    for (int i = 0; i < numOfPeople; i++) {
        for (int j = 0; j < people[i].num_availabilities; j++) {
            int count = 0;
            unsigned int start = people[i].availabilities[j].start_timestamp;
            unsigned int end = start + eventDuration * SECONDS_PER_HOUR;

            // Compare with every other person's availability
            for (int k = 0; k < numOfPeople; k++) {
                bool isAvailable = false;
                for (int l = 0; l < people[k].num_availabilities; l++) {
                    unsigned int pStart = people[k].availabilities[l].start_timestamp;
                    unsigned int pEnd = pStart + people[k].availabilities[l].duration_hours * SECONDS_PER_HOUR;

                    // Check if the person is available in the required time slot
                    if (pStart <= start && pEnd >= end) {
                        isAvailable = true;
                        break;
                    }
                }
                if (isAvailable) count++;
            }

            // Check if the minimum number of participants is met
            if (count >= minParticipants) {
                return (int)start;  // Suitable time interval found
            }
        }
    }
    return -1;  // No suitable time interval found
}

void outputResults(TTimezone timezones[], Person people[], int startTime,
 int minParticipants, unsigned int eventDuration, int numOfPeople) {
    if (startTime == -1) {
        printf("imposibil \n");
        return;
    }

    const char* monthNames[] = {
        "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
        "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"
    };
    // Print the suitable time interval
    for (int i = 0; i < numOfPeople; i++) {
        unsigned int localStartTime = startTime + people[i].timezone->utc_hour_difference * SECONDS_PER_HOUR;
        TDateTimeTZ localStartDateTime = convertUnixTimestampToDateTimeTZ(localStartTime, timezones, MAX_TIMEZONES);

        // Check if each person is available at the event start time
        bool canAttend = false;
        for (int j = 0; j < people[i].num_availabilities; j++) {
            unsigned int pStart = people[i].availabilities[j].start_timestamp;
            unsigned int pEnd = pStart + people[i].availabilities[j].duration_hours * SECONDS_PER_HOUR;

            // Check if the person is available in the required time slot
            if (pStart <= localStartTime && pEnd >= localStartTime + eventDuration * SECONDS_PER_HOUR) {
                canAttend = true;
                break;
            }
        }
        if (canAttend) {
            printf("%s: %d %s %d, %02d:%02d:%02d %s (UTC%+d) \n",
                   people[i].name,
                   localStartDateTime.date.day,
                   monthNames[localStartDateTime.date.month - 1],
                   localStartDateTime.date.year,
                   localStartDateTime.time.hour,
                   localStartDateTime.time.min,
                   localStartDateTime.time.sec,
                   people[i].timezone->name,
                   people[i].timezone->utc_hour_difference);
        } else {
            printf("%s: invalid\n", people[i].name);
        }
    }
}

int main() {
    int numOfTimezones = 0, numOfPeople = 0, minParticipants = 0;
    int eventDuration = 0;
    Person people[MAX_PEOPLE];
    TTimezone timezones[MAX_TIMEZONES];
    scanf("%d", &numOfTimezones);
    readTimezones(timezones, numOfTimezones);

    scanf("%d", &numOfPeople);
    readPeople(timezones, people, numOfPeople);

    // Reading event details
    scanf("%d %u", &minParticipants, &eventDuration);

    // Finding a suitable time interval
    int startTime = (int)findFirstSuitableTimeInterval(people, eventDuration, minParticipants, numOfPeople);

    // Output results
    outputResults(timezones, people, startTime, minParticipants, eventDuration, numOfPeople);

    return 0;
}

