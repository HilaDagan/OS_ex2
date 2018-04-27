// Thread.cpp

#include "Thread.h"

/**
 * Initialize the new thread
 * @param id the given id.
 * @param f the entry point of this.
 */
Thread::Thread(int id, void (*f)(void), unsigned int stackSize) {
    _id = id;
    _state = READY;
    _stack.reserve(stackSize); // increase the vector capacity.
    _dependentIn = nullptr;
    _function = f;
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
int Thread::getId() const {
    return _id;
}

/**
 * Reset the thread id of the thread that 'this' dependent on, to be null.
 */
void Thread::resetDependentIn()
{
    _dependentIn = nullptr;
}

/**
 * @return the thread id of a thread that this id is dependent of.
 */
int Thread::getDependentIn() const
{
    return _dependentIn;
}

/**
 * @return the thread id of all the threads that are depend on this thread.
 */
const std::vector<int> &Thread::getDependenciesList() const
{
    return _dependenciesList;
}


/**
* Add a new thread id to the dependency list.
*/
void Thread::addToDependenciesList(const int newId){
    _dependenciesList.push_back(newId);
}


/**
 * remove the given thread id from the dependencies list.
 */
void Thread::removeDependentThread(int tid){ //todo - will it really change the vector?
    _dependenciesList.erase(remove(_dependenciesList.begin(), _dependenciesList.end(), tid),
                            _dependenciesList.end());

}

/**
 * Set the thread id of a thread that this id is dependent of.
 */
void Thread::setDependentIn(int _dependentIn)
{
    Thread::_dependentIn = _dependentIn;
}
