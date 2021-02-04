#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // receive a message of type T whenever one is available in the MessageQueue, and return it

    std::unique_lock<std::mutex> ulock(_mutex);
    _cond.wait(ulock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // send a message of type T by pushing it to the MessageQueue
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // block execution until a msg TrafficLightPhase::green arrives in the MessageQueue

    while (true) {
        if (_messageQueue.receive() == TrafficLightPhase::green) { 
            break; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // start cycling through red / green traffic light phases in a thread, and add
    // this thread to the base class TrafficObject threads container
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
    
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // Toggles the current phase of the traffic light between red and green and sends an update  
    // to the message queue. The cycle duration is a random value between 4 and 6 seconds. 

    // random number between 4 and 6 seconds
    double duration = 0.0;
    double start_time = 0.0;
    while (true) {
        if ((clock() - start_time) / CLOCKS_PER_SEC >= duration) {
            // toggle phase, send update, new random stoplight duration, and restart timer
            if (getCurrentPhase() == TrafficLightPhase::red)  {
                _currentPhase = TrafficLightPhase::green; 
            }
            else {
                _currentPhase = TrafficLightPhase::red;
            }
            _messageQueue.send(std::move(_currentPhase));
            duration = 4 + 2 * ((double) rand()) / (double) RAND_MAX;
            start_time = clock();
        }

        // avoid overloading processor
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }
}

