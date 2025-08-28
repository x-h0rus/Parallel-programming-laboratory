#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<sys/times.h>
#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>


using namespace std;

//Definimos las variables del preprocesador
#define FEATURE_LOGGER 0
#define DEBUG 0

//time
double exec_time = 0;

//module operation mutex
mutex end_first_mutex;
bool end_first = false;

//logger
struct logger{
    mutex logger_mutex;
    condition_variable logger_cv;
    condition_variable thread_cv;
    bool result_ready = false;
    bool logger_finished = true;
    double thread_result = 0.0;
    double logger_result = 0.0;
    int thread_id = 0;
}logger;

//main
struct lock_main
{
    mutex main_mutex;
    condition_variable main_cv;
    bool logger_finish = false;
};


//arrays struct
struct arrays
{
    double *main_array;
    double *threads_array;
    double *logger_array;
} arrays;

//arguments struct 
struct options
{
    int size;
    enum{SUM, SUB, XOR} operation;
    int n_threads;
    enum{sThread, mThread} mode;
} arguments;

//Method in which we fill the array
void setup_data();
//Method of creating threads and data processing
void process_data();
void ADD(int start, int end, int id);
void SUB(int start, int end, int id);
void XOR(int start, int end, int id);
double results();
void logger_function(lock_main *lock_main);

int main(int argc, char const *argv[])
{
    //error handling
    if (argc == 1 || argc == 2)
    {
        cout << "Insert at least 3 arguments" << endl;
        cout << "./[executableName] [arraySize] [operationType]" << endl;
        return 1;
    }
    //We define the size of the main array
    arguments.size = atoi(argv[1]);
    //we reserve the necessary memory size
    arrays.main_array = (double *)malloc(sizeof(double) * arguments.size);
    if(DEBUG)
        cout << "Number of elements of the main array: " << arguments.size << endl;

    //We define the mathematical operation to perform
    if (strcmp(argv[2], "SUM") == 0)
    {
        arguments.operation = arguments.SUM;
        if (DEBUG)
            cout << "You have chosen to make an add" << endl;
    }
    else if (strcmp(argv[2], "SUB") == 0)
    {
        arguments.operation = arguments.SUB;
        if (DEBUG)
            cout << "You have chosen to make a sub" << endl;
    }
    else if (strcmp(argv[2], "XOR") == 0)
    {
        arguments.operation = arguments.XOR;
        if (DEBUG)
            cout << "You have chosen to make a xor" << endl;
    } 
    else //error handling
    {
        cout << "Operation not supported" << endl;
        cout << "Operations supported: SUM, SUB, XOR" << endl;
        return 1;
    }

    //We define number of threads and mode
    if (argc == 3)
    {
        arguments.mode = arguments.sThread;
        //We define the size of the threads array
        arguments.n_threads = 1;
        //we reserve the necessary memory size
        arrays.threads_array = (double *)malloc(sizeof(double) * arguments.n_threads);
        if (DEBUG)
        {
            cout << "Single thread mode" << endl;
            cout << "----------------------------------------------" << endl;
        }
    }
    else
    {
        if (strcmp(argv[3], "--multi-thread") == 0){
            arguments.mode = arguments.mThread;
            if (DEBUG)
                cout << "Multi thread mode" << endl;
        }
        else
        {
            cout << "Operation mode not supported" << endl;
            cout << "Mode supported '--multi-thread'" << endl;
            return 1;
        }
        //If we do not specify the number of threads, it gives an error
        if(argc == 4)
        {
            cout << "You must specify the number of threads to create" << endl;
            return 1;
        }
        //If we specify more threads than possible it gives an error
        else if (atoi(argv[4]) > 8)
        {
            cout << "Number of threads not supported" << endl;
            cout << "Threads supported [1-8]" << endl;
            return 1;
        }
        else
        {
            //We define the size of the threads array
            arguments.n_threads = atoi(argv[4]);
            //we reserve the necessary memory size
            arrays.threads_array = (double *)malloc(sizeof(double) * arguments.n_threads);
            
            if(DEBUG)
            {
                cout << "Threads number: " << arguments.n_threads << endl;
                cout << "----------------------------------------------" << endl;
            }
        }
    }

    //structure created so that the conditional variable is local
    lock_main lock_main;
    //Declared logger thread
    thread loggerThread;

    //We create the logger thread if it is activated
    if (FEATURE_LOGGER)
    {
        //we reserve the necessary memory size
        arrays.logger_array = (double *)malloc(sizeof(double) * arguments.n_threads);
        //We create the logger thread
        loggerThread = thread(logger_function, &lock_main);
        if (DEBUG)
        {
            printf("I'M LOGGER\n");
        }
    }
    //Method in which we fill the array
    setup_data();
    //Method of creating threads and data processing
    process_data();

    if (FEATURE_LOGGER)
    {
        //we wait until the logger end
        unique_lock<mutex> ulk(lock_main.main_mutex);
        lock_main.main_cv.wait(ulk, [&]{return lock_main.logger_finish;});
    }

    if (FEATURE_LOGGER)
    {
        loggerThread.join();
    }
 
    double finalMainResult = results();
    
  
    printf("Final main result: %f\n", finalMainResult);

    printf("Computing time: %f ms \n",exec_time);

    //free memory
    free(arrays.main_array);
    free(arrays.threads_array);
    free(arrays.logger_array);

    if (FEATURE_LOGGER)
    {
        if (finalMainResult == logger.logger_result)
        {
            exit(0);
        }
        else
        {   
            exit(1);
        }
    }    
    exit(0);
}

