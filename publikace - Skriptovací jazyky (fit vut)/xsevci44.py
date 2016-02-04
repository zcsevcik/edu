#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ====================================================================== #
# ISJ: Převod webových stránek s publikacemi do formátu BibTeXML         #
#                                                                        #
# Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>              #
# Date:           2014/04/27                                             #
# ====================================================================== #

import re
import codecs
import StringIO
from lxml import etree

import urllib2
import bibtex2xml
import scholar

import sys
reload(sys)
sys.setdefaultencoding('UTF-8')

# ====================================================================== #
def xform_fromurl(url, xslt):
    dom = etree.parse(url, etree.HTMLParser())
    xslt = etree.parse(xslt)
    xform = etree.XSLT(xslt)
    result = xform(dom)
    return str(result)

# ====================================================================== #
def tobibtex(instr):
    pubs = re.split("\n\n", instr)
    bib_rex = re.compile('^\[bib\](.*)\[/bib\]', flags = re.M|re.S)
    return [ bib_rex.match(l).group(1) for l in pubs if l ]

# ====================================================================== #
def find_scholar(title, author):
    if isinstance(title, unicode):
        title = title.encode('UTF-8')

    if isinstance(author, unicode):
        author = author.encode('UTF-8')
    
    settings = scholar.ScholarSettings()
    settings.set_citation_format(scholar.ScholarSettings.CITFORM_BIBTEX)

    querier = scholar.ScholarQuerier()
    querier.apply_settings(settings)

    query = scholar.SearchScholarQuery()
    query.set_phrase(title)
    query.set_author(author)
#     query.set_scope(True)
    query.set_num_page_results(1)

    querier.send_query(query)
    if (querier.articles):
        return querier.articles[0].as_citation()

    return None

# ====================================================================== #
def pubs_toxml(bib_items):
    stdout_ = sys.stdout
    sys.stdout = myio = StringIO.StringIO()
    bibtex2xml.contentshandler(bib_items)
    sys.stdout = stdout_
    result = myio.getvalue()
    myio.close()
    
    if isinstance(result, unicode):
        result = result.encode('UTF-8')
        
    return result

# ====================================================================== #
def reformat_authors(s_original):
    l = re.sub(',? and ', ', ', s_original)
    l = re.sub(',? & ', ', ', l)
    l = re.sub('; ', ', ', l)
    l = l.strip(',')
    l = l.split(', ')
    
    prepend = False
    names = []
    for (i, v) in enumerate(l):
        if not prepend:
            names.append(v)
            prepend = " " not in v
        else:
            names[-1] = names[-1] + ", " + v
            prepend = False

    return ' and '.join(names).strip()

# ====================================================================== #
# -- data fields --          # -- type --
# address                    # article
# author                     # book
# booktitle                  # booklet
# chapter                    # manual
# edition                    # techreport
# editor                     # mastersthesis
# howpublished               # phdthesis
# institution                # inbook
# journal                    # incollection
# month                      # proceedings
# note                       # inproceedings
# number                     # conference
# organization               # unpublished
# pages                      # misc
# publisher
# school
# series
# title
# type
# volume
# year
# ====================================================================== #
def bib(type, id, data):
    s = "@" + type + "{" + id
    for (k, v) in data.items():
        s = s + "," + k + "={" + v + "}"
    
    s = s + "}"
    return s

# ====================================================================== #
def hermansky():
    instr = xform_fromurl(
            "http://www.clsp.jhu.edu/publications/author/hynek-hermansky/",
            "hermansky.xslt")
    return pubs_toxml(tobibtex(instr))

# ====================================================================== #
def assaleh():
    instr = xform_fromurl(
            "http://tony.abou-assaleh.net/Publications",
            "assaleh.xslt")
    return pubs_toxml(tobibtex(instr))

# ====================================================================== #
def zendulka():
    instr = xform_fromurl(
            "http://www.fit.vutbr.cz/~zendulka/pubs.php.en",
            "zendulka.xslt")
    pubs = re.split("\n\n", instr)
    bib_rex = re.compile('^\[url\]/~zendulka/pubs.php.en\?id=(.*)\[/url\]')

    id_items = [ bib_rex.match(l).group(1) for l in pubs if l ]
    url_items = [ "http://www.fit.vutbr.cz/~zendulka/pubs.php.en?id=" + l
                  for l in id_items ]
    bib_items = [ xform_fromurl(l, "zendulka-detail.xslt")
                  for l in url_items ]

    for (i, l) in enumerate(bib_items):
        bib_items[i] = l.replace("\n", "zendulka-"+id_items[i]+",\n", 1)

    return pubs_toxml(bib_items)

