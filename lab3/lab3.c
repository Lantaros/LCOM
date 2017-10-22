#include <stdio.h>
#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "kbd.h"
#include "test3.h"
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
        sys_enable_iop(SELF);


        if (argc == 1) {    /* Prints usage of the program if no arguments are passed */
                print_usage(argv);
                return 0;
        }
        else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
        printf("Usage: one of the following:\n"
                        "\t service run %s -args \"scan <0-> C, 1-> ASM >\"\n"
                        "\t service run %s -args \"leds <decimal no. - frequency>\"\n"
                        "\t service run %s -args \"timedscan <decimal no. - time>\"\n",
                        argv[0], argv[0], argv[0]);
}


static int proc_args(int argc, char **argv)
{
		//test_scan arguments
        unsigned short scan_asm;

        //test_leds arguments
        unsigned short leds_n;
		unsigned short* leds_toggle;

        //test_timed_scan arguments
        unsigned short timed_scan_n;


        if (strncmp(argv[1], "scan", strlen("scan")) == 0) {

                if (argc != 3) {
                        printf("kbd: wrong no. of arguments for kbd_test_scan()\n");
                        return 1;
                }
                scan_asm = parse_ulong(argv[2], 10);   /* Parses string to unsigned long */

                if (scan_asm == ULONG_MAX)
                        return 1;

                printf("kbd::kbd_test_scan(%d)\n", scan_asm);
                return kbd_test_scan(scan_asm);
        }

        else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {

        		leds_n = parse_ulong(argv[2], 10);      /* Parses string to unsigned long */
        		printf("leds_n= %d\n", leds_n);

                if (argc != leds_n + 3) {
                        printf("kbd: wrong no. of arguments for kbd_test_leds()\n");
                        return 1;
                }



                unsigned int size = sizeof(unsigned short) * leds_n;

                leds_toggle = (unsigned short * ) malloc(size);
                //

                // strcpy((char *)leds_toggle,argv[3]);

                int i, j = 0;
                for(i=3;i < argc; i++){
                	leds_toggle[j] = (unsigned short) parse_ulong(argv[i],10);
                	j++;
                }

                                //for each element allocate the amount of space for the number of chars in each
                                for(i=0;i < leds_n; i++){

                                	if(leds_toggle[i] < 0 || leds_toggle[i] > 2){
                                		printf("Error in kbd_test_leds: Invalid argument of led array: %d\n", leds_toggle[i]);
                                		free(leds_toggle);
                                		return 1;
                                	}

                                }

                if (leds_n == ULONG_MAX)
                        return 1;

                printf("kbd::kbd_test_leds(%lu)\n", leds_n);
                return kbd_test_leds(leds_n, leds_toggle);
        }

        else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {

                if (argc != 3) {
                        printf("kbd: wrong no of arguments for kbd_test_timed_scan()\n");
                        return 1;
                }
                timed_scan_n = parse_ulong(argv[2], 10);      /* Parses string to unsigned long */

                if (timed_scan_n == ULONG_MAX)
                        return 1;

                printf("kbd::kbd_test_timed_scan(%d)\n", timed_scan_n);
                return kbd_test_timed_scan(timed_scan_n);
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