void setup_data(){
    for (int i = 0; i < arguments.size; ++i)
        arrays.main_array[i] = (double)i;
}

void process_data(){
    //Create an array of threads
    thread t[arguments.n_threads];

    struct timeval t_0, t_1;
	gettimeofday(&t_0, NULL);

    if(arguments.mode == arguments.sThread)
    {
        if (arguments.operation == arguments.SUM)
        {
            t[0] = thread(ADD, 0, arguments.size, 0);
            t[0].join();
        }
        else if (arguments.operation == arguments.SUB)
        {
            t[0] = thread(SUB, 0, arguments.size, 0);
            t[0].join();
        }
        else if (arguments.operation == arguments.XOR)
        {
            t[0] = thread(XOR, 0, arguments.size, 0);
            t[0].join();
        }
    }
    else
    {
        if (arguments.operation == arguments.SUM)
        {
           for (int i = 0; i < arguments.n_threads; ++i)
            {
                int start = i * (arguments.size / arguments.n_threads);
                int end = (i+1) * (arguments.size / arguments.n_threads);
                t[i] = thread(ADD, start, end, i);
            }
             for (int i = 0; i < arguments.n_threads; ++i)
            {
                t[i].join();
            }
        }
        else if (arguments.operation == arguments.SUB)
        {
             for (int i = 0; i < arguments.n_threads; ++i)
            {
                int start = i * (arguments.size / arguments.n_threads);
                int end = (i+1) * (arguments.size / arguments.n_threads);
                t[i] = thread(SUB, start, end , i);
            }
             for (int i = 0; i < arguments.n_threads; ++i)
            {
                t[i].join();
            }
        }
        else //XOR operation
        {
            for (int i = 0; i < arguments.n_threads; ++i)
            {
                int start = i * (arguments.size / arguments.n_threads);
                int end = (i+1) * (arguments.size / arguments.n_threads);
                t[i] = thread(XOR, start, end, i);
            }
             for (int i = 0; i < arguments.n_threads; ++i)
            {
                t[i].join();
            }
        }
    }

    gettimeofday(&t_1, NULL);
	exec_time = ((t_1.tv_sec - t_0.tv_sec)*1000 + (t_1.tv_usec - t_0.tv_usec)/1000.0);

}

void ADD(int start, int end, int id)
{
    double add = 0;

    for (int i = start; i < end; ++i)
        add += arrays.main_array[i];
    arrays.threads_array[id] = add;
    
    {
    lock_guard<mutex> lock(end_first_mutex);
        if (arguments.size % arguments.n_threads != 0 && end_first == false)
        {
            end_first = true; 
            int lastEnd = arguments.n_threads * (arguments.size / arguments.n_threads);   
            for (int i = lastEnd; i < arguments.size; ++i)
                add += arrays.main_array[i];
            arrays.threads_array[id] = add;
        }
    }

    if (FEATURE_LOGGER)
    {
        //block logger_mutex
        unique_lock<mutex> ulk(logger.logger_mutex);
        //if result_ready is true, we can go on, else we sleep
        logger.thread_cv.wait(ulk, []{return logger.logger_finished;});

        logger.thread_result = add;
        logger.thread_id = id;

        logger.result_ready = true;
        logger.logger_finished = false;
        ulk.unlock();
        logger.logger_cv.notify_all();
    }
}