# ====================================================================== #
def alegria():
    instr = xform_fromurl(
            "http://ixa.si.ehu.es/Ixa/Argitalpenak/kidearen_argitalpenak?kidea=1000808989",
            "alegria.xslt")
    pubs = re.split("\n", instr)

    scholar_rex = re.compile('^\[scholar\].*allintitle%3A([^&]+).*$')
    bib_rex = re.compile('^\[bib\](.*)$')

    scholar_url = [ l.group(1) for l in 
                        (scholar_rex.match(url) for url in pubs)
                    if l ]
    bib_url = [ l.group(1) for l in
                    (bib_rex.match(url) for url in pubs)
                if l ]

    scholar_items = []
    bib_items = [ xform_fromurl(l, "alegria-detail.xslt") for l in bib_url ]

    for (i, l) in enumerate(bib_items):
        bib_items[i] = l.replace("{", "{alegria-id"+str(i)+",\n", 1)

    settings = scholar.ScholarSettings()
    settings.set_citation_format(scholar.ScholarSettings.CITFORM_BIBTEX)

    querier = scholar.ScholarQuerier()
    querier.apply_settings(settings)

    for l in scholar_url:
        query = scholar.SearchScholarQuery()
        query.set_phrase(l)
        query.set_scope(True)
        query.set_num_page_results(1)

        querier.send_query(query)
        for art in querier.articles:
            scholar_items.append(art.as_citation())

    return pubs_toxml(scholar_items + bib_items)

# ====================================================================== #
def adamic():
    instr = xform_fromurl(
            "http://ladamic.com/",
            "adamic.xslt")
    pubs_ = [ l.strip() for l in (re.split("\n\n", instr)) ]
    pubs_1 = [ l for l in pubs_ if l and "\n" not in l ]
    pubs_2 = [ l for l in pubs_ if l and "\n" in l ]
    
    pubs = []
    
    author_pubs_1_rex = re.compile('^(.*),.*$')
    for (i, l) in enumerate(pubs_1):
        title_rest = re.split("\.[,.]", l)
        authors_rest_delim = title_rest[1].rindex(',')
        
        title = title_rest[0].strip()
        authors = title_rest[1][:authors_rest_delim].strip()
        
        scholar_lst = find_scholar(title, 'Adamic')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue
        
        pubs.append(bib("misc", "adamic-1-"+str(i), {
             'author': reformat_authors(authors),
             'title': title,
        }))

    year_2_rex = re.compile('\((\d{4})\)')
    for (i, l) in enumerate(pubs_2):
        title_author_rest = re.split("\n", l)
        
        title = title_author_rest[0].strip()
        title = title.rstrip(',')
        authors = title_author_rest[1].strip()
        
        year = ""
        year_m = year_2_rex.search(title_author_rest[2])
        if year_m: year = year_m.group(1)

        scholar_lst = find_scholar(title, 'Adamic')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue

        pubs.append(bib("misc", "adamic-2-"+str(i), {
            'title': title,
            'author': reformat_authors(authors),
            'year': year,
        }))
    
    return pubs_toxml(pubs)

# ====================================================================== #
def adar():
    instr = xform_fromurl(
            "http://cond.org/",
            "adar.xslt")
    pubs_ = [ l for l in re.split("\n\n", instr) if l ]
    pubs = []
    
    title_rest_rex = re.compile('^"(.*)"(.*)$')
    year_rex = re.compile('(\d{4})')
    for (i, l) in enumerate(pubs_):
        if not l.startswith('"'): break
        
        title = title_rest_rex.match(l).group(1)
        
        scholar_lst = find_scholar(title, 'Adar')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue
        
        authors = title_rest_rex.match(l).group(2)
        authors = reformat_authors(authors)

        year = ""
        year_m = year_rex.search(l)
        if year_m: year = year_m.group(1)
        
        pubs.append(bib("misc", "adar-"+str(i), {
            'title': title,
            'year': year,
            'author': authors,
        }))
        
    return pubs_toxml(pubs)

