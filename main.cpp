#include <iostream>
#include "uthreads.h"

int thread1, thread2, thread3;



void f1(){
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
            uthread_sync(thread2);
        }

    }
}

void f2(){
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

        if (y == 100000000) { // 1 and 3 are synced with 2.
//            printf("thread2\n");
              uthread_resume(thread1); // todo - block test

        }
        if (y == 1000000000 ) { // 1 and 3 are synced with 2.
//            printf("thread2\n");
            uthread_terminate(thread2); // because we can assume that all threads end with uthread_terminate.
            break;
        }
    }
    return;
}
void f3(){
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
            uthread_block(thread1); // a case in which thread 1 is also blocked.
            uthread_sync(thread2);
        }
    }
    return;
}

int main()

{

//    std::cout << 32123456/1000000<<std::endl ;
//    std::cout << 32123456%1000000<<std::endl ;

    printf("start\n");

    uthread_init(30000);
    printf("main added\n");


    thread1 = uthread_spawn(f1);
    printf("added 1\n");

    thread2 = uthread_spawn(f2);
    printf("added 2\n");

    thread3 = uthread_spawn(f3); //
    printf("added 3\n");

    std::cout << "Hello, World!" << std::endl;
    int w = 0;
    for(;;){
        w++;
//        if (w%100000 == 0){
//            printf("main thread\n");

//        }
//        if (w == 100000000){
//            break;
//        }

    }
    return 0; // exit the program - after creating the three threads - will not necessarily wait
    // until they finish their functions! there fore we need loop.
}