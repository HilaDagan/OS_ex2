#include <iostream>
#include "uthreads.h"





void f1(){
    int x = 0;
    for(;;) {
        x++;
        if (x%100000 == 0){
//            printf("thread1\n");
        }
    }
    return;
}

void f2(){
    int y = 0;
    for(;;) {
        y++;
        if (y%100000 == 0) {
//            printf("thread2\n");
        }
    }
    return;
}
void f3(){
    int z = 0;
    for(;;) {
        z++;
        if (z%100000 == 0){
//            printf("thread3\n");
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


    int thread1 = uthread_spawn(f1);
    printf("added 1\n");

    int thread2 = uthread_spawn(f2);
    printf("added 2\n");

    int thread3 = uthread_spawn(f3); //
    printf("added 3\n");

    std::cout << "Hello, World!" << std::endl;
    int w = 0;
    for(;;){
        w++;
        if (w%100000 == 0){
            printf("main thread\n");
        }

    }
    return 0; // exit the program - after creating the three threads - will not necessarily wait
    // until they finish their functions! there fore we need loop.
}