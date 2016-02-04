/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifndef IPK_SOCK_H
#define IPK_SOCK_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
# include <windows.h>
# include <winsock2.h>
# include <ws2tcpip.h>

# define IPK_SOCK_INVALID INVALID_SOCKET
# define ipk_sock_errno WSAGetLastError()
# define ipk_sock_close closesocket
# define SHUT_RDWR SD_BOTH
# define SHUT_RD SD_RECEIVE
# define SHUT_WR SD_SEND
  typedef SOCKET ipk_sock_t;

#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <unistd.h>

# define IPK_SOCK_INVALID -1
# define ipk_sock_errno errno
# define ipk_sock_close close
  typedef int ipk_sock_t;

#endif

bool ipk_sock_init();
bool ipk_sock_free();
bool ipk_sock_is_recoverable(int);
void ipk_sock_perror(int);
bool ipk_fd_block_mode(ipk_sock_t, bool blocking);

#ifdef __cplusplus
}
#endif

#endif //IPK_SOCK_H
