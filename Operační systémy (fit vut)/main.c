/*
 * Soubor:  proj2.c
 * Datum:   2011/04/10
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Synchronizace vlaken, projekt c. 2 pro predmet IOS 
 */

// #define _XOPEN_SOURCE 600
// /usr/local/lib/gcc/i686-linux/4.4.5/include-fixed/sys/stat.h:379: error: inline function 'mknod' declared but never defined

#include <fcntl.h>
//#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// velikost pole na stacku
#define _countof(sarr) (sizeof(sarr) / sizeof(*sarr))

#define die(err) { perror(err); _exit(EXIT_FAILURE); }
#define die_clean(err) { perror(err); exitcode = EXIT_FAILURE; goto clean; }

typedef long action_id;
typedef long customer_id;

typedef struct {
    // zidli v cekarne
    unsigned long Q;

    // interval spawnu zakazniku
    unsigned long GenC;

    // doba obsluhy zakaznika
    unsigned long GenB;

    // pocet zakazniku
    unsigned long N;

    // textovy vystup
    char* F;
} params_st;

typedef struct {
    action_id aid;
    unsigned long cust_served;
    sem_t mutex_action;
    sem_t mutex_waiting_room;
    sem_t mutex_serving;
    sem_t mutex_served;
} shared_st;

typedef enum {
    bact_checks,
    bact_ready,
    bact_finished,
} barber_action;

typedef enum {
    cact_created,
    cact_enters,
    cact_ready,
    cact_served,
    cact_refused,
} customer_action;

#define FMT_BARBER_ACTION_CHECKS "%ld: barber: checks\n"
#define FMT_BARBER_ACTION_READY "%ld: barber: ready\n"
#define FMT_BARBER_ACTION_FINISHED "%ld: barber: finished\n"
#define FMT_CUSTOMER_ACTION_CREATED "%ld: customer %ld: created\n"
#define FMT_CUSTOMER_ACTION_ENTERS "%ld: customer %ld: enters\n"
#define FMT_CUSTOMER_ACTION_READY "%ld: customer %ld: ready\n"
#define FMT_CUSTOMER_ACTION_SERVED "%ld: customer %ld: served\n"
#define FMT_CUSTOMER_ACTION_REFUSED "%ld: customer %ld: refused\n"

int fp = -1;
int shmfd;
shared_st* shm;
params_st params;

void output_barber(barber_action bact)
{
    char buf[80];
    action_id aid;
    int len;
    int hr;

    hr = sem_wait(&shm->mutex_action);
    if (hr == -1) die("sem_wait()");
    aid = ++shm->aid;

    switch (bact) {
        case bact_checks:
            len = snprintf(buf, _countof(buf), FMT_BARBER_ACTION_CHECKS, aid);
            break;

        case bact_ready:
            len = snprintf(buf, _countof(buf), FMT_BARBER_ACTION_READY, aid);
            break;

        case bact_finished:
            len = snprintf(buf, _countof(buf), FMT_BARBER_ACTION_FINISHED, aid);
            break;

        default:
            assert(false);
    }

    write(fp, buf, len);

    hr = sem_post(&shm->mutex_action);
    if (hr == -1) die("sem_post()");
}

void output_customer(customer_id cid, customer_action cact)
{
    char buf[80];
    action_id aid;
    int len;
    int hr;

    hr = sem_wait(&shm->mutex_action);
    if (hr == -1) die("sem_wait()");
    aid = ++shm->aid;

    switch (cact) {
        case cact_created:
            len = snprintf(buf, _countof(buf), FMT_CUSTOMER_ACTION_CREATED, aid, cid);
            break;

        case cact_enters:
            len = snprintf(buf, _countof(buf), FMT_CUSTOMER_ACTION_ENTERS, aid, cid);
            break;

        case cact_ready:
            len = snprintf(buf, _countof(buf), FMT_CUSTOMER_ACTION_READY, aid, cid);
            break;

        case cact_served:
            len = snprintf(buf, _countof(buf), FMT_CUSTOMER_ACTION_SERVED, aid, cid);
            break;

        case cact_refused:
            len = snprintf(buf, _countof(buf), FMT_CUSTOMER_ACTION_REFUSED, aid, cid);
            break;

        default:
            assert(false);
    }

    write(fp, buf, len);

    hr = sem_post(&shm->mutex_action);
    if (hr == -1) die("sem_post()");
}

