#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <condition_variable>
#include <thread>
#include <functional>
#include <future>


/**
 * Simple thread pool implementation based heavily off of the following sources,
 * specifically the progschj ThreadPool implementation:
 * https://vorbrodt.blog/2019/02/12/simple-thread-pool/
 * https://github.com/progschj/ThreadPool
 * 
 * I also was having trouble with the manager thread executing two quickly, so I 
 * got the idea of using two condition variables from the following SO post
 * https://stackoverflow.com/questions/48042525/thread-pool-on-a-queue-in-c
 * 
 * The pool will take an int that will allow for a dynamic number of threads
 * in the pool.  Compatable with std:c++11.
 * 
 * By: Joshua h. White
 * Date: 7 Feb 2020
 * 
 * 
 * Future work: Create a function that returns a value for a job using future()
 */

class Thread_Pool {
    public: 
        // Constructor:
        Thread_Pool(size_t threads);

        // Destructor:
        ~Thread_Pool();

        /**
         * This method pushes a lambda into the job queue when you do 
         * not expect a return from the job. 
         * 
         * I also include the code in the .h because I'm using templateing
         * 
         */
        template<typename F, typename... Args>
        auto enqueue_job(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {

            // Set up the return type to pass into the the make shared call
            using return_type = typename std::result_of<F(Args...)>::type;

            // Make the job
            auto job = std::make_shared<std::packaged_task<return_type()>> (
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

            // Future call to capture the return value
            std::future<return_type> res = job->get_future();

            {
                // Lock the mutex
                std::unique_lock<std::mutex> lock(job_mutex);

                // Don't want to add things to the queue after we have started to shutdown the
                // threadpool
                if(shutdown){
                    perror("Tried to enqueue on a Thread_Pool that was shutting down.");
                }

                // Push the passed in lambda onto the job queue. 
                job_queue.emplace([job](){(*job)();});

                // Lets one, and only one, worker thread that there is a job that is ready to be done
                job_condition.notify_one();

                // Now we want to wait here if the job queue is not empty so my manager thread doesn't jump to far ahead
                work_done.wait(lock, [&] {return job_queue.empty();} );
            }

            


            // Make sure we actually return the value
            return res;
        }

        /**
         * I was having trouble getting an accurate readout on the time it took for my prime number
         * calculator to finish, so set up a method to change shutdown to true.
         */
        void startShutdown(){shutdown = true;}

    
    private:
        // This vector will hold all of the threads I create
        std::vector<std::thread> worker_threads;

        // This queue will hold all of the jobs the threads will perform, 
        // these jobs will be passed in as lambdas.
        std::queue<std::function<void()>> job_queue;

        // Mutex for the job queue
        std::mutex job_mutex;

        // A condition variable used to block the calling thread until notified 
        // to resume, used on the job_mutex
        std::condition_variable job_condition;

        // A condition variable used to block the manger thread until the queue 
        // is empty
        std::condition_variable work_done; 

        // This boolean is used to tell the thread pool to shut down.  It is set 
        // to false in the constructor. 
        bool shutdown;


};


#endif