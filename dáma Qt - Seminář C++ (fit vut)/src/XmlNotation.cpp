/*
 * XmlNotation.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "XmlNotation.h"
#include "Log.h"

#include <QtXml>
#include <QFile>
#include <QString>

#include <fstream>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct XmlNotation::impl final: public QXmlDefaultHandler {
    std::string fileName;
    std::vector<Moves> *moves;

    static std::string xmlns;
    static std::string comment;
    static std::string eRoot;
    static std::string eTurn;
    static std::string aWhite;
    static std::string aBlack;

    impl() = default;
    ~impl() = default;

    bool
    startDocument() override;

    bool
    endDocument() override;

    bool startElement(
            const QString &namespaceURI,
            const QString &localName,
            const QString &qName,
            const QXmlAttributes &atts) override;

    bool endElement(
            const QString &namespaceURI,
            const QString &localName,
            const QString &qName) override;

private:
    seqnum_t _seqnum;
};

std::string XmlNotation::impl::xmlns = u8"urn:cz-vutbr-fit:icp:x-dama";
std::string XmlNotation::impl::eRoot = u8"root";
std::string XmlNotation::impl::eTurn = u8"turn";
std::string XmlNotation::impl::aWhite = u8"white";
std::string XmlNotation::impl::aBlack = u8"black";

std::string XmlNotation::impl::comment = u8"\n"
    u8"Project: Navrhn\u011Bte a implementujte aplikaci pro hru D\u00E1ma,\n"
    u8"         projekt pro p\u0159edm\u011Bt ICP\n"
    u8"Date:    2013/03/25\n"
    u8"Team:    Radek \u0160ev\u010D\u00EDk <xsevci44@stud.fit.vutbr.cz>\n"
    u8"         Zbyn\u011Bk Malinkovi\u010D <xmalin22@stud.fit.vutbr.cz>\n";

static QString fromU8Str(const std::string &s)
{
    return QString::fromUtf8(s.c_str(), s.size());
}

/* ==================================================================== */
/*      S  A  X  2     H  A  N  D  L  E  R  S                           */
/* ==================================================================== */
bool XmlNotation::impl::startDocument()
{
    Log::Xml("startDocument()");
    _seqnum = 0;
    return true;
}

bool XmlNotation::impl::endDocument()
{
    Log::Xml("endDocument()");
    moves = nullptr;
    return true;
}

bool XmlNotation::impl::startElement(const QString &namespaceURI,
                                     const QString &,
                                     const QString &qName,
                                     const QXmlAttributes &atts)
{
    Log::Xml("<%s xmlns=\"%s\">",
             qName.toStdString().c_str(),
             namespaceURI.toStdString().c_str());

    if (0 != namespaceURI.compare(fromU8Str(xmlns)))
        return false;

    if (0 == qName.compare(fromU8Str(eRoot))) {
        return true;
    }
    else if (0 == qName.compare(fromU8Str(eTurn))) {
        auto idx_white = atts.index(fromU8Str(aWhite));
        auto idx_black = atts.index(fromU8Str(aBlack));

        Turn t_white, t_black;

        if (-1 != idx_white) {
            t_white = Turn::from_string(atts.value(idx_white).toStdString());
            //if (t_white.position.empty()) return false;
        }
        if (-1 != idx_black) {
            t_black = Turn::from_string(atts.value(idx_black).toStdString());
            //if (t_black.position.empty()) return false;
        }

        moves->emplace_back(Moves { ++_seqnum, t_white, t_black });
        return true;
    }

    return false;
}

bool XmlNotation::impl::endElement(const QString &,
                                   const QString &,
                                   const QString &qName)
{
    Log::Xml("</%s>", qName.toStdString().c_str());
    return true;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
XmlNotation::XmlNotation(std::string fileName)
    : _pImpl(new impl())
{
    _pImpl->fileName = fileName;
}

XmlNotation::~XmlNotation() = default;

bool XmlNotation::write(const std::vector<Moves> &mv)
{
    QDomDocument doc;

    auto proc = doc.createProcessingInstruction(
            u8"xml", u8"version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(proc);

    auto cAuthors = doc.createComment(fromU8Str(impl::comment));
    doc.appendChild(cAuthors), doc.appendChild(doc.createTextNode("\n"));

    auto eRoot = doc.createElementNS(fromU8Str(impl::xmlns),
                                     fromU8Str(impl::eRoot));
    doc.appendChild(eRoot);

    for (auto &i : mv) {
        auto eTurn = doc.createElement(fromU8Str(impl::eTurn));
        eRoot.appendChild(eTurn);

        eTurn.setAttribute(fromU8Str(impl::aWhite),
            QString::fromStdString(i.white_move.to_string()));

        eTurn.setAttribute(fromU8Str(impl::aBlack),
            QString::fromStdString(i.black_move.to_string()));
    }

    QByteArray raw = doc.toByteArray(4);

    std::ofstream f(_pImpl->fileName,
                    std::ios_base::binary |
                    std::ios_base::trunc);
    f.write(raw.data(), raw.count());

    return true;
}

bool XmlNotation::read(std::vector<Moves> &mv)
{
    QFile f(QString::fromStdString(_pImpl->fileName));
    //if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QXmlInputSource srcf(&f);

    QXmlSimpleReader rdr;
    rdr.setContentHandler(_pImpl.get());

    _pImpl->moves = &mv;
    if (!rdr.parse(srcf)) return false;

    return true;
}

}
