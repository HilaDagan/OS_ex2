//
// Created by dan.kovalsky on 4/30/18.
//

// uthread.h
/*
 * User-Level Threads Library (uthreads)
 */

#include "uthreads.h"
#include "Thread.h"
#include <algorithm> // todo - is ok?
#include <map>
#include <iostream>


/* Containers */
static std::map<int, Thread*> threadsDic; // contain all the existing threads.// todo - include the main?
// the current number of threads in the size of the dic + 1 (for the main thread)
static std::list<int> blockedThreads; // contain all threads id that are currently
// blocked.
static std::list<int> readyThreads; // contain all threads id that are currently ready
// to run.


static std::vector<int> unusedId; // threads id that are unused.
static int idCounter; // the first unused id.
//static int quantum;  // the length of a quantum in micro-seconds.
static int totalQuantum;  // the number of a quantums that were started since the library
// was initialized.
static const int MAIN_THREAD_ID = 0;
static const int FAILURE = -1;
//int timerExpired = 0; // if the value is 1, the quantum is over.
struct itimerval timer;

void getNextThread();

static int curRunningId; //The thread ID of the current running thread;
sigset_t set;


/**
 * Block the signal SIGALRM.
 */
void sigvtalrmMask(int how){
//    Thread* curThread = threadsDic[curRunningId];
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(how, &set, nullptr);
}



void getNextThread() {
    int nextId = readyThreads.front();
    readyThreads.pop_front();
//    printf("next id=%d\n", nextId);
    threadsDic[nextId]->setState(RUNNING);
    curRunningId = nextId;
    Thread *curThread = threadsDic[curRunningId];

    curThread->increasQuantums();

    if (setitimer(ITIMER_VIRTUAL, &(timer), NULL)) {
        printf("setitimer error.");
    }
    sigvtalrmMask(SIG_UNBLOCK);
    siglongjmp((curThread->_env), (curRunningId + 1));

}

/**
 * A function that implement the Round-Robin scheduling policy.
 * The scheduler decides which thread to run when:
 *    1. The library is initialized (run the main thread).
 *    2. The RUNNING thread is preempted due to: terminate itself/ blocked/put into sync.
 * The input to the function is the length of a quantum in micro-seconds.
 */
void scheduler() {
    sigvtalrmMask(SIG_BLOCK);


    Thread *curThread = threadsDic[curRunningId];
    int ret_val = sigsetjmp((curThread->_env), 1);
    printf("SWITCH: ret_val=%d\n", ret_val);
    if (ret_val != 0 ) {  // means that we restore the thread env' and not save it.
//    if (ret_val == (curThread->getId())) {  // means that we restore the thread env' and not save it.
        return;
    }

    // move the next thread in the list of READY threads to RUNNING state.
    std::list<int>::iterator it;
    for (it = readyThreads.begin(); it != readyThreads.end(); ++it) {
        std::cout << *it << std::endl;
    }

    getNextThread();
}




/*
 * Handle SIGVTALRM.
 */
void switchThreads(int sig)
{
    sigvtalrmMask(SIG_BLOCK);
    Thread *curThread = threadsDic[curRunningId];

    // Move the thread to the READY state and place it at
    // the end of the list of READY threads
    curThread->setState(READY);
    readyThreads.push_back(curRunningId);
//    printf("id to add=%d\n", curRunningId);
    sigvtalrmMask(SIG_UNBLOCK); //TODO - should we block also inside schduler?
    scheduler();
}


void setupTimer(int quantum){
    sigvtalrmMask(SIG_BLOCK);

    struct sigaction sa;

    // Install switchThreads as the signal handler for SIGVTALRM.
    sa.sa_handler = &switchThreads;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        printf("sigaction error."); //todo
    }

    // Configure the timer to expire after 1 sec... */
    timer.it_value.tv_sec = quantum/1000000;		// first time interval, seconds part
    timer.it_value.tv_usec = quantum%1000000;		// first time interval, microseconds part

    // configure the timer to expire every 3 sec after that.
//    timer.it_interval.tv_sec = 0;	// following time intervals, seconds part
//    timer.it_interval.tv_usec = quantum;	// following time intervals, microseconds part //todo

    // Start a virtual timer. It counts down whenever this process is executing.
    // ITIMER_VIRTUAL decrements only when the process is executing,
    // and delivers SIGVTALRM upon expiration.
    if (setitimer (ITIMER_VIRTUAL, &(timer), NULL)) {
        printf("setitimer error.");
    }
    sigvtalrmMask(SIG_UNBLOCK);
}


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs){
    sigvtalrmMask(SIG_BLOCK);
    if (quantum_usecs <= 0)
    {
        return FAILURE;
    }
//    quantum = quantum_usecs;
    totalQuantum = 1;  // only the current quantum.
    curRunningId = 0;  // the main thread.
    idCounter = 0;  // 0 is used by the main thread.
    int mainId = uthread_spawn(nullptr); // create the main thread.
    threadsDic[mainId]->setState(RUNNING);

    setupTimer(quantum_usecs);  // set the timer.

     //todo - the first signal can be sent only now, right?
    sigsetjmp((threadsDic[mainId]->_env), 1);
    sigvtalrmMask(SIG_UNBLOCK); //todo - ok?
    return 0;
}


/*
 * Description: This function finds the samllest non-negative integer not already
 * taken by an existing thread.
 */
int findId()  //todo - need here mask?
{
    sigvtalrmMask(SIG_BLOCK);
    int newId;
    if (unusedId.empty()) {
        newId = idCounter;
        idCounter++;
        return newId;
    }
    std::vector<int>::iterator minId = std::min_element(std::begin(unusedId),
                                                        std::end(unusedId));
    if (idCounter < *minId)
    {
        newId = idCounter;
        idCounter++;
    } else {
        newId = *minId;
        unusedId.erase(minId);
    }
    sigvtalrmMask(SIG_UNBLOCK);
    return newId;
}


/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void)){
    sigvtalrmMask(SIG_SETMASK);
    int newId;

    if (threadsDic.size() == MAX_THREAD_NUM){
        return FAILURE;
    }
    newId = findId();
    Thread *newThread = new Thread(newId, f);
    threadsDic[newId] = newThread;
    if (newId != 0) { //not the main thread
        readyThreads.push_back(newId); // add the new thread to the end of the READY threads list.
    }
    std::list<int>::iterator it;
    for (it = readyThreads.begin(); it != readyThreads.end(); ++it) {
        std::cout << *it << std::endl;

    }
    sigvtalrmMask(SIG_UNBLOCK);
    return newId;
}

/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered as an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid){ //todo
    if (threadsDic.find(tid) == threadsDic.end()) { // no thread with ID tid exist
        return FAILURE;
    }
    if (tid == MAIN_THREAD_ID) { // terminating the main thread
        //todo: releasing the assigned library memory - all the exciting threads in the dic!
        delete threadsDic[tid]; //todo - the order is fine?
        threadsDic.erase(tid);
        exit(0);
    }


    Thread *threadToTerminate = threadsDic[tid];
    ThreadState tstate = threadToTerminate->getState();
    if (tstate == READY) { // removes from the READY threads list.
        readyThreads.remove(tid);

    } else if (tstate == BLOCKED) {  // removes from the BLOCKED threads list.
        blockedThreads.remove(tid);
    }

//    removeFromDependencyList(threadToTerminate, tid); // todo - remove comment
    delete threadsDic[tid]; //todo - the order is fine?
    threadsDic.erase(tid);

    if (tid == curRunningId) {  // a thread terminates itself

        getNextThread(); //todo - not return
    }

    return 0;
}

