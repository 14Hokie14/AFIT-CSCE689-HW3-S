#include "PCalc_T.h"
#include "PCalc.h"

#include <cmath>
#include <iostream>

/**
 * Constructor, make sure to call PCalc's constructor and pass in array_size. 
 * We also need to pass in num_threads to the Thread_Pool object here (the Thread_pool objec is t_pool)
 */
PCalc_T::PCalc_T(unsigned int array_size, unsigned int num_threads): PCalc(array_size), n_threads(num_threads){}





/**
 * This function implements the sieve of Eratosthenes algorithm. 
 */
void PCalc_T::markNonPrimes(){
    //std::cout << "array count: " << array_size() << std::endl;

    // This outter loop will make sure we iterate as far as we need to in the array.
    auto i = 2;
    for(; i < (unsigned int)ceil(sqrt(array_size()));){
        // This inner loop makes sure we only start up n_threads
        for(auto j = 0;j < n_threads;){
            if(at(i)){
                thread_vector.emplace_back([&](int current_i){
                    for(unsigned int k = current_i*2; k < array_size(); k += current_i){
                        at(k) = false; 
                    }  
                }, i);
                // Only increment j here so we only make at most n_threads
                j++;
            }
            // Increment i here for the outter loop
            i++; 
        }

        // Make sure we join all of the threads
        for(auto& t: thread_vector){
            t.join();
        }
        thread_vector.clear();
    }
}