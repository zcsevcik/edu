/*
 * Game_net.cpp:   Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Game_net.h"
#include "Game.h"
#include "Move.h"
#include "Color.h"
#include "Log.h"

#include "MsgBase.h"
#include "MsgFactory.h"
#include "MsgChallenge.h"
#include "MsgChallengeResponse.h"
#include "MsgMove.h"
#include "MsgMoveAck.h"
#include "MsgChat.h"
#include "MsgSurrender.h"

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <system_error>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef HOST_NAME_MAX           /* debian, centos fix */
#define HOST_NAME_MAX 256
#endif

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct Game_net::impl {
    std::unique_ptr<Game> game;
    int peer_sock;
    std::vector<int> listen_sock;

    bool conn_async;
    struct addrinfo *conn_base;
    struct addrinfo *conn_ptr;

    PlayerColor myClr;

    chlng_fn pfnChallenge;
    rspchlng_fn pfnRspChallenge;
    move_fn pfnOnMove;
    moveack_fn pfnOnMoveAck;
    chat_fn pfnOnChat;
    surr_fn pfnOnSurr;

    impl(Game *g);
    ~impl();

    int connect();
    int send(const MsgBase &);

    template<class T>
    int handle(T const &);

    std::vector<std::function<int (const MsgBase &)>> handler;

    template<class T>
    std::function<int (MsgBase const &)>
    handle_functor() {
        return [&](MsgBase const &msg) -> int {
            return this->handle<T>(static_cast<T const &>(msg));
        };
    }
};

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgChallenge &msg)
{
    bool accept = pfnChallenge(msg.getTargetColor(), msg.getGame());
    if (accept) {
        myClr = msg.getTargetColor();

        for (auto &i : msg.getGame()) {
            /* white moves */
            if (i.white_move.position.empty()) break;
            if (!game->makeMove(i.white_move))
                return -1;

            /* black moves */
            if (i.black_move.position.empty()) break;
            if (!game->makeMove(i.black_move))
                return -1;
        }

        send(MsgChallengeResponse(ChallengeResponse::Accepted));
        return 0;
    }

    send(MsgChallengeResponse(ChallengeResponse::Denied));
    return -1;
}

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgChallengeResponse &msg)
{
    if (pfnRspChallenge) pfnRspChallenge(
        ChallengeResponse::Accepted == msg.getResponse());
    return msg.getResponse() == ChallengeResponse::Accepted ? 0 : -1;
}

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgMove &msg)
{
    if (game->pulls() == myClr) goto failed;
    if (!game->makeMove(msg.getMove())) goto failed;

    // TODO: check send seqnum by playerclr

    if (pfnOnMove) pfnOnMove(msg.getMove(), true);
    send(MsgMoveAck(game->getSeqNum(), MoveAck::ACK));
    return 0;

failed:
    if (pfnOnMove) pfnOnMove(msg.getMove(), false);
    send(MsgMoveAck(game->getSeqNum(), MoveAck::NACK));
    return 0;
}

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgMoveAck &msg)
{
    if (msg.getAck() == MoveAck::NACK) {
        game->undoMove();
    }

    if (pfnOnMoveAck) pfnOnMoveAck(msg.getAck() == MoveAck::ACK);
    return 0;
}

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgChat &msg)
{
    if (pfnOnChat) pfnOnChat(msg.getText());
    return 0;
}

/* ==================================================================== */
template<>
int Game_net::impl::handle(const MsgSurrender &)
{
    if (pfnOnSurr) pfnOnSurr();
    return 0;
}

/* ==================================================================== */
Game_net::impl::impl(Game *g)
    : game(g),
      peer_sock(-1),
      conn_async(false),
      conn_base(nullptr),
      conn_ptr(nullptr)
{
    handler.emplace_back(handle_functor<MsgChallenge>());
    handler.emplace_back(handle_functor<MsgChallengeResponse>());
    handler.emplace_back(handle_functor<MsgMove>());
    handler.emplace_back(handle_functor<MsgMoveAck>());
    handler.emplace_back(handle_functor<MsgChat>());
    handler.emplace_back(handle_functor<MsgSurrender>());
}

