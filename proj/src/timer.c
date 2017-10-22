#include "timer.h"
#include "i8254.h"
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h> //libraries for sys_inb and sys_outb use
#include <minix/sysutil.h>

//these functions allow access t I/O ports
//int sys_inb(port_t port, unsignd long *byte); -> read a value into "byte" from port
//int sys_outb(port_t port, unsignd long byte); -> write a value "byte" into port
//an interrupt handler does not tak any arguments and does not return any value
//SYS_IRQCTL Kernell call functions:
//int sys_irqsetpolicy(int irq_line, int policy, int *hook_id)
//        subscribes a notification on every interrupt in the input irq_line
//  hook_id should be initialized with a value that will be used in notification
//  return value must be used in other calls to specify the interrupt notification to operate on
//int sys_irqmpolicy(int *hook_id);//  unsubscribes a previous subscription of the interrupt notification associated with the hook_id

//int sys_irqenable(int *hook_id)
//        enables interrupts on the IRQ line associated with hook_id
//        use if the policy specified in the sys_irqsetpolicy() call does not enable interrupts automatically
//int sys_irqdisable(int *hook_id);
//        disables interrupts on the IRQ line associated with the specified hook_id

//sys_irqrmpolicy(int *hook_id);
//unsubscribes a previous interrupt notification, by specifying a pointer to the
//hook_id returned by the kernel
//sys_irqdisable(int *hook_id);
//masks an interrupt line associated with a previously subscribed interrupt notification,
//by specifying a pointer to the hook_id returned by the kernel


//these operations must be specified in a file /etc/system.conf .d/

//GENERIC INTERRUPT HANDLING (GIH)
//
//1. Notifies all the device drivers (DD) interested in an interrupt, when that
//interrupt occurs
//
//2. If possible, acknowledges the interrupt by issuing the EOI command to the PIC
//
//3. Issues the IRETD instruction

//GIH knows that a DD is interested in an interrupt with kernel call:
//
//int sys_irqsetpolicy(int irq_line, int policy, int *hook_id)
//this function can be viewed as an interrupt notification subscription
//
//irq_line -> IRQ line of the interrupt
//IRQ = Interrupt ReQuest -> hardware signal sent to the processor that temporarily stops a
//running program and allows a special program, an interrupt handler, to run instead
//
//policy IRQ_REENABLE to inform the GIH that it can give the EOI (End OfInterrupt) command
//hook_id is both:
//        input: an id to be used by the kernel on interrupt notification
//        output: an id to be used by the DD in other kernel calls on this interrupt
//
//IPC (standard interprocess communication) is a mechanism used to communicate
//between processes, and between the kernel and a process
//
//DD receives a notification of the GIH by using the IPC mechanism
//
//DD sets IRQ_REENABLE policy in its interrupt subscription request(sys_irqsetpolicy())
//
//DD sends EOI to the PIC with sys_irqenable(int *hook_id)
//where hook_id points to the value returned by the kernel

int interruptCounter = 0;
int driver_receive(int num, message* numnum, int* numnumnum);


void setInterruptCounter(int arg)
{
	interruptCounter = arg;
}

int getInterruptCounter()
{
	return interruptCounter;
}

int timer_set_conf(unsigned long timer, unsigned char conf){


        if(timer > 2 || timer < 0){
                printf("Invalid timer selected\n");
                return 1;
        }
        else
        if(timer == 0){
                conf |= TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_SEL0;
        if(sys_outb(TIMER_CTRL, conf) != OK){
                printf("Error: sys_outb failed\n");
                printf("Provided arguments were:\n");
                printf("timer: %lu, configuration: %lu\n", timer, conf);
                return 1;
                }
        }

        else
                if(timer == 1){
                        conf |= TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_SEL1;
                if(sys_outb(TIMER_CTRL, conf)!= OK)
                {
                                printf("Error: sys_outb failed\n");
                                printf("Provided arguments were:\n");
                                printf("timer: %lu, configuration: %lu\n", timer, conf);
                                return 1;
                }
        }
        else if(timer == 2)
        {
                conf |= TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_SEL2;

                if(sys_outb(TIMER_CTRL, conf) != OK){
                                printf("Error: sys_outb failed\n");
                                printf("Provided arguments were:\n");
                                printf("timer: %lu, configuration: %lu\n", timer, conf);
                                return 1;
                }
        }
        return 0;
}

