#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#SYN:xsevci44

# ===================================================================
# Soubor:  syn.py 
# Datum:   2012/04/10
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Zvyrazneni syntaxe v Python 3, uloha c. 2 pro predmet IPP
# Popis:   Zvyrazneni textu podle formatovaciho souboru.
# ===================================================================

import sys;
import getopt;
import re;

import string;
from collections import deque;

## Vypise pouziti.
def usage():
    print( \
"""Zvyrazneni syntaxe v Python 3, uloha c. 2 pro predmet IPP
Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz

Pouziti: syn [--br] --format=filename --input=filename --output=filename

Parametry:
    --help\t\tVypise tuto napovedu.
    --format=filename\tFormatovaci soubor.
    --input=filename\tVstupni soubor v kodovani UTF-8.
    --output=filename\tVystupni html soubor v kodovani UTF-8.
    --br\t\tPrida odradkovani <br /> na konec kazdeho radku.
""", file=sys.stdout);

## Chybove stavy.
ESUCCESS = 0;   ## Bez chyby.
EPARAMS = 1;    ## Chyba v parametrech.
EFILEIN = 2;    ## Chyba v otevreni vstupniho/formatovaciho souboru.
EFILEOUT = 3;   ## Chyba v otevreni vystupniho souboru.
EBADFORMAT = 4; ## Spatny format ve formatovacim souboru.

## Formatovaci zaznam.
class FormatItem(object):
    def __init__(self, expr):
        self._expr = expr;
        self._regexp = "";
        self._regexp_prog = None;
        self._html_l = deque();
        self._html_r = deque();

    @property
    def expr(self):
        ## Regularni vyraz.
        return self._expr;

    @expr.setter
    def expr(self, value):
        self._expr = value;

    @property
    def regexp(self):
        ## Transformovany regularni vyraz.
        return self._regexp;

    @regexp.setter
    def regexp(self, value):
        self._regexp = value;

    @property
    def regexp_prog(self):
        ## Zkompilovany refularni vyraz.
        return self._regexp_prog;

    @regexp_prog.setter
    def regexp_prog(self, value):
        self._regexp_prog = value;

    @property
    def html_l(self):
        ## HTML tagy aplikovane pred vyrazem.
        return self._html_l;

    @html_l.setter
    def html_l(self, value):
        self._html_l = value;

    @property
    def html_r(self):
        ## HTML tagy aplikovane za vyrazem.
        return self._html_r;

    @html_r.setter
    def html_r(self, value):
        self._html_r = value;

## Zpracuje regularni vyraz ze souboru a prevede ho na
#  pouzitelny regularni vyraz jazyka
#  @param expr Regularni vyraz ze souboru
#  @return Knihovnou srozumitelny regularni vyraz
def parse_expr(expr):
    if expr == None:
        return None;

    if re.search('\.{2,}', expr) != None:      # A..B
        return None;

    # escapnuti knihovnich spec. znaku
    expr = re.sub(r'\\', r'\\\\', expr);
    expr = re.sub(r'\?', r'\?', expr);
    expr = re.sub(r'\^', r'\^', expr);
    expr = re.sub(r'\$', r'\$', expr);
    expr = re.sub(r'\{', r'\{', expr);
    expr = re.sub(r'\}', r'\}', expr);
    expr = re.sub(r'\[', r'\[', expr);
    expr = re.sub(r'\]', r'\]', expr);

    # prevod vyrazu
    expr = re.sub(r'([^%])\.', r'\1', expr);   # A.B
    expr = re.sub(r'!%a', r'', expr);          # !%a
    expr = re.sub(r'!(%?.)', r'[^\1]', expr);  # !A
    expr = re.sub(r'^(%?\w)\*$', r'\1+', expr); # pro jednoduche vyrazy typu f*
                                                # nebo %a* nahradi * za +

    # specialni vyrazy
    expr = re.sub(r'%s', r'\s', expr);
    expr = re.sub(r'%a', r'.', expr);
    expr = re.sub(r'%d', r'\d', expr);
    expr = re.sub(r'%l', r'[a-z]', expr);
    expr = re.sub(r'%L', r'[A-Z]', expr);
    expr = re.sub(r'%w', r'[a-zA-Z]', expr);
    expr = re.sub(r'%W', r'\w', expr);
    expr = re.sub(r'%t', r'\t', expr);
    expr = re.sub(r'%n', r'\n', expr);

    # specialni symboly
    expr = re.sub(r'%\.', r'\.', expr);
    expr = re.sub(r'%\|', r'\|', expr);
    expr = re.sub(r'%\*', r'\*', expr);
    expr = re.sub(r'%\+', r'\+', expr);
    expr = re.sub(r'%\(', r'\(', expr);
    expr = re.sub(r'%\)', r'\)', expr);
    expr = re.sub(r'%!',  r'!',  expr);
    expr = re.sub(r'%%',  r'%',  expr);

    return expr;

