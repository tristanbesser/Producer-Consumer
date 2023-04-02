/*
    Tristan Besser
    Producer Process
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
#include <errno.h>

int main() {

    int tbl = shm_open("table", O_CREAT | O_RDWR, 0666);
    if (tbl == -1) {
        std::cout << "shm_open error!" << std::endl;
        std::cout << errno << std::endl;
        exit(1);
    }



    if (ftruncate(tbl, sizeof(int)) == -1) {
        std::cout << "ftruncate error!" << std::endl;
        exit(1);
    }

    int *table = static_cast<int*>(mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tbl, 0));

    if (table == MAP_FAILED) {
        std::cout << "mmap error!" << std::endl;
        exit(1);
    }

    sem_t *fill = sem_open("fill", O_CREAT, 0666, 0);
    sem_t *avail = sem_open("available", O_CREAT, 0666, 2);
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);

    int loop = 10;
    std::cout << "table: " << *table << std::endl;
    std::cout << "Producer ready to create " << loop << " items." << std::endl;

    for (int i = 0; i < loop; ++i) {

        if (i > 0) sem_wait(avail);

        int wait = rand() % 2 + 1;
        sleep(wait);

        sem_wait(mutex);
        ++(*table);
        sem_post(mutex);

        std::cout << "Item produced, there are now " << *table << " item(s) in the table." << std::endl;

        sem_post(fill);
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
    close(tbl);
    shm_unlink("table");

    std::cout << "Producer cleaned up!" << std::endl;
}