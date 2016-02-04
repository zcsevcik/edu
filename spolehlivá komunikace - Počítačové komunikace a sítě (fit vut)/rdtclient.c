/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include "log.h"
#include "sock.h"
#include "udt.h"
#include "rdt.h"

#define ACT_PROCESS 0
#define ACT_HELP 1

#define BUF_MAXLEN 4096

typedef struct {
    int verbose_lvl;
    char* src_port;
    char* dst_port;
    uint8_t action;
} params_t;

static volatile bool _terminate = false;

#ifdef _WIN32
static BOOL on_console_handler(unsigned long signal)
{
    switch (signal) {
        case CTRL_CLOSE_EVENT:
        case CTRL_C_EVENT:
            _terminate = true;
            return true;

        default:
            return false;
    }
}
#else
static void on_signal(int signal)
{
    (void)signal;
    _terminate = true;
}
#endif

bool parseargs(int argc, char** argv, params_t* params)
{
    assert(argc >= 0);
    assert(argv != NULL);
    assert(params != NULL);

    params->verbose_lvl = 1;

    int opt;
    int getopt_optind = 1;
    struct option getopt_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "verbose", no_argument, &params->verbose_lvl, 2 },
        { "debug", no_argument, &params->verbose_lvl, 3 },
        { "src", required_argument, NULL, 's' },
        { "dst", required_argument, NULL, 'd' },
        { 0, 0, 0, 0 }
    };

    opterr = 1;

    while ((opt = getopt_long(argc, argv, "hs:d:",
            getopt_opts, &getopt_optind)) != -1)
    {
        switch (opt) {
            case 0:
                continue;

            default:
                return false;

            case 'h':
                params->action = ACT_HELP;
                return true;

            case 'd':
                params->dst_port = optarg;
                break;

            case 's':
                params->src_port = optarg;
                break;
        }
    }

    if (!params->src_port) {
        return false;
    }
    if (!params->dst_port) {
        return false;
    }

    params->action = ACT_PROCESS;
    return true;
}

void cleanup()
{
    ipk_sock_free();
}

void mylog(int level, const char* msg)
{
    // level = 1 error
    // level = 2 verbose
    // level = 3 debug

    if (level == 1) {
        fprintf(stderr, "%s\n", msg);
    }
    else {
        fprintf(stderr, "-- %s\n", msg);
    }
}

