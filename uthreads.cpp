/*
 * User-Level Threads Library (uthreads)
 */

#include "uthreads.h"
#include "Thread.h"
#include <algorithm>
#include <map>
#include <iostream>


/* Containers */
static std::map<int, Thread*> threadsDic; // contain all the existing threads.
// the current number of threads in the size of the dic + 1 (for the main thread)
static std::list<int> blockedThreads; // contain all threads id that are currently
// blocked, OR blocked and synced.
static std::list<int> readyThreads; // contain all threads id that are currently ready
// to run.


static std::vector<int> unusedId; // threads id that are unused.
static int idCounter; // the first unused id.
//static int quantum;  // the length of a quantum in micro-seconds.
static int totalQuantum;  // the number of a quantums that were started since the library
// was initialized.
static const int MAIN_THREAD_ID = 0;
static const int FAILURE = -1;


static const int SYSTEM_ERROR_EXIT = 1;
static const int SUCCESS = 0; // represent success return value from function or exit.

//int timerExpired = 0; // if the value is 1, the quantum is over.
struct itimerval timer;

void sigvtalrmMask(int);

static int curRunningId; //The thread ID of the current running thread;
sigset_t set;
struct sigaction sa;

// Errors massages //
static const char *ERR_EMPTY_SET=  "system error: failed to empty the given set\n";

static const char *ERR_ADD_SET = "system error: failed to add the given signal to the set\n";

static const char *ERR_SET_TIMER = "system error: failed to set the timer\n";

static const char *ERR_BAD_ALLOC = "system error: bad alloc caught\n";

static const char *ERR_BAD_DELETE = "system error: ptr cannot be deleted\n";

static const char *ERR_SIGACTION = "system error: sigaction process failed\n";

static const char *LIB_ERR_INVALID_INPUT = "thread library error: invalid input\n";

static const char *LIB_ERR_MAX_THREAD = "thread library error: too many threads\n";

/**
 * Free all the memory allocated by the program.
 * @param exitVal - the value of exit code.
 */
void freeMemory(int exitVal) {
//    sigvtalrmMask(SIG_SETMASK);  //TODO DONT HAve efaect

//    try {
    std::map<int, Thread *>::iterator it;
    for (it = ++threadsDic.begin(); it != threadsDic.end(); it++) {
        printf("terminate in loop %d\n", it->first);
        delete it->second;
//        threadsDic.erase(it);
    }
    delete threadsDic[MAIN_THREAD_ID];
//    } catch (...) { // TODO - IS OK? ...
//        fprintf(stderr, ERR_BAD_DELETE);
//    }
//    threadsDic.erase(MAIN_THREAD_ID);

    sigvtalrmMask(SIG_UNBLOCK);
    exit(exitVal);
}

/**
 * Block the signal SIGVTALRM.
 */
void sigvtalrmMask(int how) {
    if (sigemptyset(&set)) {
        fprintf(stderr, ERR_EMPTY_SET);
//        freeMemory(SYSTEM_ERROR_EXIT); // TODO - Dan say to add
    }
    if (sigaddset(&set, SIGVTALRM)) {
        fprintf(stderr, ERR_ADD_SET);
//        freeMemory(SYSTEM_ERROR_EXIT);
    }
    if (sigprocmask(how, &set, nullptr)) {
        fprintf(stderr, ERR_SET_TIMER);
//        freeMemory(SYSTEM_ERROR_EXIT);
    }
}



/**
 * Move the next thread in the list of READY threads to RUNNING state.
 */
void findNextThread() {
//    printf("READYLIST before swich\n");
//    std::list<int>::iterator it; // todo - print of ready
//    for (it = readyThreads.begin(); it != readyThreads.end(); ++it) {
//        std::cout << *it << std::endl;
//    }

    int nextId = readyThreads.front();
    readyThreads.pop_front();
    threadsDic[nextId]->setState(RUNNING);
    curRunningId = nextId;
    Thread *curThread = threadsDic[curRunningId];
//    printf("------------------------\n");

    curThread->increasQuantums();
    if (setitimer(ITIMER_VIRTUAL, &(timer), nullptr)) {
        fprintf(stderr, ERR_SET_TIMER);
        freeMemory(SYSTEM_ERROR_EXIT);
    }
    totalQuantum++;
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
    Thread *curThread = threadsDic[curRunningId];
    int ret_val = sigsetjmp((curThread->_env), 1); //TODO ERRORS CHECK??

    printf("SWITCH: ret_val=%d\n", ret_val);
    if (ret_val != 0) {  // means that we want to restore the thread env' (and not save it).
        return;
    }
    findNextThread();
}


/*
 * Handle SIGVTALRM.
 */
void switchThreads(int sig) {
    sigvtalrmMask(SIG_SETMASK);
    Thread *curThread = threadsDic[curRunningId];

    // Move the thread to the READY state and place it at
    // the end of the list of READY threads
    curThread->setState(READY);
    readyThreads.push_back(curRunningId);
//    printf("id to add=%d\n", curRunningId);
    scheduler();
    sigvtalrmMask(SIG_UNBLOCK);
}

