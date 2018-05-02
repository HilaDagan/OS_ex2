// Thread.cpp

#include <cstdio>
#include "Thread.h"


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t Thread::translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}


/**
 * Initialize the new thread
 * @param id the given id.
 * @param f the entry point of this.
 */
Thread::Thread(int id, void (*f)(void)) {
    _id = id;
    _state = READY;
    _dependentIn = NOT_DEPENDENT;
    _quantumsNum = 0;
    _function = f;
    if (id != 0) { // not the main thread
        setupEnv();
    }
}

/**
 * Free all the allocated memory.
 */
Thread::~Thread() = default;


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
    _dependentIn = NOT_DEPENDENT;
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
const std::list<int> &Thread::getDependenciesList() const
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
    _dependenciesList.remove(tid);

}

/**
 * Set the thread id of a thread that this id is dependent of.
 */
void Thread::setDependentIn(int _dependentIn)
{
    Thread::_dependentIn = _dependentIn;
}

/**
 * @return the number of quantums that 'this' was in RUNNING state.
 */
int Thread::getQuantums() const {
    return _quantumsNum;
}


/**
 * Increase the number of quantums that 'this' was in RUNNING state.
 */
void Thread::increasQuantums(){
    this->_quantumsNum++;
}

/**
 * Setup the stack environment of the thread.
 */
void Thread::setupEnv() {
    _sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    if (_function != nullptr){
        _pc = (address_t)_function; //todo
    } else { //its the main thread.
        _pc = 0;
    }
    sigsetjmp(_env, 1);

    ((_env)->__jmpbuf)[JB_SP] = translate_address(_sp);
    ((_env)->__jmpbuf)[JB_PC] = translate_address(_pc);
    sigemptyset(&(_env)->__saved_mask); //todo
}
