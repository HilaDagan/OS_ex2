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
    SYNCED
} ThreadState;


class Thread {
public:
    /**
     * Initialize the new thread
     * @param id the given id.
     * @param f the entry point of this.
     */
    Thread(int id, void (*f)(void), unsigned int stackSize);

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
    const std::vector<int> &getDependenciesList() const; // todo - what does it mean?

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

    static unsigned int curRunningId; //The thread ID of the current running thread;


private:
    int _id;  // the id
    ThreadState _state;  //[RUN,READY,BLOCK,SYNC]
    std::vector<int> _stack;
    std::vector<int> _dependenciesList;  // contains the id of all the threads that wait
    // for this thread to terminate.
    int _dependentIn;  // contains the thread id (if exist) that this thread is dependent of.
    //  void _function; //todo
};


#endif //UNTITLED_THREAD_H