bool process(params_t* args)
{
    ipk_sock_t udt = IPK_SOCK_INVALID;
    rdt_t* rdt = NULL;

#ifndef _WIN32
    if (!ipk_fd_block_mode(STDIN_FILENO, false)) {
        return false;
    }
#endif

    udt = udt_init(args->src_port);
    if (!udt_connect(udt, args->dst_port)) {
        goto return_false;
    }

    rdt = rdt_new();
    if (!rdt) {
        goto return_false;
    }

    if (!rdt_client_start(rdt)) {
        goto return_false;
    }

    bool eof = false;
    char buf[BUF_MAXLEN];
    char rcvbuf[UDT_PACKET_MAXLEN];
    char sndbuf[UDT_PACKET_MAXLEN];
    size_t nbuf = 0;
    size_t nrcvbuf = 0;
    size_t nsndbuf = 0;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

#ifdef _WIN32
    int nfds = 0; // ignored on win32
#else
    int nfds = max(STDIN_FILENO, udt) + 1;
#endif
    fd_set rfds, wfds;

    while (!_terminate) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        if (nsndbuf == 0) {
            nsndbuf = _countof(sndbuf);
            rdt_client_step(rdt, rcvbuf, nrcvbuf, sndbuf, &nsndbuf);
        }
        else {
            rdt_client_step(rdt, rcvbuf, nrcvbuf, NULL, NULL);
        }

        nrcvbuf = 0;
        FD_SET(udt, &rfds);

        if (nsndbuf > 0) {
            FD_SET(udt, &wfds);
        }

#ifndef _WIN32
        if (!eof && nbuf < _countof(buf)) {
            FD_SET(STDIN_FILENO, &rfds);
        }
#endif

        if (nbuf > 0 && rdt_data_write(rdt, buf, nbuf)) {
            nbuf = 0;
        }
        else if (eof) {
            if (rdt_client_done(rdt))
                break;
        }

        int numfd = select(nfds, &rfds, &wfds, NULL, &tv);
        if (numfd == -1) {
            ipk_sock_perror(ipk_sock_errno);
        }
        else if (numfd == 0) {
            continue;
        }
        else {
#ifndef _WIN32
            if (FD_ISSET(STDIN_FILENO, &rfds)) {
#else
            if (!eof) {
#endif
                size_t erecv = _countof(buf) - nbuf;
                size_t nrecv = fread(buf + nbuf, sizeof(char), erecv, stdin);
                _ipk_verbose_log("Readed %zu from stdin.", nrecv);

                nbuf += nrecv;
                if (nrecv != erecv) {
                    if (feof(stdin)) {
                        eof = true;
                    }
                    else if (ferror(stdin) && !ipk_sock_is_recoverable(errno)) {
                        ipk_perror(errno);
                        // send RST
                    }
                }
            }

            if (FD_ISSET(udt, &wfds)) {
                int nsnd = udt_send(udt, sndbuf, nsndbuf);
                if (nsnd == -1) {
                    ipk_sock_perror(ipk_sock_errno);
                    goto return_false;
                }
                else if (nsnd > 0) {
                    nsndbuf = 0;

                    sndbuf[nsnd] = '\0';
                    _ipk_debug_log("#%x <<<\n%s", udt, sndbuf);
                    _ipk_debug_log("#%x /<<<", udt);
                }
                else {
                    // send again
                }
            }

            if (FD_ISSET(udt, &rfds)) {
                int nrcv = udt_recv(udt, rcvbuf, _countof(rcvbuf));
                if (nrcv == -1) {
                    ipk_sock_perror(ipk_sock_errno);
                    goto return_false;
                }
                else if (nrcv > 0) {
                    nrcvbuf = nrcv;

                    rcvbuf[nrcv] = '\0';
                    _ipk_debug_log("#%x >>>\n%s", udt, rcvbuf);
                    _ipk_debug_log("#%x />>>", udt);
                }
                else {
                    // read again
                }
            }
        }
    }

    rdt_dispose(rdt);
    ipk_sock_close(udt);
    return true;

return_false:
    rdt_dispose(rdt);
    ipk_sock_close(udt);
    return false;
}

int main(int argc, char* argv[])
{
    params_t params;
    memset(&params, '\0', sizeof(params_t));

    setlocale(LC_ALL, "");
    ipk_set_log_func(&mylog);
    ipk_set_log_level(1);

    if (!parseargs(argc, argv, &params)) {
        _ipk_error_log("%s", "Chyba: nebyl zadan povinny parametr.");
        fprintf(stdout, "%s", "Synopsis: rdtclient -s source_port -d dest_port\n");
        return EXIT_FAILURE;
    }

    ipk_set_log_level(params.verbose_lvl);

    if (params.action == ACT_HELP) {
        fprintf(stdout, "%s", 
            "Spolehliva komunikace, projekt c. 3 pro predmet IPK.\n"
            "Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz\n"
            "\n"
            "Synopsis: rdtclient -s source_port -d dest_port\n"
            "\n"
            "Parametry:\n"
            "  -h\t\t\tVypise napovedu.\n"
            "  -s source_port\tNasloucha na portu source_port.\n"
            "  -d dest_port\t\tPripoji se na port dest_port.\n");
        return EXIT_SUCCESS;
    }

    // sigint (ctrl+c), sigterm
#ifdef _WIN32
    if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)&on_console_handler, true)) {
        ipk_perror_win(GetLastError());
        return EXIT_FAILURE;
    }
#else
    if (SIG_ERR == signal(SIGINT, &on_signal)) {
        ipk_perror(errno);
        return EXIT_FAILURE;
    }
    if (SIG_ERR == signal(SIGTERM, &on_signal)) {
        ipk_perror(errno);
        return EXIT_FAILURE;
    }
#endif

    if (!ipk_sock_init()) {
        return EXIT_FAILURE;
    }

    if (0 != atexit(&cleanup)) {
        _ipk_error_log("%s", "Chyba: nedostatek pameti");
        return EXIT_FAILURE;
    }

    return process(&params)
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
