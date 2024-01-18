#include <stdio.h>
#include <stdlib.h>
#include "timelib.h"
#define BITS_POSITION 17
#define DATE_BITS 15
#define BITS_PER_INT 32
#define EIGHT 8
#define SEVEN 7
#define EXTRACT_DAY 0x1F
#define EXTRACT_MONTH 0x0F
#define EXTRACT_YEAR 0x3F
#define FIRST_YEAR 1970
#define SHIFT_FIVE 5
#define SHIFT_NINE 9

// Compare function for qsort
int compareDates(const void *a, const void *b) {
    TDate *date1 = (TDate *)a;
    TDate *date2 = (TDate *)b;

    if (date1->year != date2->year) {
        return (int)(date1->year - date2->year);
    }
    if (date1->month != date2->month) {
        return (int)(date1->month - date2->month);
    }
    return (int)(date1->day - date2->day);
}
// Function to unpack the date from the packed date
TDate unpackDate(unsigned int packedDate) {
    TDate date;
    date.day = packedDate & EXTRACT_DAY;                     // Extract day (5 bits)
    date.month = (packedDate >> SHIFT_FIVE) & EXTRACT_MONTH;  // Extract month (4 bits)
    date.year = (packedDate >> SHIFT_NINE) & EXTRACT_YEAR;   // Extract year (6 bits)
    date.year += FIRST_YEAR;                                 // Adjust year
    return date;
}

// Function to count bits set to 1 for parity check
int countBits(unsigned int number) {
    unsigned int count = 0;
    while (number) {
        count += (unsigned int)(number & 1);
        number >>= 1;
    }
    return (int)count;
}

// Function to unpack the bits from the array of ints
TDate unpackDateFromBits(unsigned int *data, int bitStart) {
    unsigned int packedDate = 0;
    for (int i = 0; i < DATE_BITS; i++) {
        int bitValue = (int)(data[(bitStart + i) / BITS_PER_INT] >> ((bitStart + i) % BITS_PER_INT)) & 1;
        packedDate |= (bitValue << i);
    }
    return unpackDate(packedDate);
}

int main() {
    int task_number = 0;
    scanf("%d", &task_number);

    if (task_number == SEVEN) {
        int num_dates = 0;
        scanf("%d", &num_dates);
        const char *months[] = {
            "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
            "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};

        // Allocate memory for the packed and unpacked dates
        unsigned int *packed_dates = (unsigned int *)malloc(num_dates * sizeof(unsigned int));
        TDate *unpacked_dates = (TDate *)malloc(num_dates * sizeof(TDate));
        // Read the packed dates
        for (int i = 0; i < num_dates; i++) {
            scanf("%u", &packed_dates[i]);
            unpacked_dates[i] = unpackDate(packed_dates[i]);  // Unpack the date
        }
        // Sort the unpacked dates
        qsort(unpacked_dates, num_dates, sizeof(TDate), compareDates);

        // Print the unpacked dates
        for (int i = 0; i < num_dates; i++) {
            printf("%d %s %04d\n", unpacked_dates[i].day, months[unpacked_dates[i].month - 1], unpacked_dates[i].year);
        }

    } else if (task_number == EIGHT) {
        int numDates = 0;
        scanf("%d", &numDates);
        const char *months[] = {
            "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
            "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};

        // Allocate memory for the number of ints needed to store the packed dates
        int numInts = (numDates * DATE_BITS + BITS_PER_INT - 1) / BITS_PER_INT;

        unsigned int *packedData = (unsigned int *)malloc(numInts * sizeof(unsigned int));
        for (int i = 0; i < numInts; i++) {
            scanf("%u", &packedData[i]);
        }

        // Allocate memory for the number of ints needed to store the control data
        int controlInts = (numInts + BITS_PER_INT - 1) / BITS_PER_INT;
        unsigned int *controlData = (unsigned int *)malloc(controlInts * sizeof(unsigned int));
        for (int i = 0; i < controlInts; i++) {
            scanf("%u", &controlData[i]);
        }

        TDate *dates = (TDate *)malloc(numDates * sizeof(TDate));
        int datesIndex = 0;
        int bitIndex = 0;
        int myPackedDate = -1;  // Used to store the date that spans two ints
        int last = 0;
        // Iterate through the packed data
        for (int i = 0; i < numDates; i++) {
            bitIndex = i * DATE_BITS;
            int currentInt = bitIndex / BITS_PER_INT;
            int controlBitIndex = currentInt / BITS_PER_INT;
            int controlBitPosition = currentInt % BITS_PER_INT;

            // Extract the control bit
            int controlBit = (int)((controlData[controlBitIndex] >> controlBitPosition) & 1);
            int parity = countBits(packedData[currentInt]) % 2;

            // If the control bit matches the parity bit then the data is not corrupted
            if (controlBit == parity) {  // Data is not corrupted
                if (myPackedDate != -1) {
                    TDate unpackedDate = unpackDate(myPackedDate);
                    dates[datesIndex++] = unpackedDate;
                    myPackedDate = -1;
                }
                // Check if this is the last date
                if (last == 1) {
                    break;
                }
                // Check if the date is stored in one int or two
                if (bitIndex % BITS_PER_INT <= BITS_POSITION) {
                    TDate unpackedDate = unpackDateFromBits(packedData, bitIndex);
                    dates[datesIndex++] = unpackedDate;
                } else {
                    // If the date is stored in two ints, combine the two ints
                    int remainingBits = BITS_PER_INT - (bitIndex % BITS_PER_INT);
                    int remainingNumber = (int)(packedData[currentInt] >> (BITS_PER_INT - remainingBits));
                    int nextNumber = (int)(packedData[currentInt + 1] << remainingBits);
                    myPackedDate = remainingNumber | nextNumber;
                    if (numDates == i + 1) {
                        last = 1;
                        numDates++;
                    }
                }
            } else {
                // If the control bit does not match, skip all dates in this int
                int remainingBits = BITS_PER_INT - (bitIndex % BITS_PER_INT);
                int datesToSkip = (remainingBits + DATE_BITS - 1) / DATE_BITS;
                i += datesToSkip - 1;  // Adjust loop index for skipped dates
                myPackedDate = -1;
            }
            bitIndex += DATE_BITS;  // Move to the next date
        }

        // Sort and print dates
        qsort(dates, datesIndex, sizeof(TDate), compareDates);
        for (int i = 0; i < datesIndex; i++) {
            printf("%d %s %04d\n", dates[i].day, months[dates[i].month - 1], dates[i].year);
        }
    }
    return 0;
}
