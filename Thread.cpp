

#include "Thread.h"


Thread::Thread(unsigned int id, void (*f)(void)) {
    _id = id;
//    _function = f;

}

Thread::~Thread() {

}

void Thread::setState(unsigned int newState) {

}

unsigned int Thread::getState() const {
    return 0;
}

unsigned int Thread::getId() const {
    return 0;
}
