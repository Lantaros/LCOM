#include <stdio.h>
#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "test4.h"
#include <limits.h>
#include <errno.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <string.h>

//Prototypes
static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
        sef_startup(); //initializes service - initialization of privileged user processes


        if (argc == 1) {    /* Prints usage of the program if no arguments are passed */
                print_usage(argv);
                return 0;
        }
        else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
        printf("Usage: one of the following:\n"
                        "\t service run %s -args \"packet <number of packets>\"\n"
                        "\t service run %s -args \"async <decimal no. - time>\"\n"
                        "\t service run %s -args \"config\"\n"
			"\t service run %s -args \"gesture <decimal no. - lentght>\"\n",
                        argv[0], argv[0], argv[0],argv[0]);
}


static int proc_args(int argc, char **argv)
{
		//test_packets arguments
        unsigned short cnt;

        //test_async arguments
        unsigned short idle_time;

        //test_config arguments
        //(void...)

        //test_gesture
		short length;


        if (strncmp(argv[1], "packet", strlen("packet")) == 0) {

		if (argc != 3) {
			printf("ps/2: wrong no. of arguments for test_packet()\n");
			return 1;
		}

		cnt = parse_ulong(argv[2], 10); /* Parses string to unsigned long */

		if (cnt == ULONG_MAX)
			return 1;

		printf("ps/2::test_packet(%d)\n", cnt);
		return test_packet(cnt);
	}

	else if (strncmp(argv[1], "async", strlen("async")) == 0) {

		if (argc != 3) {
			printf("kbd: wrong no. of arguments for test_async()\n");
			return 1;
		}

		idle_time = parse_ulong(argv[2], 10); /* Parses string to unsigned long */

		if (idle_time == ULONG_MAX)
			return 1;


		printf("ps/2::test_async(%d)\n", idle_time);
		return test_async(idle_time);

        }

        else if (strncmp(argv[1], "config", strlen("config")) == 0) {

                if (argc != 2) {
                        printf("mouse: wrong no of arguments for test_config()\n");
                        return 1;
                }
                return test_config();
        }

        else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {

                       if (argc != 3) {
                               printf("kbd: wrong no of arguments for test_gesture()\n");
                               return 1;
                       }
                       length = parse_ulong(argv[2], 10);      /* Parses string to unsigned long */

                       if (length == ULONG_MAX)
                               return 1;

                       printf("kbd::test_gesture(%d)\n", length);

                       //return test_gesture(length);
                       return 0;
               }


        else {
                printf("kbd: %s - no valid function!\n", argv[1]);
                return 1;
        }
}

static unsigned long parse_ulong(char *str, int base)
{
        char *endptr;
        unsigned long val;

        /* Convert string to unsigned long */
        val = strtoul(str, &endptr, base);

        /* Check for conversion errors */
        if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
                perror("strtoul");
                return ULONG_MAX;
        }
        if (endptr == str) {
                printf("timer: parse_ulong: no digits were found in %s\n", str);
                return ULONG_MAX;
        }
        /* Successful conversion */
        return val;
}
