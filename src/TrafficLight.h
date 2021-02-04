#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

enum TrafficLightPhase {red, green};

// forward declarations to avoid include cycle
class Vehicle;

// MessageQueue class used to send and receive messages under the lock
template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
    
};

class TrafficLight : TrafficObject
{
public:
    // constructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behavior methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messageQueue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif