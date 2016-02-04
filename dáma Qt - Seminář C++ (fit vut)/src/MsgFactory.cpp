/*
 * MsgFactory.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "MsgFactory.h"
#include "MsgBase.h"
#include "MsgChallenge.h"
#include "MsgChallengeResponse.h"
#include "MsgChat.h"
#include "MsgMove.h"
#include "MsgMoveAck.h"
#include "MsgSurrender.h"

#include "MyEndian.h"

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <vector>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
typedef std::size_t (*msg_read_t)(const char *, std::size_t, MsgBase* &);
typedef std::size_t (*msg_write_t)(char *, std::size_t, const MsgBase &);
typedef MsgBase *(*msg_ctor_t)();

/* ==================================================================== */
class MsgFactory::impl
{
private:
    static std::size_t read_msgchlng(const char *, std::size_t, MsgBase* &);
    static std::size_t read_msgchlngr(const char *, std::size_t, MsgBase* &);
    static std::size_t read_msgmv(const char *, std::size_t, MsgBase* &);
    static std::size_t read_msgmvack(const char *, std::size_t, MsgBase* &);
    static std::size_t read_msgchat(const char *, std::size_t, MsgBase* &);
    static std::size_t read_msgsurr(const char *, std::size_t, MsgBase* &);

    static std::size_t write_msgchlng(char *, std::size_t, const MsgBase &);
    static std::size_t write_msgchlngr(char *, std::size_t, const MsgBase &);
    static std::size_t write_msgmv(char *, std::size_t, const MsgBase &);
    static std::size_t write_msgmvack(char *, std::size_t, const MsgBase &);
    static std::size_t write_msgchat(char *, std::size_t, const MsgBase &);
    static std::size_t write_msgsurr(char *, std::size_t, const MsgBase &);

    template<class T>
    static MsgBase *
    mycreate() {
        return new (T);
    }

public:
    template<typename T>
    using is_least16 = std::enable_if
        <std::is_integral<T>::value &&
        sizeof(T) >= sizeof(std::uint16_t), T>;

    template<class T>
    static bool
    mywrite(const typename is_least16<T>::type &, char *&, std::size_t &);

    template<class T>
    static bool
    mywrite(const T &val, char *&, std::size_t &);

    template<class T>
    static bool
    myread(typename is_least16<T>::type &, const char *&, std::size_t &);

    template<class T>
    static bool
    myread(T &, const char *&, std::size_t &);

    template<class T>
    static bool
    mycmp(const T &val, const char *&, std::size_t &);

public:
    static std::vector<msg_read_t> Read;
    static std::vector<msg_write_t> Write;
    static std::vector<msg_ctor_t> Ctor;

    static std::string Header;
    static std::string ProtocolRevision;

    static const std::size_t ErrorValue;
};

/* ==================================================================== */
const std::size_t MsgFactory::impl::ErrorValue = (size_t) -1;

/* ==================================================================== */
std::string MsgFactory::impl::Header = u8"ICP-Dama 2013";

/* ==================================================================== */
std::string MsgFactory::impl::ProtocolRevision = u8"0";

/* ==================================================================== */
std::vector<msg_read_t> MsgFactory::impl::Read = {
        &read_msgchlng,
        &read_msgchlngr,
        &read_msgmv,
        &read_msgmvack,
        &read_msgchat,
        &read_msgsurr,
};

/* ==================================================================== */
std::vector<msg_write_t> MsgFactory::impl::Write = {
        &write_msgchlng,
        &write_msgchlngr,
        &write_msgmv,
        &write_msgmvack,
        &write_msgchat,
        &write_msgsurr,
};

/* ==================================================================== */
std::vector<msg_ctor_t> MsgFactory::impl::Ctor = {
        &mycreate<MsgChallenge>,
        &mycreate<MsgChallengeResponse>,
        &mycreate<MsgMove>,
        &mycreate<MsgMoveAck>,
        &mycreate<MsgChat>,
        &mycreate<MsgSurrender>,
};

/* ==================================================================== */
/*      S  T  R  E  A  M     O  P  E  R  A  T  I  O  N  S               */
/* ==================================================================== */

