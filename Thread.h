// Thread.h

#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H
#include <vector>


/**
 * @brief Represent the possible states of a thread.
 */
typedef enum ThreadState
{
    READY,
    RUNNING,
    BLOCKED,
    SYNCED,
} ThreadState;


class Thread {
public:
    /**
     * Initialize the new thread
     * @param id the given id.
     * @param f the entry point of this.
     */
    Thread(unsigned int id, void (*f)(void), unsigned int stackSize);

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
    unsigned int getId() const;

    static unsigned int curRunningId; //The thread ID of the current running thread;


private:
    unsigned int _id;        // the id
    ThreadState _state;     //[RUN,READY,BLOCK,SYNC]
    std::vector<int> _stack;

//    void _function; //todo
    std::vector<unsigned int> _dependenciesInThis; // contains the id of all the threads that wait
    // for this thread to terminate.
    std::vector<unsigned int> _thisDependentIn; // contains the id of all the threads that this
    // thread is dependent of.
};


#endif //UNTITLED_THREAD_H
