#include <iostream>
#include <zconf.h>
#include "uthreads.h"

int thread1, thread2, thread3, thread4, thread5, thread6, thread7;

#define GRN "\e[32m"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

void printQuantums();

void f1(){
    printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));

    int x = 0;
    for(;;) {
        x++;
        if (x%100000 == 0) {

//            printf("thread1\n");

            //todo:  termination test:
//            uthread_terminate(thread1); // because we can assume that all threads end with uthread_terminate.
//            break;

            // todo - block test
//            uthread_block(thread1); // self block.
//            uthread_block(thread2); // block of thread in ready
        }
        if (x==100000){// todo - synce test:
            uthread_sync(0);
        }

    }
}

void f2(){
    printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));

    int y = 0;
    for(;;) {
        y++;
//        if (y%100000000 == 0) {
////            printf("thread2\n");
////            uthread_resume(thread1); // todo - block test
//            uthread_terminate(thread2); // because we can assume that all threads end with uthread_terminate.
//            break;
//
//        }

        // todo - synce test:

        if (y == 10000000) { // 1 and 3 are synced with 2.
//            printf("thread2\n");
//              uthread_resume(thread1); // todo - block test
//            uthread_sync(thread1); // 2 is also synced - and by 1.
            printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));

        }
//        if (y == 1000000000 ) { // 1 and 3 are synced with 2.
////            printf("thread2\n");
//            uthread_terminate(thread2); // because we can assume that all threads end with uthread_terminate.
//            break;
//        }
    }
    return;
}
void f3(){
    printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));
    int z = 0;
    for(;;) {
        z++;
        if (z%10000000 == 0){
//            printf("thread3\n");
//            uthread_terminate(thread1);// check the terminate of the same thread twice.
//            uthread_terminate(thread2); // check the termination of other thread.
//            uthread_terminate(0); // check the termination of the main thread.
//            uthread_terminate(thread3); // because we can assume that all threads end with uthread_terminate.
//            break;

        }
//        if (z%100000000 == 0){
////            printf("thread3\n");
//            uthread_terminate(thread3); // because we can assume that all threads end with uthread_terminate.
//            break;
//
//        }

        // todo - synce test:
        if (z==100000){
//            uthread_block(thread1); // a case in which thread 1 is also blocked.
            uthread_sync(thread2);
        }
    }
    return;
}


void thread_block_myself()
{
    printf("enter thread_block_myself()\n");

    uthread_block(uthread_get_tid());
}

void threadhalt()
{
//    printf("enter threadhalt()\n");
//    printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));
//    printf("Current total Quantum in threadhalt()  %d\n",uthread_get_total_quantums());

    while (true)
    {}
}

void threadHalt10(int curTotalQuantum)
{
//    printf("enter threadhalt()\n");
//    printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));
//    printf("Current total Quantum in threadhalt()  %d\n",uthread_get_total_quantums());

    while (uthread_get_total_quantums() < curTotalQuantum + 10)
    {}
}

void waitQuantum(int curTotalQuantum){
    while(uthread_get_total_quantums()==curTotalQuantum){

    }
}

int main() {
    uthread_init(100000);


//    threadHalt10(uthread_get_total_quantums());

    thread1 = uthread_spawn(f1);
//    waitQuantum(uthread_get_total_quantums());
//    printQuantums();

//    waitQuantum(uthread_get_total_quantums());
//    waitQuantum(uthread_get_total_quantums());
//    printf("Current total Quantum after waitQuantum()  %d\n",uthread_get_total_quantums());
//    thread2 = uthread_spawn(f2);
//    printf("Current total Quantum after waitQuantum()  %d\n",uthread_get_total_quantums());
//    thread3 = uthread_spawn(threadhalt);
//    printf("Current total Quantum after waitQuantum()  %d\n",uthread_get_total_quantums());
//    thread4 = uthread_spawn(threadhalt);
//    thread5 = uthread_spawn(threadhalt);
//    thread6 = uthread_spawn(threadhalt);
//    thread7 = uthread_spawn(threadhalt);
//    uthread_sync(2);
//    uthread_terminate(2);
//    uthread_terminate(1);


//    while (true) {
//        usleep(400000);
//        printf("%d\n",uthread_get_total_quantums());
//    }

//    uthread_terminate(thread1);
//    uthread_terminate(thread2);
//    uthread_terminate(thread3);
//    uthread_terminate(thread4);
//    uthread_terminate(thread5);
//    uthread_terminate(thread6);
//    uthread_terminate(thread7);
//    uthread_terminate(0);
//    return 0;

//    std::cout << 32123456 / 1000000 << std::endl;
//    std::cout << 32123456 % 1000000 << std::endl;
//}



////    printf("start\n");
////
////    uthread_init(30000);
//    uthread_init(1000000);
//    printf("main added\n");
//
//
//    thread1 = uthread_spawn(f1);
//    printf("added 1\n");
//
//
//
//    thread2 = uthread_spawn(f2);
//    printf("added 2\n");
////
////    thread3 = uthread_spawn(f3); //
////    printf("added 3\n");
////
////    std::cout << "Hello, World!" << std::endl;
    int w = 0;
    for(;;){
        w++;
//        if (w%100000000 == 0){
////            printf("main thread\n");
////            printf("Current Quantum  -  %d\n",uthread_get_total_quantums());
//            printf("Current Quantum  of Threac %d   is   %d\n",uthread_get_tid(),uthread_get_quantums(uthread_get_tid()));
//        }
//        if (w == 100000000){ // todo - block test
//            // 2 is also synced - and by 1. released here by the main thread.
//            uthread_terminate(thread2);
////            thread4 = uthread_spawn(f2);
//            printf("added 4----------------------------------------------------------\n");
////            printf("id = %d\n", thread4.get);
////            uthread_terminate(thread1);
////            break;
//        }
//
        if (w == 1000000000) { // stop the program
            break;
        }
    }
    uthread_terminate(1);
    uthread_terminate(0);
    return 0; // exit the program - after creating the three threads - will not necessarily wait
////    // until they finish their functions! there fore we need loop.
}