/* ==== I N T E G E R ================================================= */
template<class T>
bool
MsgFactory::impl::mywrite(const typename is_least16<T>::type &val,
                          char *&buf, std::size_t &len)
{
    if (len < sizeof(T)) return false;
    *reinterpret_cast<T *>(buf) = hton(val);
    buf += sizeof(T);
    len -= sizeof(T);
    return true;
}

template<class T>
bool
MsgFactory::impl::myread(typename is_least16<T>::type &val,
                         const char *&buf, std::size_t &len)
{
    if (len < sizeof(T)) return false;
    val = ntoh(*reinterpret_cast<const T *>(buf));
    buf += sizeof(T);
    len -= sizeof(T);
    return true;
}

/* ==== C H A R ======================================================= */
template<>
bool
MsgFactory::impl::mywrite(const char &val,
                          char *&buf, std::size_t &len)
{
    if (len < sizeof(char)) return false;
    *buf++ = val;
    len--;
    return true;
}

template<>
bool
MsgFactory::impl::myread(char &val,
                         const char *&buf, std::size_t &len)
{
    if (len < sizeof(char)) return false;
    val = *buf++;
    len--;
    return true;
}

/* ==== S T R I N G =================================================== */
template<>
bool
MsgFactory::impl::mywrite(const std::string &val,
                          char *&buf, std::size_t &len)
{
    if (len < val.size() + sizeof(char)) return false;
    std::string::size_type i = val.copy(buf, std::string::npos);
    *(buf += i++)++ = '\0';
    len -= i;
    return true;
}

template<>
bool
MsgFactory::impl::mycmp(const std::string &ref,
                        const char *&buf, std::size_t &len)
{
    if (len < ref.size() + sizeof(char)) return false;
    if (0 != std::strncmp(ref.c_str(), buf, ref.size())) return false;
    buf += ref.size() + sizeof(char);
    len -= ref.size() + sizeof(char);
    return true;
}

/* ==== T U R N ======================================================= */
template<>
bool
MsgFactory::impl::mywrite(const Turn &val,
                          char *&buf, std::size_t &len)
{
    if (len < sizeof(uint16_t)) return false;
    mywrite<std::uint16_t>(
        (std::uint16_t) val.capture << 15 |
        (std::uint16_t) val.position.size(),
        buf, len);

    for (auto &loc : val.position) {
        if (len < sizeof(char)) return false;
        mywrite<char>((loc.file << 4) | (loc.rank & 0x0f),
                      buf, len);
    }

    return true;
}

template<>
bool
MsgFactory::impl::myread(Turn &val,
                         const char *&buf, std::size_t &len)
{
    if (len < sizeof(uint16_t)) return false;
    uint16_t size;
    myread<std::uint16_t>(size, buf, len);
    val.position.reserve(size & ~(1U << 15));
    val.capture = !!(size & (1U << 15));

    size &= ~(1U << 15);
    while (size--) {
        if (len < sizeof(char)) return false;
        char c;
        myread<char>(c, buf, len);
        val.position.emplace_back(Square(c >> 4, c & 0x0F));
    }

    return true;
}

/* ==================================================================== */
/*      M  E  S  S  A  G  E  S                                          */
/* ==================================================================== */
#define CHECKED(x) \
    if (!(x)) return MsgFactory::impl::ErrorValue

#define CHECKED_LEN(len, x) do { \
    std::size_t retval = (x); \
    if (retval == MsgFactory::impl::ErrorValue) \
        return MsgFactory::impl::ErrorValue; \
    else \
        (len) -= retval; \
} while (0)

/* ==== C H A L L E N G E ============================================= */
std::size_t MsgFactory::impl::read_msgchlng(const char *buf,
                                            std::size_t len,
                                            MsgBase* &msg)
{
    const std::size_t len_ = len;
    MsgChallenge *msg_t = static_cast<MsgChallenge *>(msg);
    char c;

    CHECKED(myread(c, buf, len));
    msg_t->setChallengeType((ChallengeType) c);

    CHECKED(myread(c, buf, len));
    msg_t->setInitiatorColor((PlayerColor) c);

    CHECKED(myread(c, buf, len));
    msg_t->setTargetColor((PlayerColor) c);

    if (msg_t->getChallengeType() == ChallengeType::Saved) {
        uint16_t gamelen;
        CHECKED(myread<uint16_t>(gamelen, buf, len));

        std::vector<Moves> mvs;
        mvs.reserve(gamelen);

        while (gamelen--) {
            seqnum_t seqnum;
            Turn white_move;
            Turn black_move;

            CHECKED(myread<uint16_t>(seqnum, buf, len));
            CHECKED(myread(white_move, buf, len));
            CHECKED(myread(black_move, buf, len));

            mvs.emplace_back(Moves(seqnum, white_move, black_move));
        }

        msg_t->setGame(std::move(mvs));
    }

    return len_ - len;
}