# ====================================================================== #
def alexandersson():
    instr = xform_fromurl(
            "http://www.dfki.de/~janal/publications.html",
            "alexandersson.xslt")
    pubs = []

    def get_xpath_text(child, x):
        t = child.xpath(x + '/text()')
        return t and t[0] or ''
    
    def type_mapping(x):
        if x=='Thesis': return 'masterthesis'
        if x=='Articles': return 'article'
        if x=='Book-Chapters': return 'inbook'
        if x=='Conference-Papers': return 'inproceedings'
        if x=='Workshop-Papers': return 'inproceedings'
        return 'misc'

    root = etree.fromstring(instr)
    for (i, child) in enumerate(root.xpath("/Alexandersson/*[Author!='' and Title!='']")):
        title = get_xpath_text(child, 'Title')
        
        scholar_lst = find_scholar(title, 'Alexandersson')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue
        
        pubs.append(bib(type_mapping(child.tag),
                        "alexandersson-"+str(i), {
            'author': reformat_authors(get_xpath_text(child, 'Author')),
            'title': get_xpath_text(child, 'Title'),
        }))

    return pubs_toxml(pubs)

# ====================================================================== #
def allen():
    instr = xform_fromurl(
            "http://www.cs.rochester.edu/u/james/",
            "allen.xslt")
    pubs_ = [ l for l in re.split("\n\n", instr) if l ]
    pubs = []
    
#     '""(.*)"|"(.*)"|``(.*)\'\'|\(\d{4}\)\.\w+(.*)\.'
#     title_rex = re.compile('"([^"]+)' + '|' + "``([^']+)''", re.MULTILINE)
    title_1_rex = re.compile('"([^"]+)', re.MULTILINE)
    title_2_rex = re.compile("``([^']+)''", re.MULTILINE)
    title_3_rex = re.compile('\(\d{4}\)\. ([^.]+)', re.MULTILINE)

    for l in pubs_:
        title_m = None
        
        title_m = title_m or title_1_rex.search(l)
        title_m = title_m or title_2_rex.search(l)
        title_m = title_m or title_3_rex.search(l)
        
        if not title_m: continue
        title = title_m.group(1).replace('\n', ' ').strip()
        
        scholar_lst = find_scholar(title, 'Allen')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue
       
    return pubs_toxml(pubs)

# ====================================================================== #
def alonso():
    instr = xform_fromurl(
            "http://www.dc.fi.udc.es/~alonso/papers.html",
            "alonso.xslt")
    pubs_ = [ l for l in re.split("\n\n", instr) if l ]
    pubs = []
    
    bib_rex = re.compile('\[bib\](.*)\[/bib\]')
    title_rex = re.compile('"(.*)"')
    
    for l in pubs_:
        bib_m = bib_rex.search(l, re.MULTILINE)
        if bib_m:
            try:
                bib = urllib2.urlopen(bib_m.group(1)).read()
                pubs.append(bib)
                continue
            except:
                None
        
        title_m = title_rex.search(l, re.MULTILINE)
        if not title_m: continue
        
        scholar_lst = find_scholar(title_m.group(1).strip(), 'Alonso')
        if scholar_lst: 
            pubs.append(scholar_lst)
            continue
        
    return pubs_toxml(pubs)

# ====================================================================== #
def main():
    if len(sys.argv) < 2: sys.exit(
"""Usage: %s author

author ::= adamic | adar | alegria | alexandersson | allen | alonso | assaleh |
           hermansky | zendulka
""" % sys.argv[0])

    known = {
        'adamic': adamic,
        'adar': adar,
        'alegria': alegria,
        'alexandersson': alexandersson,
        'allen': allen,
        'alonso': alonso,
        'assaleh': assaleh,
        'hermansky': hermansky,
        'zendulka': zendulka,
    }

    if (known.has_key(sys.argv[1])):
        print(known[sys.argv[1]]())
        return 0
    
    sys.exit("Unknown argument")
    return 1

# ====================================================================== #
if __name__ == "__main__":
    sys.exit(main())

# ====================================================================== #