int timer_set_square(unsigned long timer, unsigned long freq) {

//configures a timer to generate a square wave with a given frequency
//square waves are generated in operator-mode-3

//check if provided timer is valid

        if(timer > 2){
                printf("Error: provided timer is invalid\n");
                printf("Provided timer: %s\n", timer);
                return 1;
        }

        //check if provided frequency is valid

        if(freq <= 0){
                printf("Error: provided frequency value is invalid\n");
                printf("Frequency value given: %lu\n", freq);
                return 1;
        }

        //set frequency: load timer_0 with the value of the divisor to generate
        //the frequency corresponding to the desired rate
        //higher frequency -> time is counted faster
        //lower frequency  -> time is counted slower

        unsigned long divisor = TIMER_FREQ/freq;

        //divisor &= 0xFF;        //is this conversion necessary?
        //we need to preserve the 4 least significant bits of the old_timer_configuration;

        unsigned char old_timer_configuration;

        if(timer_get_conf(timer, &old_timer_configuration) != OK){
                printf("Error in timer_set_square: timer_get_conf function failed\n");
                printf("Arguments were:\n");
                printf("timer: %lu, frequency: %lu\n", timer, freq);
                return 1;
        }

        if(timer_set_conf(timer, old_timer_configuration) != OK){
                printf("Error: timer_set_square: ");
                return 1;
        }

        if(sys_outb(TIMER_0 + timer, LSBSEL(divisor)) != OK){
                printf("Error set_square: sys_outb failed: LSB\n");
                return 1;
        }

        if(sys_outb(TIMER_0 + timer, MSBSEL(divisor)) != OK){
                printf("Error set_square: sys_outb failed: MSB\n");
                return 1;
        }
        return 0;
}

static int hook_id = 0; //Global variable, accessible by all functions


int timer_subscribe_int(void) {

        //returns the bit number that will be set in msg.NOTIFY_ARG upon a TIMER_0 interrupt

        int hookCopy = hook_id;

        if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK){
                printf("Error: timer_subscribe_int: sys_irqsetpolicy failed\n");
                return -1;//Asked in documentation
        }

        //if sys_irqsetpolicy does not enable interrupts automatically

        if(sys_irqenable(&hook_id) != OK){
                printf("Error: timer_subscribe_int: sys_irqsetpolicy failed\n");
                return -1;
        }
    return hookCopy;
}

int timer_unsubscribe_int() {

        if(sys_irqdisable(&hook_id) != OK)
                {
                        printf("Error: timer_unsubscribe_int: sys_irqdisable failed\n");
                        return 1;
                }

        if(sys_irqrmpolicy(&hook_id)!= OK)
        {
                printf("Error: timer_unsubscribe_int: sys_irqrmpolicy failed\n");
                return 1;
        }
        return 0;
}

void timer_int_handler() {
        //increments interrupt counter
        //increments on every timer interrupt (Timer 0, in this case)
        interruptCounter++;
        return;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {

        //timer = nr of timer, it can be either 0, 1 or 2
        unsigned long readback;  //read-back command to transfer to the control register
        unsigned long timerinfo; //well transfer timer info into this

        //*st = where well transfer the timer info to

        //Format of the Read-Back command:
        //| 1 | 1 | 0 | 0 | 0 | 0 | 1 | 0                 |
        //|bits | value | function               |
        //| 7,6 | 1,1   | read-back command      |
        //| 5   | 0     | read counter value     | COUNT
        //| 4   | 0     | read  programmed mode  | STATUS
        //| 3   | 1     | if=1, selects counter 2|
        //| 2   | 1     | if=1, selects counter 1|
        //| 1   | 1     | if=1, selects counter 0|
        //| 0   | 0     | reserved                                 |
        //Note: bits 4,5 are active when = 0

        //check if given timer is valid
                if(timer > 2){
                        printf("Error in timer_get_conf(): invalid timer selected\n");
                        printf("Timer selected: %d\n", timer);
                        return 1;
                        }

  readback = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

        //need to write to the control register before accessing any of the timers
        //control register @ address TIMER_CTRL
        if(sys_outb(TIMER_CTRL, readback) != OK){
                printf("Error in timer_get_conf(): error in sys_outb function\n");
                return 1;
        }

        //we can now access the timer
        if(sys_inb(TIMER_0 + timer, &timerinfo) != OK){
                printf("Error in timer_get_conf(): error in sys_inb function\n");
                return 1;
                        }

        //now transfer just read info into the *st argument
        *st = (unsigned char) timerinfo;
        return 0;
}

int timer_display_conf(unsigned char conf) {

        printf("\n\nTimer Status byte: 0x%x\n", conf);

        //Evaluate bit 7
        if(conf & BIT(7))
                printf("\nOutput Value: 1");
        else printf("\nOutput Value: 0");

        //Evaluate bit 6
        if(conf & BIT(6))
                printf("\nNull Count: 1\n");
                else printf("\nNull Count: 0\n");

        //Evaluate bits 4 and 5
        printf("Access Type: ");
        if(conf & (BIT(5)|BIT(4)))
                printf("LSB followed by MSB\n");
        else if( conf & BIT(5) )
                printf("MSB\n");
        else if( conf & BIT(4) )
                printf("LSB\n");

        //Evaluate bits 1, 2 and 3
        printf("Programmed Mode: ");
        if(conf & Mode0)
                printf("0 - Interrupt on Terminal Count\n");
        else if(conf & (BIT(1)|BIT(2)))
                printf("3 - Square Wave Mode\n");
        else if(conf & (BIT(1)|BIT(3)))
                        printf("Mode 5: Hardware Triggered Strobe (Retriggable)\n");
        else if(conf & BIT(1))
                printf("1 - Hardware Retriggable One-Shot\n");
        else if(conf & BIT(2))
                printf("2 - Rate Generator\n");
        else if(conf & BIT(3))
                printf("4 - Software Triggered Strobe\n");

        //Evaluate bit 0
        printf("Counting Mode: ");
        if(conf & BIT(0))
                printf("BCD\n");
        else printf("Binary (16 bits)\n");
        return 0;
}

int timer_test_square(unsigned long freq) {

        if(timer_set_square(TIMER_DEFAULT, freq) != OK){
                printf("Error: timer_set_square() failed\n");
                return 1;
        }

        printf("timer_set_square() executed successfully\n");
        return 0;
}

int timer_test_int(unsigned long time) {

//print one message per second, for a time interval with duration "time"
//1. subscribe Timer_0 interrupts
//2. handler should print message
//3. unsubscribe Timer_0 at the end

//        INTERRUPT LOOP:

        int ipc_status;
        message msg;
        int r;
        int irq_set ;
        irq_set = timer_subscribe_int();

        while(interruptCounter <= time*60){
                //get a request message

                if( (r = driver_receive(ANY, &msg, &ipc_status)) != OK){
                        printf("driver_receive failed with: %d", r);
                        continue;
                        //driver_receive -> used to receive messages from the kernel or other processes
                        //ANY - driver accepts messages form any process
                }
                if(is_ipc_notify(ipc_status)){ //received notification
                        switch(_ENDPOINT_P(msg.m_source)){
                        case HARDWARE: //hardware interrupt notification
                                if(msg.NOTIFY_ARG & BIT(irq_set)){ //subscribed interrupt
                                        //process it
                                        //call the interrupt handler
                                        if(interruptCounter % 60 == 0 && interruptCounter != 0)
                                                printf("Time elapsed: %d\n",interruptCounter/60);
                                        timer_int_handler();
                                }
                                break;
                                //msg.m_source - endpoint of the sender of the msg; address used to specify the source and destination of a communication
                                //_ENDPOINT_P - allows to extract the process identifier from a process's endpoint
                        default:
                                break; //no other notifications expected: do nothing
                        }
                } else{ //received a standard message, not a notification
                        //no standard messages expected: do nothing
                }
        }
        timer_unsubscribe_int();
        return 0; //ou 1??? D:
}

int timer_test_config(unsigned long timer) {

        //invokes the get_conf and display_conf functions

        unsigned char st;

        if(timer_get_conf(timer, &st) != OK){

                printf("Error: timer_get_conf failed.\n");
                return 1;
        }
        if(timer_display_conf(st) != OK){
                printf("Error: timer_display_conf failed.\n");
                return 1;
        }

        printf("timer_get_conf() and timer_display_conf() successfully executed.\n");
        return 0;
}
