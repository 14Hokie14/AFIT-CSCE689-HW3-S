#include "PCalc_T.h"
#include "PCalc.h"

#include <cmath>
#include <iostream>

/**
 * Constructor, make sure to call PCalc's constructor and pass in array_size. 
 * We also need to pass in num_threads to the Thread_Pool object here (the Thread_pool objec is t_pool)
 */
PCalc_T::PCalc_T(unsigned int array_size, unsigned int num_threads): PCalc(array_size), n_threads(num_threads - 1), t_pool(n_threads){}

/**
 * This function implements the sieve of Eratosthenes algorithm. 
 */
void PCalc_T::markNonPrimes(){
    
    //std::cout << "array_size() returns: " << array_size() << std::endl;
    std::cout << "array count: " << array_size() << std::endl;
    for(unsigned int i = 2; i < (unsigned int)sqrt(array_size()); i++){
        if(at(i)){
            // We want to capture i by copy here, but everything else by reference,
            // specifically the primelist by reference!
            t_pool.enqueue_job([&](int current_i)
            {
                for(unsigned int j = current_i*2; j < array_size(); j += current_i){
                    at(j) = false; 
                }   
            }, i);
        }
    }

    // Start the shutdown now, but this halts the manager thread so we get a correct time display. 
    t_pool.startShutdown();
}