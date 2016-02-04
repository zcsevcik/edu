/*
 * btntrigger.c
 *
 * Author:         Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/03/05
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <pifacedigital.h>

/* ==================================================================== */
#define countof(x) \
    ((sizeof(x) / sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

/* ==================================================================== */
static int current_directory = -1;

static inline void wait_for_child_processes() {
    while (1) {
        int status = waitpid((pid_t)-1, NULL, WNOHANG);
        if (status == 0)
            break;
        if (status == -1 && errno == ECHILD)
            break;
    }
}

/* ==================================================================== */
enum button_state {
    not_pressed = 0,
    down = 1,
    up = 2,
    holding = 3,
};

typedef void (*handler_t)(int, enum button_state);

/* ******************************************************************** */
/* ******************************************************************** */

/* ==================================================================== */
static void default_button_handler(int i_btn, enum button_state state) {
    (void)i_btn;
    (void)state;
    return;
}

/* ==================================================================== */
static void shellcmd_button_handler(int i_btn, enum button_state state) {
    wait_for_child_processes();

    if (state != down && state != up)
        return;

    static struct {
        int fd[2];
        char filename[2][16];
    } exec[8];

    static int initialized = 0;
    if (!initialized) {
        initialized = 1;

        static const char *state_strings[] = {
            NULL,
            "down",
            "up",
            NULL,
        };

        for (int i = 0; i < 8; ++i) {
            snprintf(exec[i].filename[0], countof(exec[i].filename[0]),
                     "%d_%s.sh", i, state_strings[down]);
            snprintf(exec[i].filename[1], countof(exec[i].filename[1]),
                     "%d_%s.sh", i, state_strings[up]);

            /* BUG: if fd is script, O_CLOEXEC will fail with ENOENT */
            exec[i].fd[0] = openat(current_directory, exec[i].filename[0],
                                   O_RDONLY|O_NOFOLLOW /*|O_CLOEXEC*/ );
            exec[i].fd[1] = openat(current_directory, exec[i].filename[1],
                                   O_RDONLY|O_NOFOLLOW /*|O_CLOEXEC*/ );
        }
    }

    int exec_fd = exec[i_btn].fd[(int)state-1];
    char *const argv_0 = exec[i_btn].filename[(int)state-1];

    if (exec_fd == -1)
        return;

    pid_t childpid = fork();
    if (childpid == -1)
        return;
    else if (childpid == 0) {
        for (int fd = 3; ; ++fd) {
            if (fd == exec_fd) continue;
            if (close(fd) < 0 && errno == EBADF)
                break;
        }

        int fd_stdin = -1, fd_stdout = -1, fd_stderr = -1;

        dup2((fd_stdin = open("/dev/null", O_RDONLY)), STDIN_FILENO);
        dup2((fd_stdout = open("/dev/null", O_WRONLY)), STDOUT_FILENO);
        dup2((fd_stderr = open("/dev/null", O_WRONLY)), STDERR_FILENO);

        close(fd_stderr);
        close(fd_stdout);
        close(fd_stdin);

        char *const argv[] = {
            argv_0,
            NULL,
        };

        fexecve(exec_fd, argv, environ);
        abort();
    }

    return;
}

/* ==================================================================== */
static void set_button_handlers(handler_t h[8]) {
    h[0] = &shellcmd_button_handler;
    h[1] = &shellcmd_button_handler;
    h[2] = &shellcmd_button_handler;
    h[3] = &shellcmd_button_handler;
    h[4] = &shellcmd_button_handler;
    h[5] = &shellcmd_button_handler;
    h[6] = &shellcmd_button_handler;
    h[7] = &shellcmd_button_handler;
}

/* ******************************************************************** */
/* ******************************************************************** */

/* ==================================================================== */
struct s_opts {
    int daemonize;
    int sample_timeout; // 10~15 is optimal [msec]
    uint8_t hw_addr;
};

static struct s_opts opts = {
    .daemonize = 0,
    .sample_timeout = 15,
    .hw_addr = 0,
};

/* ==================================================================== */
static uint8_t samples[3] = { 0, 0, 0 };

static int samples_count() {
    return countof(samples);
}

__attribute((__optimize__("unroll-loops")))
static void samples_clear() {
    for (int i = 0; i < (int)countof(samples); ++i)
        samples[i] = 0;
}

__attribute((__optimize__("unroll-loops")))
static int samples_equals() {
    for (int i = 0; i < (int)countof(samples) - 1; ++i) {
        if (samples[i] != samples[i+1])
            return 0;
    }

    return 1;
}

__attribute((__optimize__("unroll-loops")))
static void samples_shift_add(uint8_t value) {
    int i;
    for (i = 0; i < (int)countof(samples) - 1; ++i) {
        samples[i] = samples[i+1];
    }
    samples[i] = value;
}

static inline uint8_t samples_get() {
    return samples[countof(samples) - 1];
}

/* ==================================================================== */
static int io_spifd = -1;
static volatile sig_atomic_t io_term = 0;

static int io_init() {
    if ((io_spifd = pifacedigital_open(opts.hw_addr)) < 0)
        return 0;
    if (pifacedigital_enable_interrupts() < 0) {
        pifacedigital_close(opts.hw_addr);
        return 0;
    }

    fcntl(io_spifd, F_SETFD, fcntl(io_spifd, F_GETFD) | FD_CLOEXEC);
    return 1;
}

static inline void io_close() {
    pifacedigital_disable_interrupts();
    pifacedigital_close(opts.hw_addr);
    io_spifd = -1;
}

static inline void io_abort() {
    io_term = 1;
}

static int io_wait_for_sample() {
    int first_time = 1;
    uint8_t inputs;

    samples_clear();
    for (int i = 0; i < (int)samples_count() || !samples_equals(); ++i) {
        int status = pifacedigital_wait_for_input(
                         &inputs,
                         (first_time ? -1 : opts.sample_timeout),
                         opts.hw_addr);
        if ((status < 0 && errno == EINTR) && io_term)
            return -2;
        else if (status < 0)
            return -1;

        samples_shift_add(inputs);
        if (!first_time && status > 0)
            i = 0;

        first_time = 0;
    }

    return samples_get();
}

/* ==================================================================== */
struct s_buttons {
    uint8_t previous_state;
    uint8_t state;

    handler_t handlers[8];
};

static struct s_buttons buttons = {
    .previous_state = 0,
    .state = 0,
    .handlers = {
        &default_button_handler, &default_button_handler,
        &default_button_handler, &default_button_handler,
        &default_button_handler, &default_button_handler,
        &default_button_handler, &default_button_handler,
    },
};

static inline int buttons_getstate(uint8_t i) {
    const uint8_t idx = (1 << i);
    return (!!(buttons.previous_state & idx) << 1)
         | (!!(buttons.state & idx) << 0);
}

__attribute((__optimize__("unroll-loops")))
static void buttons_event(uint8_t value) {
    buttons.previous_state = buttons.state;
    buttons.state = value;

    for (int i = 0; i < (int)countof(buttons.handlers); ++i) {
        if (buttons.handlers[i] != NULL)
            buttons.handlers[i]( i, (enum button_state)buttons_getstate(i) );
    }
}

/* ==================================================================== */
static int get_bin_dir() {
    int fd;
    char buf[PATH_MAX];

    ssize_t len = readlink("/proc/self/exe", buf, sizeof buf);
    if (len == -1) {
        return -1;
    }

    for (char *p = &buf[len - 1]; p >= buf; --p) {
        if (*p == '/') {
            *p = '\0';
            break;
        }
    }

    fd = open(buf, O_PATH | O_DIRECTORY | O_CLOEXEC);
    return fd;
}

/* ==================================================================== */
static int my_daemon() {
    pid_t pid, sid;
    int fd_stdin = -1, fd_stdout = -1, fd_stderr = -1;

    if ((pid = fork()) == -1) {
        return -1;
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    if ((sid = setsid()) == -1) {
        return -1;
    }

    if ((chdir("/")) == -1) {
        return -1;
    }

    dup2((fd_stdin = open("/dev/null", O_RDONLY)), STDIN_FILENO);
    dup2((fd_stdout = open("/dev/null", O_WRONLY)), STDOUT_FILENO);
    dup2((fd_stderr = open("/dev/null", O_WRONLY)), STDERR_FILENO);

    close(fd_stderr);
    close(fd_stdout);
    close(fd_stdin);

    return 0;
}

/* ==================================================================== */
static void signal_handler(int signal) {
    (void)signal;
    io_abort();
}

int main(int argc, char *argv[])
{
    opts.hw_addr = 0;
    if (argc >= 2 && !strcmp(argv[1], "-d"))
        opts.daemonize = 1;

    set_button_handlers(buttons.handlers);

    current_directory = get_bin_dir();
    if (current_directory == -1)
        return EXIT_FAILURE;

    if (opts.daemonize && my_daemon() < 0)
        return EXIT_FAILURE;

    signal(SIGINT, &signal_handler);
    signal(SIGTERM, &signal_handler);

    if (!io_init())
        return EXIT_FAILURE;

    int return_value = EXIT_SUCCESS;
    while (1) {
        int result = io_wait_for_sample();
        if (result == -2) break;
        if (result < 0) { return_value = EXIT_FAILURE; break; }

        // pullup resistor
        buttons_event(~(uint8_t)(result & 0xff));
    }

    io_close();
    close(current_directory); current_directory = -1;
    wait_for_child_processes();
    return return_value;
}
