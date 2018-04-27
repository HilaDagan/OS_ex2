// Thread.cpp

#include "Thread.h"

/**
 * Initialize the new thread
 * @param id the given id.
 * @param f the entry point of this.
 */
Thread::Thread(unsigned int id, void (*f)(void), unsigned int stackSize) {
    _id = id;
    _state = READY;
    _stack.reserve(stackSize); // increase the vector capacity.
//    _function = f;
}

/**
 * Free all the allocated memory.
 */
Thread::~Thread() {
}


/**
 * Set the state of the thread
 */
void Thread::setState(ThreadState newState) {
    _state = newState;
}

/**
 * Return The state of this.
 * @return The state of this
 */
ThreadState Thread::getState() const {
    return _state;
}

/**
 * Return The id of this.
 * @return The id of this
 */
unsigned int Thread::getId() const {
    return _id;
}
