#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> ulock(_mutex);
    _cond.wait(ulock,[this]{return !_message.empty();});
    T msg = std::move(_message.back());
    _message.pop_back();
    return msg;

}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    
    std::lock_guard<std::mutex> ulock(_mutex);
    //std::this_thread::sleep_for(std::chrono::microseconds(100));
   //std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
    _message.push_back(std::move(msg));
    _cond.notify_one();
        
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        auto tlstatus = queue->receive();
        if (tlstatus ==  TrafficLightPhase::green)
        return;
    }
    
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
     
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
     std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(4,6); // distribution in range [4, 6]
    double cycleDuration = dist6(rng)*1000;

    while (true)
    {
         // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
            _currentPhase = ( _currentPhase == TrafficLightPhase::red )?TrafficLightPhase::green:TrafficLightPhase::red;
            
            queue->send(std::move(_currentPhase));
            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
            
        }

        //end of simulation loop
    }
    
}

