/*
 * Interval-timer demo program.
 * Hebrew University OS course.
 * Author: OS, os@cs.huji.ac.il
 */

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <zconf.h>

struct itimerval timer;

int gotit = 0;


void interupt(){
    printf("interupt\n");

    timer.it_value.tv_sec = 3;		// first time interval, seconds part
    timer.it_value.tv_usec = 0;		// first time interval, microseconds part
//    // configure the timer to expire every 3 sec after that.
//    timer.it_interval.tv_sec = 3;	// following time intervals, seconds part
//    timer.it_interval.tv_usec = 0;	// following time intervals, microseconds part
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
        printf("setitimer error.");}
}

void timer_handler(int sig)
{
    gotit = 1;
    printf("Timer expired\n");
    // Start a virtual timer. It counts down whenever this process is executing.
    //    // Configure the timer to expire after 1 sec... */
//    timer.it_value.tv_sec = 1;		// first time interval, seconds part
//    timer.it_value.tv_usec = 0;		// first time interval, microseconds part
    interupt();
}



int main(void) {
    struct sigaction sa;

    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa,NULL) < 0) {
        printf("sigaction error.");
    }

//    // Configure the timer to expire after 1 sec... */
    timer.it_value.tv_sec = 1;		// first time interval, seconds part
    timer.it_value.tv_usec = 0;		// first time interval, microseconds part

    // configure the timer to expire every 3 sec after that.
    timer.it_interval.tv_sec = 10;	// following time intervals, seconds part
    timer.it_interval.tv_usec = 0;	// following time intervals, microseconds part

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
        printf("setitimer error.");
    }

    for(;;) {
        if (gotit) {
            printf("Got it!\n");
            gotit = 0;
        }
    }
}