## Zpracuje jeden formatovaci parametr a prida odpovidajici
#  html tag do polozky FormatItem.
#  @param item Polozka typu FormatItem.
#  @param key Klic formatovaciho prikazu.
#  @param value Hodnota formatovaciho prikazu.
#  @return True byla-li polozka pridana; jinak False
def parse_fmt(item, key, value):
    if not isinstance(item, FormatItem):
        return False;
    if key == None:
        return False;
    if value == None:
        return False;

    key = key.lower();
    html_l = "";
    html_r = "";

    if key == "bold":
        html_l = "<b>";
        html_r = "</b>";
    elif key == "italic":
        html_l = "<i>";
        html_r = "</i>";
    elif key == "underline":
        html_l = "<u>";
        html_r = "</u>";
    elif key == "teletype":
        html_l = "<tt>";
        html_r = "</tt>";
    elif key == "size":
        # kontrola na rozsah hodnoty 1..7
        if not value.isdigit() or \
           int(value) not in range(1, 8): return False;

        html_l = "<font size=" + value + ">";
        html_r = "</font>";
    elif key == "color":
        # kontrola na hexa znaky ve formatu RRGGBB
        if len(value) != 6: return False;
        for i in value:
            if i not in string.hexdigits: return False;

        html_l = "<font color=#" + value.upper() + ">";
        html_r = "</font>";
    else:
        return False;

    # posledni pridany tag musi byt ukoncen jako prvni
    item.html_l.append(html_l);
    item.html_r.appendleft(html_r);
    return True;

## Nacte data z formatovaciho souboru.
#
# Je-li fd=None, vrati prazdne pole.
# @param fd Deskriptor formatovaciho souboru.
# @return Pole formatovaci struktury.
def parse_fmtfile(fd):
    if fd == None:
        return list();

    fmt = list();
    lineno = 0;

    # projdeme postupne vsechny radky
    while True:
        line = fd.readline();
        lineno += 1;
        if not line:
            break

        iKey = 0;
        iTail = 0;
        key = "";
        value = "";

        # vynechame mezery na zacatku radku
        while iKey < len(line) and line[iKey].isspace():
            iKey += 1;

        iTail = iKey;

        # radek obsahuje pouze mezery, preskocime ho
        if iKey >= len(line):
            continue;

        # cteme do prvniho tabulatoru
        while line[iTail].isprintable() and line[iTail] != '\t':
            iTail += 1;

        # vytvorime objekt a dame ho do listu
        key = line[iKey:iTail];
        regexp = parse_expr(key);

        if (regexp is None):
            print("Chyba ve formatovacim souboru: chybny vyraz '" + key + "' " \
                  "na radku " + str(lineno), file=sys.stderr);
            sys.exit(EBADFORMAT);

        item = FormatItem(key);
        item.regexp = regexp;
        fmt.append(item);

        # preskocime vsechny mezery pred prikazy
        iKey = iTail;
        while iKey < len(line) and line[iKey].isspace():
            iKey += 1;

        # vynechame mezery od konce radku
        iTail = len(line);
        while line[iTail - 1].isspace():
            iTail -= 1;

        # kontrola jestli mame nejake prikazy
        if iKey >= iTail:
            print("Chyba ve formatovacim souboru: chybi prikazy " \
                  "na radku " + str(lineno), file=sys.stderr);
            sys.exit(EBADFORMAT);

        # rozdeleni a zpracovani prikazu
        stmts = line[iKey:iTail].split(',');

        for stmt_ in stmts:
            stmt = stmt_.partition(':');
            key = stmt[0].strip();
            value = stmt[2].strip();
            if not parse_fmt(item, key, value):
                print("Chyba ve formatovacim souboru: neznamy klic nebo " \
                      "neplatna hodnota '" + stmt_ + "' " \
                      "na radku " + str(lineno), file=sys.stderr);
                sys.exit(EBADFORMAT);
        
        try:
            item.regexp_prog = re.compile(item.regexp, re.DOTALL);
        except re.error as e:
            print("Chyba ve zpracovani formatovaciho souboru: " + str(e),
                  file=sys.stderr);
            sys.exit(EBADFORMAT);

    return fmt;

