// Thread.cpp

#include "Thread.h"


Thread::Thread(unsigned int id, void (*f)(void)) {
    _id = id;
    _state = READY;
//    _function = f;

}

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
unsigned int Thread::getState() const {
    return _state;
}

/**
 * Return The id of this.
 * @return The id of this
 */
unsigned int Thread::getId() const {
    return _id;
}