void SUB(int start, int end, int id)
{
    double sub = 0;

    for (int i = start; i < end; ++i)
        sub -= arrays.main_array[i];
    arrays.threads_array[id] = sub;
    
    {
    lock_guard<mutex> lock(end_first_mutex);
        if (arguments.size % arguments.n_threads != 0 && end_first == false)
        {
            end_first = true;
            int lastEnd = arguments.n_threads * (arguments.size / arguments.n_threads);
            for (int i = lastEnd; i < arguments.size; ++i)
                sub -= arrays.main_array[i];
            arrays.threads_array[id] = sub;
        }
    }

    if (FEATURE_LOGGER)
    {
        //block logger_mutex
        unique_lock<mutex> ulk(logger.logger_mutex);
        //if result_ready is true, we can go on, else we sleep
        logger.thread_cv.wait(ulk, []{return logger.logger_finished;});

        logger.thread_result = sub;
        logger.thread_id = id;

        logger.result_ready = true;
        logger.logger_finished = false;
        ulk.unlock();
        logger.logger_cv.notify_all();
    }
}

void XOR(int start, int end, int id)
{
    double result = 0;
    //If size%n_threads is 0, only do this for
    for (int i = start; i < end; ++i)
        result = (long)result ^ (long)arrays.main_array[i];
    arrays.threads_array[id] = result;
    
    {
    lock_guard<mutex> lock(end_first_mutex);
        if (arguments.size % arguments.n_threads != 0 && end_first == false)
        {
            end_first = true;
            int lastEnd = arguments.n_threads * (arguments.size / arguments.n_threads);
            for (int i = lastEnd; i < arguments.size; ++i)
                result = (long)result ^ (long)arrays.main_array[i];
            arrays.threads_array[id] = result;
        }
    }

    if (FEATURE_LOGGER)
    {
        //block logger_mutex
        unique_lock<mutex> ulk(logger.logger_mutex);
        //if result_ready is true, we can go on, else we sleep
        logger.thread_cv.wait(ulk, []{return logger.logger_finished;});

        logger.thread_result = result;
        logger.thread_id = id;

        logger.result_ready = true;
        logger.logger_finished = false;
        ulk.unlock();
        logger.logger_cv.notify_all();
    }
}

double results()
{
    double result = 0;

    printf("[");
    for (int i = 0; i < arguments.n_threads - 1; ++i)
       printf("%f, ", arrays.threads_array[i]);
    printf("%f", arrays.threads_array[arguments.n_threads - 1]);
    printf("]\n");

    if (arguments.operation == arguments.SUM || arguments.operation == arguments.SUB)
    {
        for (int i = 0; i < arguments.n_threads; ++i)
            result += arrays.threads_array[i];
    }
    else
    {
        for (int i = 0; i < arguments.n_threads; ++i)
            result = (long)result ^ (long)arrays.threads_array[i];
    }
    return result;
}

void logger_function(lock_main *lock_main){
    for (int i = 0; i < arguments.n_threads; ++i)
    {
        //block logger_mutex
        unique_lock<mutex> ulk(logger.logger_mutex);
        //if result_ready is true, we can go on, else we sleep
        logger.logger_cv.wait(ulk, []{return logger.result_ready;});
        //we save in the order the threads were created
        arrays.logger_array[logger.thread_id] = logger.thread_result;

        if (arguments.operation == arguments.SUM || arguments.operation == arguments.SUB)
        {
            logger.logger_result += logger.thread_result;
            if (DEBUG)
            {
                printf("Pos: %d, value: %f\n",logger.thread_id, logger.thread_result);
                printf("Partial result: %f\n",logger.logger_result);
            }      
        }      
        else
        {
            logger.logger_result = (long)logger.logger_result ^ (long)logger.thread_result;
            if (DEBUG)
            {
                printf("Pos: %d, value: %f\n",logger.thread_id, logger.thread_result);
                printf("Partial result: %f\n",logger.logger_result);
            } 
        }
        //change the condition_variable
        logger.logger_finished = true;
        logger.result_ready = false;
        //free mutex
        ulk.unlock();
        //notify other threads
        logger.thread_cv.notify_all();
    }

    printf("[");
    for (int i = 0; i < arguments.n_threads - 1; ++i)
       printf("%f, ", arrays.logger_array[i]);
    printf("%f", arrays.logger_array[arguments.n_threads - 1]);
    printf("]\n");
    
    printf("Final logger result: %f\n",logger.logger_result);
    printf("---------------------------------------------------------\n");        

   //nottify and wake up the main 
   lock_main->logger_finish = true;
   lock_main->main_cv.notify_all();
}