bool parseParams(params_st* params, int argc, char** argv)
{
    assert(params != NULL);
    assert(argc == 6);
    assert(argv != NULL);

    int index = 1;

    params->Q = strtoul(argv[index++], NULL, 10);
    if (errno == ERANGE) {
        perror("Q");
        return false;
    }

    params->GenC = strtoul(argv[index++], NULL, 10);
    if (errno == ERANGE) {
        perror("GenC");
        return false;
    }

    params->GenB = strtoul(argv[index++], NULL, 10);
    if (errno == ERANGE) {
        perror("GenB");
        return false;
    }

    params->N = strtoul(argv[index++], NULL, 10);
    if (errno == ERANGE) {
        perror("N");
        return false;
    }

    params->F = argv[index++];

    if (params->Q == 0) { return false; }
    if (params->GenB == 0) { params->GenB = 1; }
    if (params->GenC == 0) { params->GenC = 1; }

    return true;
}

void do_barber(unsigned long GenB)
{
    int hr;
    int free_seats;
    unsigned int time_sleep;

    while (shm->cust_served < params.N) {
        // podiva se do cekarny
        output_barber(bact_checks);
        hr = sem_getvalue(&shm->mutex_waiting_room, &free_seats);
        assert(free_seats >= 0);
        if (hr == -1) die("sem_getvalue()");
        bool have_customer = (unsigned int)free_seats < params.Q;

        if (have_customer) {
            // je pripraven prijmout zakanika
            output_barber(bact_ready);
            hr = sem_post(&shm->mutex_serving);
            if (hr == -1) die("sem_post()");
        }

        // striha zakaznika nebo spi
        time_sleep = rand() % GenB;
        hr = usleep(time_sleep);
        if (hr == -1) die("usleep()");
        
        if (have_customer) {    
            // zakaznik je ostrihan
            output_barber(bact_finished);
            hr = sem_post(&shm->mutex_served);
            if (hr == -1) die("sem_post()");
        }

        // necha zakaznika odejit
        time_sleep = 1;
        hr = usleep(time_sleep);
        if (hr == -1) die("usleep()");
    }

    _exit(EXIT_SUCCESS);
}

