/*
 * ouisearch.c:    Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/14
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "oui_internal.h"
#include "ouidb.h"
#include "ouinetwork.h"
#include "ouiqueue.h"

#define PACKETS_PER_SEC_DEF UINT16_MAX
#define PACKETS_PER_SEC_MAX UINT16_MAX

// ----------------------------------------------------------------------------
typedef struct {
    struct in6_addr prefix;
    uint8_t         prefixlen;
    char*           ouifile;

    uint16_t        netlimit;
    bool            verbose;
} params_t;

// ----------------------------------------------------------------------------
static int parseargs(int argc, char** argv, params_t* params)
{
    assert(argc >= 0);
    assert(argv != NULL);
    assert(params != NULL);

    int opt;
    int getopt_optind = 1;
    struct option getopt_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "debug", no_argument, NULL, 'v' },
        { "prefix", required_argument, NULL, 'p' },
        { "database", required_argument, NULL, 'd' },
        { "bandwidth", required_argument, NULL, 'b' },
        { 0, 0, 0, 0 }
    };

    opterr = 1;

    if (argc == 1) {
        return EHELP;
    }

    params->netlimit = PACKETS_PER_SEC_DEF;

    while ((opt = getopt_long(argc, argv, "hvp:d:",
            getopt_opts, &getopt_optind)) != -1)
    {
        switch (opt) {
            case 0:
                continue;

            default:
                return EINVALARG;

            case 'h':
                return EHELP;

            case 'v':
                params->verbose = true;
                break;

            case 'b': {
                int val = atoi(optarg);
                if (val <= 0)
                    val = 1;
                else if (val >= PACKETS_PER_SEC_MAX)
                    val = PACKETS_PER_SEC_MAX;

                params->netlimit = (uint16_t)val;
            }   break;

            case 'p': {
                char* p = strrchr(optarg, '/');
                if (!p) {
                    return EARGNET;
                }

                *p++ = '\0';
                int prefixlen = atoi(p);
                if (prefixlen <= 0 || prefixlen > 64) {
                    return EARGNET;
                }

                params->prefixlen = (uint8_t)prefixlen;

                struct addrinfo* res = NULL;
                struct addrinfo hints = {
                    .ai_flags = AI_NUMERICHOST,
                    .ai_family = AF_INET6,
                    .ai_socktype = SOCK_RAW,
                    .ai_protocol = IPPROTO_ICMPV6,
                };

                int eai_hr = getaddrinfo(optarg, NULL, &hints, &res);
                if (eai_hr != 0) {
                    fprintf(stderr, "%s\n", gai_strerror(eai_hr));
                    return EARGNET;
                }

                params->prefix = ((struct sockaddr_in6*)res->ai_addr)->sin6_addr;
                freeaddrinfo(res);
            }   break;

            case 'd':
                params->ouifile = optarg;
                break;
        }
    }

    if (!params->ouifile) {
        return EARGOUI;
    }
    if (params->prefixlen == 0) {
        return EARGNET;
    }

    return EOK;
}

static void verbose(const char* msg)
{
    fprintf(stderr, "## %s\n", msg);
}

static void usage()
{
    fprintf(stdout, "%s",
        "ISA projekt #1: Programovani sitove sluzby\n"
        "Mapovani adresoveho prostoru IPv6 pomoci OUI.\n"
        "\n"
        "Pouziti: " PACKAGE " -p sit/rozsah -d filename\n"
        "\n"
        "Parametry: \n"
        "  -p\tAdresovy prostor.\n"
        "  -d\tDatabaze OUI.\n"
        "\n"
        PACKAGE_AUTHOR " <" PACKAGE_BUGREPORT ">.\n"
    );
}

static void signal_handler(int sig)
{
    if (sig == SIGINT ||
        sig == SIGTERM)
    {
        _terminate = true;
    }
}

static void addr_found(struct in6_addr addr, void* arg)
{
    (void)arg;

    char ip[INET6_ADDRSTRLEN];
    _addr2name6(addr, ip, _countof(ip));

    const char* ouidesc = ouidb_search(addr);

    fprintf(stdout, "%s %s\n", ip, ouidesc ? ouidesc : "");
}

int main(int argc, char* argv[])
{
    params_t params;
    memset(&params, '\0', sizeof(params_t));

    // nastavi locale podle prostredi -- ceske hlasky
    setlocale(LC_ALL, "");

    // odchytneme signal ukonceni
    if (SIG_ERR == signal(SIGINT, &signal_handler)) {
        fprintf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (SIG_ERR == signal(SIGTERM, &signal_handler)) {
        fprintf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // parsnuti argumentu
    switch (parseargs(argc, argv, &params))
    {
        case EOK:
            break;

        case EHELP:
            usage();
            return EXIT_SUCCESS;

        case EARGOUI:
            fprintf(stderr, "Chyba v parametru OUI databaze.\n");
            return EXIT_FAILURE;

        case EARGNET:
            fprintf(stderr, "Chyba v parametru IPv6 adresy.\n");
            return EXIT_FAILURE;

        default:
            return EXIT_FAILURE;
    }

    _oui_set_dbg(params.verbose);
    _oui_set_dbg_fn(&verbose);

    if (!ouidb_init(params.ouifile)) {
        fprintf(stderr, "Chyba pri nacitani OUI databaze.\n");
        return EXIT_FAILURE;
    }

    if (!ouiqueue_init()) {
        ouidb_cleanup();
        return EXIT_FAILURE;
    }

    oui_scan_network(params.prefix, params.prefixlen, params.netlimit, &addr_found, NULL);

    ouiqueue_cleanup();
    ouidb_cleanup();
    return EXIT_SUCCESS;
}