std::size_t MsgFactory::impl::write_msgchlng(char *buf,
                                             std::size_t len,
                                             const MsgBase &msg)
{
    const std::size_t len_ = len;
    const MsgChallenge &msg_t = static_cast<const MsgChallenge &>(msg);

    CHECKED(mywrite<char>((char )msg_t.getChallengeType(), buf, len));
    CHECKED(mywrite<char>((char )msg_t.getInitiatorColor(), buf, len));
    CHECKED(mywrite<char>((char )msg_t.getTargetColor(), buf, len));

    if (msg_t.getChallengeType() == ChallengeType::Saved) {
        CHECKED(mywrite<uint16_t>((uint16_t )msg_t.getGame().size(), buf, len));
        for (auto &mvs : msg_t.getGame()) {
            CHECKED(mywrite<uint16_t>(mvs.seqnum, buf, len));
            CHECKED(mywrite(mvs.white_move, buf, len));
            CHECKED(mywrite(mvs.black_move, buf, len));
        }
    }

    return len_ - len;
}

/* ==== C H A L L E N G E   R E S P O N S E =========================== */
std::size_t MsgFactory::impl::read_msgchlngr(const char *buf,
                                             std::size_t len,
                                             MsgBase* &msg)
{
    const std::size_t len_ = len;
    MsgChallengeResponse *msg_t = static_cast<MsgChallengeResponse *>(msg);
    char c;

    CHECKED(myread(c, buf, len));
    msg_t->setResponse((ChallengeResponse) c);

    return len_ - len;
}

std::size_t MsgFactory::impl::write_msgchlngr(char *buf,
                                              std::size_t len,
                                              const MsgBase &msg)
{
    const std::size_t len_ = len;
    const MsgChallengeResponse &msg_t =
        static_cast<const MsgChallengeResponse &>(msg);

    CHECKED(mywrite<char>((char )msg_t.getResponse(), buf, len));

    return len_ - len;
}

/* ==== M O V E ======================================================= */
std::size_t MsgFactory::impl::read_msgmv(const char *buf,
                                         std::size_t len,
                                         MsgBase* &msg)
{
    const std::size_t len_ = len;
    MsgMove *msg_t = static_cast<MsgMove *>(msg);
    Turn mv;

    CHECKED(myread(mv, buf, len));
    msg_t->setMove(mv);

    return len_ - len;
}

std::size_t MsgFactory::impl::write_msgmv(char *buf,
                                          std::size_t len,
                                          const MsgBase &msg)
{
    const std::size_t len_ = len;
    const MsgMove &msg_t = static_cast<const MsgMove &>(msg);
    const Turn &mv = msg_t.getMove();

    CHECKED(mywrite(mv, buf, len));

    return len_ - len;
}

/* ==== M O V E   A C K N O W L E D G E =============================== */
std::size_t MsgFactory::impl::read_msgmvack(const char *buf,
                                            std::size_t len,
                                            MsgBase* &msg)
{
    const std::size_t len_ = len;
    MsgMoveAck *msg_t = static_cast<MsgMoveAck *>(msg);
    char c;
    uint16_t h;

    CHECKED(myread<std::uint16_t>(h, buf, len));
    msg_t->setSeqNum((seqnum_t) h);

    CHECKED(myread<char>(c, buf, len));
    msg_t->setAck((MoveAck) c);

    return len_ - len;
}

std::size_t MsgFactory::impl::write_msgmvack(char *buf,
                                             std::size_t len,
                                             const MsgBase &msg)
{
    const std::size_t len_ = len;
    const MsgMoveAck &msg_t = static_cast<const MsgMoveAck &>(msg);

    CHECKED(mywrite<std::uint16_t>((uint16_t)msg_t.getSeqNum(), buf, len));
    CHECKED(mywrite<char>((char)msg_t.getAck(), buf, len));

    return len_ - len;
}