def process(fdin, fdout, fmt):
    assert fdin != None;
    assert fdout != None;
    assert type(fmt) is list;

    indices = [];
    indices_l = [];
    indices_r = [];
    text = fdin.read();

    # najde indexy, kam se maji vlozit tagy
    for i in fmt:
        #print(str(i.html_l) + " ('" + i.expr + "' => '" + i.regexp + "') " + str(i.html_r));
        for match in re.finditer(i.regexp_prog, text):
            indices_l.append((match.start(0), i.html_l));
            indices_r.append((match.end(0), i.html_r));

    # seradi podle indexu, musime pocitat i s poradim ve fmt. souboru
    # tzn. stejne indexy zprava musime vzit pozpatku
    sort_by_index = lambda index: index[0];
    indices_l.sort(key = sort_by_index);
    indices_r.sort(key = sort_by_index);
    indices_r.reverse();

    indices = indices_r + indices_l;
    indices.sort(key = sort_by_index);
    
    if len(indices) > 1:
        idx_last = 0;
        for i in indices:
            idx = i[0];

            # zapiseme text pred novym tagem (tzn. zacatek pred 1. tagem
            # a text mezi spolu nesouvisejicimi tagy)
            fdout.write(text[idx_last:idx]);

            # zapsat tagy
            for tag in i[1]:
                fdout.write(tag);

            idx_last = idx;

        # zapise zbytek za poslednim tagem
        fdout.write(text[indices[-1][0]:]);
    else:
        # zadne tagy se nebudou vkladat, zapsat vstup tak jak je
        fdout.write(text);

    return;

## Funkce main.
def main(argv = None):
    if argv is None:
        argv = sys.argv;

    # zpracovani parametru prikazove radky
    myin = None;
    myout = None;
    myfmt = None;
    br = False;

    try:
        opts, args = getopt.getopt(argv[1:], "h", \
            ["help", "input=", "output=", "format=", "br"]);
    except getopt.GetoptError as e:
        print(e, file=sys.stderr);
        return EPARAMS;

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage();
            return ESUCCESS;
        elif opt == "--input":
            myin = arg;
        elif opt == "--output":
            myout = arg;
        elif opt == "--format":
            myfmt = arg;
        elif opt == "--br":
            br = True;
        else:
            return EPARAMS;

    # zpracovani formatovaciho souboru
    fdfmt = None;
    if myfmt != None:
        try:
            fdfmt = open(myfmt, 'r', encoding="utf-8");
        except IOError as e:
            print(e, file=sys.stderr);
            return EFILEIN;

    fmt = parse_fmtfile(fdfmt);

    if myfmt != None:
        fdfmt.close();

    if br:
        brItem = FormatItem('%n');
        brItem.regexp = "\n";
        brItem.regexp_prog = re.compile(brItem.regexp, re.DOTALL);
        brItem.html_l.append("<br />");
        fmt.append(brItem);

    # otevreni souboru
    fdin = None;
    fdout = None;
    
    if myin != None:
        try:
            fdin = open(myin, 'r', encoding="utf-8");
        except IOError as e:
            print(e, file=sys.stderr);
            return EFILEIN;
    else:
        fdin = sys.stdin;

    if myout != None:
        try:
            fdout = open(myout, 'w', encoding="utf-8");
        except IOError as e:
            print(e, file=sys.stderr);
            return EFILEOUT;
    else:
        fdout = sys.stdout;

    # zpracovani vstupu
    process(fdin, fdout, fmt);

    # uzavreni souboru
    if myout != None:
        fdout.close();

    if myin != None:
        fdin.close();

    return ESUCCESS;

if __name__ == "__main__":
    sys.exit(main());

