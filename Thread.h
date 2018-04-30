// Thread.h

#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H
#include <vector>
#include <list>
#include "uthreads.h"
#include <cstdio>
#include <csetjmp>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>

/**
 * @brief Represent the possible states of a thread.
 */
typedef enum ThreadState
{
    READY,
    RUNNING,
    BLOCKED,
    SYNCED
} ThreadState;

/**
 * Used when this thread doesn't dependent on any other thread.
 */
const static int NOT_DEPENDENT = -1;


//======================= Definition of address_t =======================//


typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7





// ======================= end of definition =======================//



class Thread {
public:

    /**
     * Initialize the new thread
     * @param id the given id.
     * @param f the entry point of this.
     */
    Thread(int id, void (*f)(void));


    /**
     * Free all the allocated memory.
     */
    ~Thread();

    /**
     * Set the state of the thread
     */
    void setState(ThreadState newState);

    /**
     * Return The state of this.
     * @return The state of this
     */
    ThreadState getState() const;

    /**
     * Return The id of this.
     * @return The id of this
     */
    int getId() const;

    /**
     * Reset the thread id of the thread that 'this' dependent on, to be null.
     */
    void resetDependentIn();

    /**
     * @return the thread id of all the threads that are depend on this thread.
     */
    const std::list<int> &getDependenciesList() const; // todo - what does it mean?

    /**
    * Add a new thread id to the dependency list.
    */
    void addToDependenciesList(const int newId);

    /**
     * remove the given thread id from the dependencies list.
     */
    void removeDependentThread(int tid);

    /**
     * @return the thread id of a thread that this id dependent of.
     */
    int getDependentIn() const;

    /**
     * Set the thread id of a thread that this id is dependent of.
     */
    void setDependentIn(int _dependentIn);

    /**
     * @return the number of quantums that 'this' was in RUNNING state.
     */
    int getQuantums() const;

    /**
     * Increase the number of quantums that 'this' was in RUNNING state.
     */
    void increasQuantums();

//    static int curRunningId; //The thread ID of the current running thread;
    sigjmp_buf _env;

    /* A translation is required when using an address of a variable.
    Use this as a black box in your code. */
    address_t translate_address(address_t addr);

private:
    int _id;  // the thread id

    ThreadState _state;  // one of: [RUN,READY,BLOCK,SYNC]

    char _stack[STACK_SIZE];

    // contains the id of all the threads that wait
    // for this thread to terminate.
    std::list<int> _dependenciesList;

    int _dependentIn;  // contains the thread id (if exist) that this thread is dependent of.

    void (*_function)(void);

    address_t _sp, _pc;

    int _quantumsNum;  // the number of quantums that 'this' was in RUNNING state.


    /**
     * Setup the stack environment of the thread.
     */
    void setupEnv();
};


#endif //UNTITLED_THREAD_H