/* ==== C H A T ======================================================= */
std::size_t MsgFactory::impl::read_msgchat(const char *buf,
                                           std::size_t len,
                                           MsgBase* &msg)
{
    const std::size_t len_ = len;
    MsgChat *msg_t = static_cast<MsgChat *>(msg);

    uint16_t slen;
    CHECKED(myread<uint16_t>(slen, buf, len));

    std::string str(buf, slen);
    buf += slen + 1;
    len -= slen + 1;

    msg_t->setText(str);

    return len_ - len;
}

std::size_t MsgFactory::impl::write_msgchat(char *buf,
                                            std::size_t len,
                                            const MsgBase &msg)
{
    const std::size_t len_ = len;
    const MsgChat &msg_t = static_cast<const MsgChat &>(msg);
    const std::string &text = msg_t.getText();

    if (text.length() + sizeof(uint16_t) + sizeof(char) > len)
        return ErrorValue;

    CHECKED(mywrite<std::uint16_t>((uint16_t )text.length(), buf, len));

    msg_t.getText().copy(buf, std::string::npos, 0);
    buf += text.length();
    len -= text.length();

    CHECKED(mywrite<char>((char )'\0', buf, len));

    return len_ - len;
}

/* ==== S U R R E N D E R ============================================= */
std::size_t MsgFactory::impl::read_msgsurr(const char *,
                                           std::size_t,
                                           MsgBase* &)
{
    return 0;
}


std::size_t MsgFactory::impl::write_msgsurr(char *,
                                            std::size_t,
                                            const MsgBase &)
{
    return 0;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
std::size_t MsgFactory::read(const char *buf, std::size_t len,
                             MsgBase* &msg)
{
    const std::size_t len_ = len;

    CHECKED(impl::mycmp(impl::Header, buf, len));
    CHECKED(impl::mycmp(impl::ProtocolRevision, buf, len));

    MsgType msgtype;
    CHECKED(impl::myread<char>((char &)msgtype, buf, len));
    if ((size_t) msgtype >= impl::Write.size()) return impl::ErrorValue;

    uint16_t pktlen;
    CHECKED(impl::myread<uint16_t>(pktlen, buf, len));
    if (pktlen > len) return impl::ErrorValue;

    msg = impl::Ctor.at((size_t) msgtype)();
    if (msg == nullptr) return impl::ErrorValue;

    msg_read_t &read = impl::Read.at((size_t) msgtype);
    CHECKED_LEN(len, read(buf, len, msg));

    return len_ - len;
}

std::size_t MsgFactory::write(char *buf, std::size_t len,
                              const MsgBase &msg)
{
    const std::size_t len_ = len;

    CHECKED(impl::mywrite<std::string>(impl::Header, buf, len));
    CHECKED(impl::mywrite<std::string>(impl::ProtocolRevision, buf, len));
    CHECKED(impl::mywrite<char>((char )msg.getType(), buf, len));

    msg_write_t &write = impl::Write.at((size_t) msg.getType());

    char *pktlen = buf;
    const std::size_t lpktlen = len;

    len -= sizeof(uint16_t);
    buf += sizeof(uint16_t);

    CHECKED_LEN(len, write(buf, len, msg));

    len += sizeof(uint16_t);
    impl::mywrite<std::uint16_t>(lpktlen - len, pktlen, len);

    return len_ - len;
}

/* ==================================================================== */
std::size_t MsgFactory::minLength()
{
    std::size_t len = 0;
    len += impl::Header.size() + sizeof(char);
    len += impl::ProtocolRevision.size() + sizeof(char);
    len += sizeof(char);
    len += sizeof(uint16_t);
    return len;
}

/* ==================================================================== */
std::size_t MsgFactory::pktLength(const char *buf)
{
    uint16_t len;
    buf += minLength() - sizeof(uint16_t);
    std::size_t buflen = sizeof(uint16_t);
    impl::myread<uint16_t>(len, buf, buflen);
    return len;
}

}  // namespace icp