/* ==================================================================== */
Game_net::impl::~impl()
{
    ::close(peer_sock);
    std::for_each(begin(listen_sock), end(listen_sock), &::close);
    listen_sock.clear();
}

/* ==================================================================== */
int Game_net::impl::connect()
{
    int retval = ECONNREFUSED;

    for (struct addrinfo *p = conn_ptr; p; p = p->ai_next) {
        int sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == sockfd) {
            Log::Socket("socket() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            continue;
        }

        /* ============================================================ */
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        ::fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        /* ============================================================ */
        int conn_hr = ::connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (0 == conn_hr) {
            //peer_sock.setSocketDescriptor(sockfd);
            peer_sock = sockfd;
            conn_async = false;
            conn_ptr = nullptr;
            ::freeaddrinfo(conn_base), conn_base = nullptr;
            return 0;
        }
        else if (EINPROGRESS == errno) {
            peer_sock = sockfd;
            conn_async = true;
            conn_ptr = p->ai_next;
            return 0;
        }
        else {
            retval = errno;
            Log::Socket("connect() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            close(sockfd);

            continue;
        }
    }

    ::freeaddrinfo(conn_base), conn_base = nullptr;
    conn_ptr = nullptr;
    conn_async = false;
    peer_sock = -1;

    return retval;
}

/* ==================================================================== */
int Game_net::impl::send(const MsgBase &msg)
{
    Log::Socket("sending msg %02Xh", msg.getType());

    constexpr std::size_t buflen = 1024;
    std::unique_ptr<char []> buf(new char[buflen]);
    std::size_t msglen = MsgFactory::write(buf.get(), buflen, msg);
    std::size_t sentlen = 0;

    /* ============================================================ */
    std::string hexpkt;
    hexpkt.reserve(msglen * (2 + 1));
    for (const char *p = buf.get(); p < buf.get() + msglen; ++p) {
        char tmp[8];
        sprintf(tmp, "%02X ", (unsigned char) *p);
        hexpkt.append(tmp);
    }
    hexpkt.pop_back();
    Log::Socket("sending packet: %s", hexpkt.c_str());

    while (sentlen < msglen) {
        /* ============================================================ */
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(peer_sock, &wfds);

        int sel_hr = ::select(peer_sock + 1, nullptr, &wfds,
                              nullptr, nullptr);
        if (-1 == sel_hr) {
            Log::Socket("select() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }
        else if (0 == sel_hr) {
            Log::Socket("select() returned 0");
            return EINPROGRESS;
        }

        /* ============================================================ */
        ssize_t send_hr = ::send(peer_sock,
                                 buf.get() + sentlen,
                                 msglen - sentlen,
                                 MSG_NOSIGNAL);
        if (-1 == send_hr) {
            Log::Socket("send() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }
        else if (!send_hr) {
            Log::Assert();
        }
        else {
            sentlen += send_hr;
        }
    }

    return 0;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
Game_net::Game_net(Game *g)
    : _pImpl(new impl(g))
{ }

/* ==================================================================== */
Game_net::~Game_net() = default;

/* ==================================================================== */
Game *Game_net::getGame()
{
    return _pImpl->game.get();
}

/* ==================================================================== */
int Game_net::getSock() const
{
    //return _pImpl->peer_sock.socketDescriptor();
    return _pImpl->peer_sock;
}

/* ==================================================================== */
int Game_net::listen(uint16_t port)
{
    struct addrinfo hints;
    std::memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_NUMERICSERV;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *res = nullptr;

    int gai_hr = ::getaddrinfo(nullptr,
                               std::to_string(port).c_str(),
                               &hints,
                               &res);

    if (0 != gai_hr) {
        Log::Socket("getaddrinfo() failed: %s", gai_strerror(gai_hr));
        return gai_hr;
    }

    /* ================================================================ */
    for (struct addrinfo *p = res; p; p = p->ai_next) {
        int sockfd;
        goto create_sock;

        /* ============================================================ */
    push_sock:
        _pImpl->listen_sock.push_back(sockfd);
        continue;

        /* ============================================================ */
    clean_sock:
        close(sockfd);
        continue;

        /* ============================================================ */
    create_sock:
        sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == sockfd) {
            Log::Socket("socket() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            continue;
        }

        /* ============================================================ */
        int bind_hr = ::bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (-1 == bind_hr) {
            Log::Socket("bind() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            goto clean_sock;
        }

        /* ============================================================ */
        int listen_hr = ::listen(sockfd, 1);
        if (-1 == listen_hr) {
            Log::Socket("listen() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            goto clean_sock;
        }

        /* ============================================================ */
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        ::fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        /* ============================================================ */
        struct sockaddr_storage localname;
        socklen_t localnamelen = sizeof(struct sockaddr_storage);
        int gsn_hr = ::getsockname(sockfd,
                                   (struct sockaddr *) &localname,
                                   &localnamelen);

        if (0 != gsn_hr) {
            Log::Socket("getsockname() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            goto push_sock;
        }

        /* ============================================================ */
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        int gni_hr = ::getnameinfo(
                (struct sockaddr *) &localname, localnamelen,
                host, sizeof(host),
                serv, sizeof(serv),
                0);

        if (0 != gni_hr) {
            Log::Socket("getnameinfo() failed: %s", gai_strerror(gni_hr));
            goto push_sock;
        }

        const char *fmt = (localname.ss_family == AF_INET6)
            ? "Listening on [%s]:%s"
            : "Listening on %s:%s";
        Log::Socket(fmt, host, serv);

        goto push_sock;
    }

    /* ================================================================ */
    ::freeaddrinfo(res);
    return 0;
}

/* ==================================================================== */
std::vector<std::string>
Game_net::getListenAddr() const
{
    std::vector<std::string> S;
    S.reserve(_pImpl->listen_sock.size());

    for (int &sockfd : _pImpl->listen_sock) {
        struct sockaddr_storage localname;
        socklen_t localnamelen = sizeof(struct sockaddr_storage);
        int gsn_hr = ::getsockname(sockfd, (struct sockaddr *) &localname,
                                   &localnamelen);

        if (0 != gsn_hr) {
            Log::Socket("getsockname() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            continue;
        }

        /* ============================================================ */
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        int gni_hr = ::getnameinfo((struct sockaddr *) &localname,
                                   localnamelen,
                                   host, sizeof(host),
                                   serv, sizeof(serv),
                                   NI_NUMERICHOST | NI_NUMERICSERV);

        if (0 != gni_hr) {
            Log::Socket("getnameinfo() failed: %s", gai_strerror(gni_hr));
            continue;
        }

        /* ============================================================ */
        std::string addr;
        if (PF_INET6 == localname.ss_family) addr.push_back('[');
        addr.append(host);
        if (PF_INET6 == localname.ss_family) addr.push_back(']');
        addr.push_back(':');
        addr.append(serv);

        /* ============================================================ */
        S.push_back(addr);
    }

    return S;
}

/* ==================================================================== */
const std::vector<int> &
Game_net::getListenSock() const
{
    return _pImpl->listen_sock;
}

/* ==================================================================== */
std::string
Game_net::getLocalName() const
{
    char szHostName[HOST_NAME_MAX];
    int h_hr = ::gethostname(szHostName, _countof(szHostName));
    if (-1 == h_hr) {
        throw std::system_error(errno, std::system_category());
    }

    /* ================================================================ */
    struct addrinfo hints, *res;
    std::memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int gai_hr = ::getaddrinfo(szHostName, nullptr, &hints, &res);
    if (0 != gai_hr) {
        Log::Socket("getaddrinfo() failed: %s", gai_strerror(gai_hr));
        return szHostName;
    }

    /* ================================================================ */
    std::string name = res->ai_canonname;
    ::freeaddrinfo(res);
    return name;
}

/* ==================================================================== */
std::string
Game_net::getPeerName() const
{
    struct sockaddr_storage peername;
    socklen_t peernamelen = sizeof(struct sockaddr_storage);

    int pn_hr = ::getpeername(_pImpl->peer_sock,
                              (struct sockaddr *) &peername,
                              &peernamelen);
    if (-1 == pn_hr) {
        throw std::system_error(errno, std::system_category());
    }

    /* ================================================================ */
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    int gni_hr = ::getnameinfo((struct sockaddr *) &peername,
                               peernamelen,
                               host, sizeof(host),
                               serv, sizeof(serv),
                               NI_NUMERICSERV);

    if (0 != gni_hr) {
        throw std::system_error(gni_hr, std::system_category(),
                                gai_strerror(gni_hr));
    }

    /* ============================================================ */
    std::string addr;
    //if (PF_INET6 == peername.ss_family) addr.push_back('[');
    addr.append(host);
    //if (PF_INET6 == peername.ss_family) addr.push_back(']');
    addr.push_back(':');
    addr.append(serv);

    return addr;
}

/* ==================================================================== */
int
Game_net::waitAccept()
{
    if (_pImpl->listen_sock.empty())
        throw std::system_error(EBADF, std::system_category());

    int nfds = 1 + *std::max_element(begin(_pImpl->listen_sock),
                                     end(_pImpl->listen_sock));

    fd_set rfds;
    FD_ZERO(&rfds);

    for (int sockfd : _pImpl->listen_sock) {
        FD_SET(sockfd, &rfds);
    }

    int retval = ::select(nfds, &rfds, nullptr, nullptr, nullptr);
    if (-1 == retval) {
        Log::Socket("select() failed: %s", std::system_category()
            .default_error_condition(errno).message().c_str());
        return errno;
    }
    else if (0 == retval) {
        Log::Socket("select() returned 0");
        return EINPROGRESS;
    }
    else {
        int sockfd_peer = -1;

        /* ============================================================ */
        for (int sockfd : _pImpl->listen_sock) {
            if (!FD_ISSET(sockfd, &rfds)) continue;

            struct sockaddr_storage peername;
            socklen_t peernamelen = sizeof(struct sockaddr_storage);
            sockfd_peer = ::accept(sockfd,
                                   (struct sockaddr *) &peername,
                                   &peernamelen);
            if (-1 == sockfd_peer) {
                Log::Socket("accept() failed: %s", std::system_category()
                    .default_error_condition(errno).message().c_str());

                continue;
            }

            /* ======================================================== */
            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            int gni_hr = ::getnameinfo((struct sockaddr *) &peername,
                                       peernamelen,
                                       host, sizeof(host),
                                       serv, sizeof(serv),
                                       NI_NUMERICSERV);

            if (0 != gni_hr) {
                Log::Socket("getnameinfo() failed: %s",
                            gai_strerror(gni_hr));
                break;
            }

            /* ======================================================== */
            std::string addr;
            addr.append(host);
            addr.push_back(':');
            addr.append(serv);

            Log::Socket("accept(): %s", addr.c_str());
            break;
        }

        /* ============================================================ */
        if (-1 == sockfd_peer) return EAGAIN;

        std::for_each(begin(_pImpl->listen_sock),
                      end(_pImpl->listen_sock),
                      &::close);
        _pImpl->listen_sock.clear();

        /* ============================================================ */
        int flags = ::fcntl(sockfd_peer, F_GETFL, 0);
        ::fcntl(sockfd_peer, F_SETFL, flags & ~O_NONBLOCK);

        /* ============================================================ */
        //_pImpl->peer_sock.setSocketDescriptor(sockfd_peer);
        _pImpl->peer_sock = sockfd_peer;
    }

    return 0;
}

/* ==================================================================== */
int
Game_net::beginConnect(std::string host, std::string port)
{
    struct addrinfo hints, *res = nullptr;
    std::memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_CANONNAME | AI_NUMERICSERV;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int gai_hr = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (0 != gai_hr) {
        Log::Socket("getaddrinfo() failed: %s", gai_strerror(gai_hr));
        return gai_hr;
    }

    Log::Socket("Resolved '%s:%s' as '%s'",
                host.c_str(), port.c_str(), res->ai_canonname);

    if (-1 != _pImpl->peer_sock) close(_pImpl->peer_sock);
    _pImpl->peer_sock = -1;
    _pImpl->conn_base = res;
    _pImpl->conn_ptr = res;
    _pImpl->conn_async = false;

    return _pImpl->connect();
}

/* ==================================================================== */
int
Game_net::endConnect()
{
    if (_pImpl->conn_async) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(_pImpl->peer_sock, &wfds);

        int select_hr = ::select(_pImpl->peer_sock + 1, nullptr, &wfds,
                                 nullptr, nullptr);

        if (-1 == select_hr) {
            Log::Socket("select() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }
        else if (0 == select_hr) {
            Log::Socket("select() returned 0");
            return EINPROGRESS;
        }

        /* ============================================================ */
        int conn_hr;
        socklen_t optlen = sizeof(conn_hr);

        int gso_hr = ::getsockopt(_pImpl->peer_sock, SOL_SOCKET, SO_ERROR,
                                  &conn_hr, &optlen);
        if (-1 == gso_hr) {
            Log::Socket("getsockopt() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }

        /* ============================================================ */
        if (0 == conn_hr) {
            _pImpl->conn_async = false;
            _pImpl->conn_ptr = nullptr;
            ::freeaddrinfo(_pImpl->conn_base), _pImpl->conn_base = nullptr;
            return 0;
        }
        else {
            close(_pImpl->peer_sock), _pImpl->peer_sock = -1;
            conn_hr = _pImpl->connect();
            if (0 == conn_hr && _pImpl->conn_async) return EINPROGRESS;
            else if (0 == conn_hr) {
                int flags = ::fcntl(_pImpl->peer_sock, F_GETFL, 0);
                ::fcntl(_pImpl->peer_sock, F_SETFL, flags & ~O_NONBLOCK);

                return 0;
            }
            else {
                return conn_hr;
            }
        }
    }
    else if (_pImpl->peer_sock != -1) {
        int flags = ::fcntl(_pImpl->peer_sock, F_GETFL, 0);
        ::fcntl(_pImpl->peer_sock, F_SETFL, flags & ~O_NONBLOCK);

        return 0;
    }
    else {
        return EBADF;
    }
}

/* ==================================================================== */
bool
Game_net::dataAvailable() const
{
    if (_pImpl->peer_sock != -1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(_pImpl->peer_sock, &rfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int sel_hr = ::select(_pImpl->peer_sock + 1, &rfds, nullptr,
                              nullptr, &tv);

        if (-1 == sel_hr) {
            Log::Socket("select() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return false;
        }
        else if (0 == sel_hr) {
            return false;
        }

        return true;
    }

    return false;
}

/* ==================================================================== */
int
Game_net::waitMessage()
{
    if (_pImpl->peer_sock != -1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(_pImpl->peer_sock, &rfds);

        int sel_hr = ::select(_pImpl->peer_sock + 1, &rfds, nullptr,
                              nullptr, nullptr);
        if (-1 == sel_hr) {
            Log::Socket("select() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }
        else if (0 == sel_hr) {
            Log::Socket("select() returned 0");
            return EINPROGRESS;
        }

        /* ============================================================ */
        constexpr std::size_t buflen = 1024;
        std::unique_ptr<char []> buf_p(new char[buflen]);
        char *buf = buf_p.get();

        ssize_t rcv_hr = ::recv(_pImpl->peer_sock,
                                buf, MsgFactory::minLength(),
                                MSG_WAITALL);
        if (-1 == rcv_hr) {
            Log::Socket("recv() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            return errno;
        }
        else if (!rcv_hr) {
            ::shutdown(_pImpl->peer_sock, SHUT_RDWR);
            return -1;
        }

        /* ============================================================ */
        size_t rcvlen = MsgFactory::minLength();
        size_t pktlen = rcvlen + MsgFactory::pktLength(buf);

        while (rcvlen < pktlen) {
            FD_ZERO(&rfds);
            FD_SET(_pImpl->peer_sock, &rfds);

            sel_hr = ::select(_pImpl->peer_sock + 1, &rfds, nullptr,
                              nullptr, nullptr);
            if (-1 == sel_hr) {
                Log::Socket("select() failed: %s", std::system_category()
                    .default_error_condition(errno).message().c_str());
                return errno;
            }
            else if (0 == sel_hr) {
                Log::Socket("select() returned 0");
                return EINPROGRESS;
            }

            /* ======================================================== */
            rcv_hr = ::recv(_pImpl->peer_sock,
                            buf + rcvlen,
                            pktlen - rcvlen,
                            0);

            if (-1 == rcv_hr) {
                Log::Socket("recv() failed: %s", std::system_category()
                    .default_error_condition(errno).message().c_str());
                return errno;
            }
            else if (!rcv_hr) {
                ::shutdown(_pImpl->peer_sock, SHUT_RDWR);
                return -1;
            }
            else {
                rcvlen += rcv_hr;
            }

        }

        Log::AssertIf(rcvlen == pktlen, "bad msg size");

        /* ============================================================ */
        std::string hexpkt;
        hexpkt.reserve(rcvlen * (2 + 1));
        for (const char *p = buf; p < buf + rcvlen; ++p) {
            char tmp[8];
            sprintf(tmp, "%02X ", (unsigned char) *p);
            hexpkt.append(tmp);
        }
        hexpkt.pop_back();
        Log::Socket("rcvd packet: %s", hexpkt.c_str());

        /* ============================================================ */
        MsgBase *msg_p = nullptr;
        size_t msglen = MsgFactory::read(buf, rcvlen, msg_p);

        if ((size_t) -1 == msglen) {
            Log::Socket("dropped unparseable msg");
            return -1;
        }

        std::unique_ptr<MsgBase> msg(msg_p);
        Log::Socket("received msg %02Xh", msg->getType());

        return _pImpl->handler.at(msg->getType())(*msg);
    }

    return EBADF;
}

/* ==================================================================== */
int
Game_net::sendGame(PlayerColor initiatorClr)
{
    PlayerColor targetClr = PlayerColor::White == initiatorClr
                          ? PlayerColor::Black
                          : PlayerColor::White;

    _pImpl->myClr = initiatorClr;
    const std::vector<Moves> mvs = _pImpl->game->getMoves();
    MsgChallenge msg(mvs.empty() ? ChallengeType::New : ChallengeType::Saved,
                     initiatorClr, targetClr);
    msg.setGame(std::move(mvs));
    return _pImpl->send(msg);
}

/* ==================================================================== */
std::string
Game_net::errno_to_string(int ec) const
{
    if (ec < 0) return gai_strerror(ec);
    else if (ec > 0) return std::error_code(ec, std::system_category()).message();
    else return "";
}

/* ==================================================================== */
bool Game_net::mePulls() const
{
    return _pImpl->myClr == _pImpl->game->pulls();
}

/* ==================================================================== */
PlayerColor Game_net::myColor() const
{
    return _pImpl->myClr;
}

/* ==================================================================== */
bool Game_net::makeMove(const Turn &mv)
{
    if (_pImpl->game->makeMove(mv)) {
        MsgMove msg;
        msg.setMove(mv);
        _pImpl->send(msg);
        return true;
    }

    return false;
}

/* ==================================================================== */
void Game_net::setCallbackChallenge(chlng_fn value)
{
    _pImpl->pfnChallenge = value;
}

/* ==================================================================== */
void Game_net::setCallbackChallengeResponse(rspchlng_fn value)
{
    _pImpl->pfnRspChallenge = value;
}

/* ==================================================================== */
void Game_net::setOnMove(move_fn value)
{
    _pImpl->pfnOnMove = value;
}

/* ==================================================================== */
void Game_net::setOnMoveAck(moveack_fn value)
{
    _pImpl->pfnOnMoveAck = value;
}

/* ==================================================================== */
void Game_net::setOnChat(chat_fn value)
{
    _pImpl->pfnOnChat = value;
}

/* ==================================================================== */
void Game_net::setOnSurrender(surr_fn value)
{
    _pImpl->pfnOnSurr = value;
}

}  // namespace icp
