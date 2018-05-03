//
// Created by dan.kovalsky on 5/3/18.
//

#include <iostream>
#include <zconf.h>
#include "uthreads.h"

int thread1, thread2, thread3, thread4, thread5, thread6, thread7;

#define GRN "\e[32m"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

void printQuantums();



void thread_block_myself()
{
    printf("enter thread_block_myself()\n");

    uthread_block(uthread_get_tid());
}

void threadhalt()
{
    while (true)
    {}
}

void threadHalt10(int curTotalQuantum)
{
    while (uthread_get_total_quantums() < curTotalQuantum + 10)
    {}
}

void waitQuantum(int curTotalQuantum){
    while(uthread_get_total_quantums()==curTotalQuantum){

    }
}

void f1(){
    uthread_sync(2);
    printf("Thread 1 come back");
    waitQuantum(uthread_get_total_quantums());

}

void f2(){
    printf("Thread 2 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());
    uthread_sync(5);
}

void f3(){
//    printf("Thread 3 synced with 2\n");
    uthread_sync(2);
    printf("Thread 3 come back");
    waitQuantum(uthread_get_total_quantums());
}

void f4(){
    printf("Thread 4 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());
    printf("here\n");
    uthread_block(2);
    waitQuantum(uthread_get_total_quantums());
    waitQuantum(uthread_get_total_quantums());
    waitQuantum(uthread_get_total_quantums());


}
void f5(){
    while (true){}

}




int main() {
    uthread_init(1000000);
    thread1 = uthread_spawn(f1);
    thread2 = uthread_spawn(f2);
    thread3 = uthread_spawn(f3);
    thread4 = uthread_spawn(f4);
    thread5 = uthread_spawn(f5);
//    thread6 = uthread_spawn(f1);

//    threadHalt10(uthread_get_total_quantums());
    printf("Thread 0 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());
    printf("Thread 0 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());
    uthread_resume(2);
    printf("2 realesed\n");

    printf("Thread 0 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());

    uthread_terminate(2);
    printf(" 1 and 3 are back\n");
    printf("Thread 0 waits one quantum 2\n");
    waitQuantum(uthread_get_total_quantums());

    uthread_terminate(7); // we want to wee error.
    printf("cur TID = %d\n",uthread_get_tid());
    uthread_terminate(thread1);
    uthread_terminate(0);
    return 0;

}



