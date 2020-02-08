#include "PCalc_SP.h"
#include "PCalc.h"
#include <cmath>
#include <iostream>

/**
 * Constructor, make sure to call PCalc's constructor and pass in array_size. 
 */
PCalc_SP::PCalc_SP(unsigned int array_size):PCalc(array_size) {}

/**
 * This function implements the sieve of Eratosthenes algorithm. 
 */
void PCalc_SP::markNonPrimes(){
    //std::cout << "array_size() returns: " << array_size() << std::endl;
    for(unsigned int i = 2; i < (unsigned int)ceil(sqrt(array_size())); i++){
        if(at(i)){
            for(unsigned int j = i*2; j < array_size(); j += i){
                at(j) = 0; 
            }
        }
    }
}