/**
 * Set up the timer object according to the user's input, and start a new quantum.
 * @param quantum - the length of a quantum in micro-seconds.
 */
void setupTimer(int quantum) {
    // Install switchThreads as the signal handler for SIGVTALRM.
    sa.sa_handler = &switchThreads;
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0) {
        fprintf(stderr, ERR_SIGACTION);
        freeMemory(SYSTEM_ERROR_EXIT);
    }

    // Configure the timer to expire after 1 sec... */
    timer.it_value.tv_sec = quantum / 1000000;  // first time interval, seconds part
    timer.it_value.tv_usec = quantum % 1000000;  // first time interval, microseconds part

    totalQuantum++;

    // Start a virtual timer. It counts down whenever this process is executing.
    // ITIMER_VIRTUAL decrements only when the process is executing,
    // and delivers SIGVTALRM upon expiration.
    if (setitimer(ITIMER_VIRTUAL, &(timer), nullptr)) {
        fprintf(stderr, ERR_SET_TIMER);
        freeMemory(SYSTEM_ERROR_EXIT);
    }
}


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs) {
    sigvtalrmMask(SIG_SETMASK);
    if (quantum_usecs <= 0) {
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }
    totalQuantum = 0;  // only the current quantum.
    idCounter = 0;  // 0 is used by the main thread.
    curRunningId = MAIN_THREAD_ID;  // the main thread.
    int mainId = uthread_spawn(nullptr);  // create the main thread.
    sigvtalrmMask(SIG_SETMASK);  // block again after spawn

    threadsDic[mainId]->setState(RUNNING);
    threadsDic[mainId]->increasQuantums();
    setupTimer(quantum_usecs);  // set the timer.

    sigsetjmp((threadsDic[mainId]->_env), 1);  //TODO ERRORS CHECK??

    printf("Before Usleep\n");
    for (int i = 0; i < 999999999; ++i) {
        for (int j = 0; j < 3; ++j) {
        }
    }
    printf("after Usleep\n");

    sigvtalrmMask(SIG_UNBLOCK);
    return SUCCESS;
}


/*
 * Description: This function finds the samllest non-negative integer not already
 * taken by an existing thread.
 */
