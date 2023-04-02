/*
    Tristan Besser
    Consumer Process
    4/3/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>

int main() {

    // Allocated shared memory and semaphores.
    int shm_fd = shm_open("table", O_RDWR, 0666);
    int *table = static_cast<int*>(mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    sem_t *fill = sem_open("fill", O_CREAT, 0666, 0);
    sem_t *avail = sem_open("available", O_CREAT, 0666, 2);
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);

    int loop = 10;
    std::cout << "Consumer ready to recieve " << loop << " items." << std::endl;

    for (int i = 0; i < loop; ++i) {

        sem_wait(fill);

        int wait = rand() % 2 + 1;
        sleep(wait);

        sem_wait(mutex);
        --(*table);
        sem_post(mutex);

        std::cout << "Item consumed, there are now " << *table << " item(s) in the table." << std::endl;

        sem_post(avail);
    }

    // Close and unlink semaphores
    // Deallocate shared memory

    sem_close(fill);
    sem_close(avail);
    sem_close(mutex);

    sem_unlink("fill");
    sem_unlink("available");
    sem_unlink("mutex");

    munmap(table, sizeof(int));
    close(shm_fd);
    shm_unlink("table");

    std::cout << "Consumer cleaned up!" << std::endl;

}