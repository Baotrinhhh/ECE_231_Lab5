#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/epoll.h>
#include "pin_config_lib.h"

#define SIZE 10 
#define PN 50
// Define global buffer array
struct data_type {
    struct timespec timestamp;
    pthread_t thread_id;
    };

struct data_type buffer[SIZE];

int put_idx = 0;
int get_idx = 0;
int cnt = 0;
int WriteCNT = 0;

pthread_mutex_t lock;

// Define input Thread
void* inputThread(void* var) {
    // Taking the argument
    char* input = (char*) var;
    FILE* fp = fopen(input, "r");

    if (fp == NULL) {
        perror("Failed to open interrupt file");
        pthread_exit(NULL);
    }
    int fd = fileno(fp);

    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("Failed to create epoll instance");
        fclose(fp);
        pthread_exit(NULL);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("Failed to add file descriptor to epoll");
        close(epfd);
        fclose(fp);
        pthread_exit(NULL);
    }

    struct epoll_event ev_wait;
    sleep(1);

    while (WriteCNT < PN){
        // Critical Section - Start 
        pthread_mutex_lock(&lock);

        while (cnt < SIZE) {
            int ret = epoll_wait(epfd, &ev_wait, 1, -1);
            // if there is not rising edge
            if (ret < 0) {
                perror("epoll_wait failed");
                continue;
            }
            
            // if capture rising edge
            if (ret > 0){
                // Reset file pointer if required to clear the interrupt:
                fseek(fp, 0, SEEK_SET);

                // Capture the current timestamp and thread id
                clock_gettime(CLOCK_MONOTONIC, &buffer[put_idx].timestamp);
                buffer[put_idx].thread_id = pthread_self();
                cnt ++;
                put_idx++;
            }

            if (put_idx == SIZE) {
                put_idx = 0;
                }
        }
        // Critical Section - End 
        pthread_mutex_unlock(&lock); 
    }
    close(epfd);
    fclose(fp);
}
   
// Define output thread
void* outputThread(void * var){
    // Taking the argument
    char* input = (char*) var;
    FILE* txt = fopen(input, "a");

    if (txt == NULL) {
        perror("Failed to open text file");
        pthread_exit(NULL);
    }
    while (WriteCNT < PN){
        // Critical Section - Start 
        pthread_mutex_lock(&lock);

        while (cnt > 0){
            // Write to text file
            fprintf(txt, "%ld\t%ld\t%lu\n", 
                buffer[get_idx].timestamp.tv_sec, 
                buffer[get_idx].timestamp.tv_nsec,
                (unsigned long)buffer[get_idx].thread_id);

            // Print to terminal
            printf("%ld\t%ld\t%lu\n", buffer[get_idx].timestamp.tv_sec, 
                buffer[get_idx].timestamp.tv_nsec,
                (unsigned long)buffer[get_idx].thread_id);
            cnt--;
            get_idx++;
            WriteCNT++;

            if (get_idx == SIZE) {
                get_idx = 0;
                }
        }

        // Critical Section - End 
        pthread_mutex_unlock(&lock); 
    }
    fclose(txt);
}


int main() {
    char *TEXT = "/home/debian/Lab5/Bao_Trinh_data.txt";
    remove(TEXT); // Delete if it exists for fresh start

    char gpio_pin_number[32] = "P8_09";
    int gpio_number = 69;

    // Configure GPIO input and interrupt on rising edge
    configure_interrupt(gpio_number, gpio_pin_number);

    // Build the file path for the GPIO value file
    char InterruptPath[40];
    sprintf(InterruptPath, "/sys/class/gpio/gpio%d/value", gpio_number);

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    
    pthread_t thread_id[2];
    // initialize lock
    pthread_create(&(thread_id[1]), NULL, inputThread, (void*)InterruptPath);
    pthread_create(&(thread_id[0]), NULL, outputThread, (void*)TEXT);
    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);
    pthread_mutex_destroy(&lock); //Uninitialize Lock
    pthread_exit(0);
}