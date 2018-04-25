//
// Created by dan.kovalsky on 4/25/18.
//

#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H


#include <vector>

class Thread {
public:
    /**
     * Initialize the new thread
     * @param id the given id.
     * @param f the entry point of this.
     */
    Thread(unsigned int id, void (*f)(void));

    /**
     * Free all the allocated memory.
     */
    ~Thread();

    static unsigned int curRunningId; //The thread ID of the current running thread;

    /**
     * Set the state of the thread
     */
    void setState(unsigned int newState);

    /**
     * Return The state of this.
     * @return The state of this
     */
    unsigned int getState() const;

    /**
     * Return The id of this.
     * @return The id of this
     */
    unsigned int getId() const;

private:

    unsigned int _id;        // the id
    unsigned int _state;     //[RUN,READY,BLOCK,SYNC]
//    void _function;
    std::vector<unsigned int> _synced; // contains the id of all the threads that wait for this thread to terminate.



};


#endif //UNTITLED_THREAD_H