int findId()
{
    int newId;
    if (unusedId.empty()) {
        newId = idCounter;
        idCounter++;
        return newId;
    }
    std::vector<int>::iterator minId = std::min_element(std::begin(unusedId),
                                                        std::end(unusedId));
    if (idCounter < *minId) {
        newId = idCounter;
        idCounter++;
    } else {
        newId = *minId;
        unusedId.erase(minId);
    }
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
int uthread_spawn(void (*f)(void)) {
    sigvtalrmMask(SIG_SETMASK);
    int newId;

    if (threadsDic.size() == MAX_THREAD_NUM) {
        fprintf(stderr, LIB_ERR_MAX_THREAD);
        return FAILURE;
    }
    newId = findId();

    try {
        Thread *newThread = new Thread(newId, f);
        threadsDic[newId] = newThread;
        if (newId != 0) { //not the main thread
            readyThreads.push_back(newId); // add the new thread to the end of the READY threads list.
        }
    }
    catch (std::bad_alloc &ba) {
        fprintf(stderr, ERR_BAD_ALLOC);
        freeMemory(SYSTEM_ERROR_EXIT);
    }
//    std::list<int>::iterator it; // todo - print of ready
//    for (it = readyThreads.begin(); it != readyThreads.end(); ++it) {
//        std::cout << *it << std::endl;
//
//    }
    sigvtalrmMask(SIG_UNBLOCK);
    return newId;
}


/*
 * Remove the given terminated thread from the dependencies lists of other threads.
 */
void removeFromDependencyList(const Thread *deadThread, const int id) {
    // If the dead thread was dependent in other thread, remove it from the dependency list
    // of that thread:
    if (deadThread->getDependentIn() != NOT_DEPENDENT) {
        threadsDic[deadThread->getDependentIn()]->removeDependentThread(id);
    }

    // If there are threads that are dependent in the given dead thread,
    // remove it and change their state to READY.
    std::list<int> depList = deadThread->getDependenciesList();
    std::list<int>::iterator it;
    for (it = depList.begin(); it != depList.end(); ++it) {
//        printf("released from sync: %d\n", *it);
        threadsDic[*it]->resetDependentIn();
        if (threadsDic[*it]->getState() == SYNCED) {
            threadsDic[*it]->setState(READY);
            readyThreads.push_back(*it);

        } else if (threadsDic[*it]->getState() == BLOCKED_SYNCED) {
            threadsDic[*it]->setState(BLOCKED);
            blockedThreads.push_back(*it);

        }
    }
}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid) {
    sigvtalrmMask(SIG_SETMASK);
    printf("terminate %d\n", tid);

    if (threadsDic.find(tid) == threadsDic.end()) { // no thread with ID tid exist
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }
    if (tid == MAIN_THREAD_ID) { // terminating the main thread
        //releasing the assigned library memory, all the existing threads in the dic.
        freeMemory(SUCCESS);
    }

    Thread *threadToTerminate = threadsDic[tid];
    ThreadState tstate = threadToTerminate->getState();
    if (tstate == READY) { // removes from the READY threads list.
        readyThreads.remove(tid);

    } else if (tstate == BLOCKED) {  // removes from the BLOCKED threads list.
        blockedThreads.remove(tid);
    }

    removeFromDependencyList(threadToTerminate, tid);

    unusedId.push_back(tid);
    try {
        delete threadsDic[tid];
        threadsDic.erase(tid);

        // print the dic:
        printf("print dic after deletion:\n");
        std::map<int, Thread*>::iterator ttr;
        for (ttr = threadsDic.begin(); ttr != threadsDic.end(); ++ttr) {
            printf("in dic: %d\n", ttr->first);
        }
    } catch (...) { // TODO - IS OK? ...
        fprintf(stderr, ERR_BAD_DELETE);
        freeMemory(SYSTEM_ERROR_EXIT);
    }

//    std::map<int, Thread*>::iterator it;
//
//    for (it = threadsDic.begin(); it != threadsDic.end(); ++it )
//    {
//        std::cout << int(it->first)  // string (key)
//                  << ':'
//                  << it->second->getId()   // string's value
//                  << std::endl ;
//    }

    if (tid == curRunningId) {  // a thread terminates itself
        sigvtalrmMask(SIG_UNBLOCK);
        findNextThread();
    }
    sigvtalrmMask(SIG_UNBLOCK);
    return SUCCESS;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid) {
    sigvtalrmMask(SIG_SETMASK);
    // no thread with ID tid exist or trying to block the main thread:
    if ((threadsDic.find(tid) == threadsDic.end()) || (tid == MAIN_THREAD_ID)) {
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }

    if (threadsDic[tid]->getState() != BLOCKED && threadsDic[tid]->getState() != BLOCKED_SYNCED) {
        if (threadsDic[tid]->getState() == SYNCED) { // SYNCED
//            printf("blocked synced %d\n", tid);
            threadsDic[tid]->setState(BLOCKED_SYNCED);

        } else { // READY or RUNNING
            threadsDic[tid]->setState(BLOCKED);

            if (threadsDic[tid]->getState() == READY) {
                readyThreads.remove(tid);
            }
        }

        blockedThreads.push_back(tid);

        if (curRunningId == tid) {  // a thread blocks itself
//            printf("blocked myself");
            scheduler();
        }
    }
    sigvtalrmMask(SIG_UNBLOCK);
    return SUCCESS;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid) {
    sigvtalrmMask(SIG_SETMASK);
//    printf("resumed %d\n", tid);

    if (threadsDic.find(tid) == threadsDic.end()) {  // no thread with ID tid exist
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }
    if (threadsDic[tid]->getState() == BLOCKED_SYNCED) {
        threadsDic[tid]->setState(SYNCED);
        blockedThreads.remove(tid);

    } else if (threadsDic[tid]->getState() == BLOCKED) {
        threadsDic[tid]->setState(READY);
        readyThreads.push_back(tid);
        blockedThreads.remove(tid);
    }

    sigvtalrmMask(SIG_UNBLOCK);
    return SUCCESS;
}



/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will terminate. It is considered an error if no thread with ID tid
 * exists or if the main thread (tid==0) calls this function. Immediately after the
 * RUNNING thread transitions to the BLOCKED state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid) {
    sigvtalrmMask(SIG_SETMASK);

    // no thread with ID tid exist or trying to sync the main thread or a thread try to sync itself:
    if ((threadsDic.find(tid) == threadsDic.end()) || (curRunningId == MAIN_THREAD_ID)
        || (tid == curRunningId)) {
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }

    Thread *curThread = threadsDic[curRunningId];
    curThread->setState(SYNCED);  // the current state of curThread can be only RUNNING.
    curThread->setDependentIn(tid); // the current thread is dependent on tid.
    printf("%d synced with %d\n", curRunningId, tid);
    threadsDic[tid]->addToDependenciesList(curRunningId); // add this thread to the list.
    scheduler();
    sigvtalrmMask(SIG_UNBLOCK);
    return SUCCESS;
}



/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid() {
    return curRunningId;
}


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums() {
    return totalQuantum;
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid) {
    sigvtalrmMask(SIG_SETMASK);
    // no thread with ID tid exist or trying to sync the main thread:
    if ((threadsDic.find(tid) == threadsDic.end())) {
        fprintf(stderr, LIB_ERR_INVALID_INPUT);
        return FAILURE;
    }
    sigvtalrmMask(SIG_UNBLOCK);
    return threadsDic[tid]->getQuantums();
}



