#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<sys/time.h>
#include<sys/times.h>
#include<string.h>
#include<time.h>

using namespace std;

struct cosas_logger{
    thread thread_logger;
    mutex mutex_logger;
    condition_variable cv_logger;
    condition_variable cv_threads;
    bool logger_finalizado = true;
    bool resultados_listos = false;
    int thread_id;
    double thread_resultado;
}cosas_logger;

struct argumentos{
    int array_lenght;
    enum{SUM, SUB, XOR} operation;
    enum{s_thread, m_thread} mode;
    int num_threads;
}argumentos;

struct arrays{
    double *main_array;
    double *results_array;
    double *logger_array;
}arrays;


struct bloqueo_resto{
    mutex mutex_resto;
    bool resto_tomado;
}bloqueo_resto;


//Metodo donde vamos a llenar el array
void llenar_array();
//Metodo donde se realizara el procesado de datos
void procesado_de_datos();
//Metodo donde se realiza la suma
void suma(int inicio, int final, int id);
//Metodo donde sacamos el resultado de la suma
void resultados();
//Metodo donde realizaremos las funciones del logger
void funcion_logger();

int main(int argc, char const *argv[])
{
    //El primer argumentos son el numero de elementos del array
    argumentos.array_lenght = atoi(argv[1]);
    //El segundo argumento es el tipo de operacion
    if (strcmp(argv[2], "SUM") == 0)
        argumentos.operation = argumentos.SUM;
    else if (strcmp(argv[2], "SUB") == 0)
        argumentos.operation = argumentos.SUB;
    else if (strcmp(argv[2], "XOR") == 0)
        argumentos.operation = argumentos.XOR;
    else
    {
        printf("Operacion no valida\n");
        return 1;
    }

    if (argc == 3)
    {
        argumentos.mode = argumentos.s_thread;
        argumentos.num_threads = 1;
    }
    else if (argc = 5)
    {
        argumentos.mode = argumentos.m_thread;
        argumentos.num_threads = atoi(argv[4]);
    }
    else
    {
        printf("Numero de argumentos no validos\n");
        return 1;
    }

     

    //Reservamos el espacio en memoria para los arrays
    arrays.main_array = (double*)malloc(sizeof(double) * argumentos.array_lenght);
    arrays.results_array = (double*)malloc(sizeof(double) * argumentos.num_threads);
    arrays.logger_array = (double*)malloc(sizeof(double)* argumentos.num_threads);

    llenar_array();

    cosas_logger.thread_logger = thread(funcion_logger);

    procesado_de_datos();
    resultados();
    
    cosas_logger.thread_logger.join();
    return 0;
}

void llenar_array(){
    for (int i = 0; i < argumentos.array_lenght; i++)
        arrays.main_array[i] = (double)i;
}

void procesado_de_datos(){
    thread t[argumentos.num_threads];

    if (argumentos.mode == argumentos.s_thread)
    {
       if (argumentos.operation == argumentos.SUM)
       {
           t[0] = thread(suma, 0, argumentos.array_lenght, 0);
           t[0].join();
       }
    }  
    else
    {
        if (argumentos.operation == argumentos.SUM)
       {
           for (int i = 0; i < argumentos.num_threads; i++)
           {
               int inicio = i * (argumentos.array_lenght / argumentos.num_threads);
               int fin =  (i+1) * (argumentos.array_lenght / argumentos.num_threads);
               t[i] = thread(suma, inicio, fin, i);
           }
           for (int i = 0; i < argumentos.num_threads; i++)
           {
               t[i].join();
           }
       }
    }
}

void suma(int inicio, int final, int id){
    double resultado = 0;

    for (int i = inicio; i < final; i++)
        resultado += arrays.main_array[i];
    

    lock_guard<mutex> lock(bloqueo_resto.mutex_resto);
    {
        if (bloqueo_resto.resto_tomado == false && argumentos.array_lenght % argumentos.num_threads != 0)
        {
            bloqueo_resto.resto_tomado = true;
            for (int i = argumentos.num_threads * (argumentos.array_lenght / argumentos.num_threads); i < argumentos.array_lenght; i++)
            {
                resultado += arrays.main_array[i];
            }
        }
    }

    arrays.results_array[id] = resultado;
    printf("Resultado del thread %d: %f\n", id, resultado);

    unique_lock<mutex> ulk(cosas_logger.mutex_logger);
    cosas_logger.cv_threads.wait(ulk, []{return cosas_logger.logger_finalizado;});

    cosas_logger.thread_id = id;
    cosas_logger.thread_resultado = resultado;

    cosas_logger.resultados_listos = true;
    cosas_logger.logger_finalizado = false;

    ulk.unlock();
    cosas_logger.cv_logger.notify_all();
}

void resultados()
{
    double resultado = 0;
    for (int i = 0; i < argumentos.num_threads; i++)
    {
        resultado += arrays.results_array[i];
    }
    
    printf("El resultado es: %f\n", resultado);

    resultado = 0;
    for (int i = 0; i < argumentos.num_threads; i++)
    {
        resultado += arrays.logger_array[i];
    }

    printf("El resultado final del logger es: %f\n", resultado);
    
}

void funcion_logger(){
    for (int i = 0; i < argumentos.num_threads; i++)
    {
        unique_lock<mutex> ulk(cosas_logger.mutex_logger);
        cosas_logger.cv_logger.wait(ulk, []{return cosas_logger.resultados_listos;});

        arrays.logger_array[cosas_logger.thread_id] = cosas_logger.thread_resultado;

        cosas_logger.resultados_listos = false;    
        cosas_logger.logger_finalizado = true;

        printf("Soy el logger, el thread %d me ha pasado el resultado: %f\n", cosas_logger.thread_id, cosas_logger.thread_resultado);

        ulk.unlock();
        cosas_logger.cv_threads.notify_all();
    }
}