void do_customer(customer_id cid)
{    
    output_customer(cid, cact_created);
    output_customer(cid, cact_enters);

    int hr = sem_trywait(&shm->mutex_waiting_room);
    if (hr == -1) {
        if (errno == EAGAIN) {
            // plna cekarna
            output_customer(cid, cact_refused);
            ++shm->cust_served;
        }
        else die("sem_trywait()");
    }
    else {
        // ceka az bude holic pripraven
        hr = sem_wait(&shm->mutex_serving);
        if (hr == -1) die("sem_wait()");
        output_customer(cid, cact_ready);

        // uvolni misto v cekarne
        hr = sem_post(&shm->mutex_waiting_room);
        if (hr == -1) die("sem_post()");

        // ceka na ostrihani
        hr = sem_wait(&shm->mutex_served);
        if (hr == -1) die("sem_wait()");
        output_customer(cid, cact_served);

        ++shm->cust_served;
    }

    _exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    int exitcode = EXIT_SUCCESS;

    // zpracovani parametru
    if (argc != 6) {
        fprintf(stderr, "Spatny pocet parametru!\n");
        return EXIT_FAILURE;
    }
    if (!parseParams(&params, argc, argv)) {
        return EXIT_FAILURE;
    }

    // vytvoreni sdilene pameti
    //shmfd = shm_open(SHARED_MEMORY_KEY, O_CREAT | O_RDWR, 0666);
    //if (shmfd == -1) die_clean("shm_open()");

    //void* shmraw = mmap(0, sizeof(shared_st), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    //if ((void *)-1 == shmraw) die_clean("mmap()");
    
    //shm = (shared_st *)shmraw;
    //shm->aid = 0;

    key_t shmkey = ftok(argv[0], 1234);
    if (shmkey == -1) die_clean("ftok()");

    shmfd = shmget(shmkey, sizeof(shared_st), IPC_CREAT | IPC_EXCL | 0666);
    if (shmfd == -1) die_clean("shmget()");

    void* shmraw = shmat(shmfd, NULL, 0);
    if ((void *)-1 == shmraw) die_clean("shmat()");

    shm = (shared_st *)shmraw;
    shm->aid = 0;
    shm->cust_served = 0;

    // nastaveni semaforu
    if (-1 == sem_init(&shm->mutex_action, 1, 1)) die_clean("sem_init()")
    if (-1 == sem_init(&shm->mutex_waiting_room, 1, params.Q)) die_clean("sem_init()")
    if (-1 == sem_init(&shm->mutex_serving, 1, 0)) die_clean("sem_init()")
    if (-1 == sem_init(&shm->mutex_served, 1, 0)) die_clean("sem_init()")

    if (strcmp(params.F, "-") == 0) {
        fp = fileno(stdout);
    }
    else {
        fp = open(params.F, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC | O_APPEND, 0666);
        if (fp == -1) die_clean("open()");
    }

    // vytvoreni podprocesu holice
    pid_t pid_barber = fork();
    if (pid_barber == 0) {
        do_barber(params.GenB);
    }
    else if (pid_barber == -1) { 
        die_clean("fork()");
    }

    // vytvoreni podprocesu zakazniku
    for (unsigned long i = 1; i <= params.N; ++i) {
        pid_t pid_customer = fork();
        if (pid_customer == 0) {
            do_customer((customer_id)i);
        }
        else if (pid_customer == -1) {
            die_clean("fork()"); 
        }

        unsigned int time_sleep = rand() % params.GenC;
        int hr = usleep(time_sleep);
        if (hr == -1) die_clean("usleep()");
    }

    // cekani na dokonceni vsech podprocesu
    int wait_status;
    pid_t wait_pid = waitpid(pid_barber, &wait_status, 0);
    //pid_t wait_pid = wait(&wait_status);
    if (wait_pid == -1) {
        die_clean("wait()");
    }
    //else {
    //    if (!WIFEXITED(wait_pid) || WEXITSTATUS(wait_pid) != 0) {
    //        fprintf(stderr, "process %d fail: %d\n", wait_pid, WEXITSTATUS(wait_pid));
            //return EXIT_FAILURE;
    //    }
    //}

    //printf("pokracovani parent, wait return %d\n", wait_status);

    // uvolneni zdroju
clean:

    if (-1 == sem_destroy(&shm->mutex_served)) {
        perror("sem_destroy()");
    }
    if (-1 == sem_destroy(&shm->mutex_serving)) {
        perror("sem_destroy()");
    }
    if (-1 == sem_destroy(&shm->mutex_waiting_room)) {
        perror("sem_destroy()");
    }
    if (-1 == sem_destroy(&shm->mutex_action)) {
        perror("sem_destroy()");
    }
    if (-1 == shmctl(shmfd, IPC_RMID, NULL)) {
        perror("shmctl()");
    }
    //if (-1 == munmap(shm, sizeof(shared_st))) {
    //    perror("munmap()");
    //}
    //if (-1 == shm_unlink(SHARED_MEMORY_KEY)) {
    //    perror("shm_unlink()");
    //}

    if (fp != fileno(stdout)) {
        if (-1 == close(fp)) {
            perror("close()");
        }
    }

    return exitcode;
}

