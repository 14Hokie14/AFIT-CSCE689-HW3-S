#include <Thread_Pool.h>

#include <stdio.h>


/**
 * The constructor for the thread pool.  Takes in a size_t that 
 * represents the number of threads that will be created. 
 * 
 * @param threads number of 
 */
Thread_Pool::Thread_Pool(size_t threads): shutdown(false) {
    // Check to make sure the number of threads to spin up is greater than 1
    // to ensure we always spin up at least one worker
    if(threads <= 0){
        threads = 1; 
    }

    // Now loop once per thread, create the thread and put it in the 
    // worker_threads queue.  Each thread will be in an infinite loop that is checking to 
    // see if there is a job to be done on the job queue as long as shutdown is false and 
    // the job_queue is not empty. 
    for(auto i = 0; i < threads; i++){
        worker_threads.emplace_back(
            [&]{
                for (;;){
                    // Now check to see if shutdown is true and the queue is empty, if both
                    // of those conditions are not met then pop the first job off the queue
                    // and do it. 
                    std::function<void()> job;
                    {
                        // Using a unique lock so we don't have to worry about mutex logic, 
                        // Also have to use a unique lock because we are using a condition variable (shutdown). 
                        std::unique_lock<std::mutex> lock(this->job_mutex);
                        this->job_condition.wait(lock, [&] {return this->shutdown || !this->job_queue.empty(); });
                        if(this->shutdown && this->job_queue.empty()){
                            return;
                        }
                        job = std::move(this->job_queue.front());
                        this->job_queue.pop();
                        work_done.notify_all();
                    }

                    job();
                }
            }
        );
    }

}



/** 
 * Deconstructor, will handle calling join() for all of the created threads. 
 */
Thread_Pool::~Thread_Pool(){
    {
        // Make sure we grab the lock
        std::unique_lock<std::mutex> lock(job_mutex);
        // This is the only place in the code we update shutdown
        shutdown = true;
    }
    // Let all the threads know via the conditional variable
    job_condition.notify_all();
    // Call join() on each thread
    for(std::thread &worker: worker_threads){
        worker.join();
    }
}



