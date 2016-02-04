/*
 * scanner.c:      Implementace interpretu imperativniho jazyka IFJ12
 *
 * Team:           Daniel Berek <xberek00@stud.fit.vutbr.cz>
 *                 Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>
 *                 Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>
 *                 Peter Pritel <xprite01@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ifj12_b2i.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "defs.h"
#include "scanner.h"
#include "str.h"

#define FSM
#define STATE(x) s_##x:
#define NEXTSTATE(x) goto s_##x

#define SRC_GET(c) (_col++, (c) = fgetc(src_f))
#define SRC_UNGET(c) (_col--, ungetc((c), src_f))

#define CHECKED(x, s) do { \
    if (!(x)) goto s; \
} while (0)

#define HEX_TO_INT(c) (isdigit(c) ? ((c) - '0') : (tolower(c) - 'a' + 0x0A))

static size_t _line = 0;
static size_t _col = 0;

size_t getCurrCol()
{
    return _col;
}

size_t getCurrLine()
{
    return _line + 1;
}

int getNextToken(void **attr)
{
    string_t *s = NULL;
    keywords_t kw;

    int c;

    FSM {
        // ---------------------------------------------------------------
        // initial state
        // ---------------------------------------------------------------
        STATE(S_INIT) {
            SRC_GET(c);

            if (c == EOF) {
                NEXTSTATE(S_EOF);
            }
            else if (c == '\n') {
                NEXTSTATE(S_EOL);
            }
            else if (isspace(c)) {
                NEXTSTATE(S_INIT);
            }
            else if (isdigit(c)) {
                CHECKED(s = string_new(), alloc_failed);
                NEXTSTATE(S_INTEGRAL);
            }
            else if (isalpha_l(c, c_locale) || c == '_') {
                CHECKED(s = string_new(), alloc_failed);
                NEXTSTATE(S_IDENTIFIER);
            }
            else {
                switch (c) {
                default:
                    NEXTSTATE(S_ERROR);
                case '+':
                    NEXTSTATE(S_PLUS);
                case '-':
                    NEXTSTATE(S_MINUS);
                case '!':
                    NEXTSTATE(S_EXCL);
                case '=':
                    NEXTSTATE(S_EQUAL);
                case '*':
                    NEXTSTATE(S_ASTERISK);
                case '/':
                    NEXTSTATE(S_SLASH);
                case '"':
                    CHECKED(s = string_new(), alloc_failed);
                    NEXTSTATE(S_QUOT);
                case '(':
                    NEXTSTATE(S_LPAR);
                case ')':
                    NEXTSTATE(S_RPAR);
                case '[':
                    NEXTSTATE(S_LBRACK);
                case ']':
                    NEXTSTATE(S_RBRACK);
                case '<':
                    NEXTSTATE(S_LCHEV);
                case '>':
                    NEXTSTATE(S_RCHEV);
                case ':':
                    NEXTSTATE(S_COLON);
                case ',':
                    NEXTSTATE(S_COMMA);
                }
            }
        }

        // ---------------------------------------------------------------
        // character +
        // ---------------------------------------------------------------
        STATE(S_PLUS) {
            _ifj_lexical_log("TOK_OP_SUM");
            return TOK_OP_SUM;
        }

        // ---------------------------------------------------------------
        // character -
        // ---------------------------------------------------------------
        STATE(S_MINUS) {
            _ifj_lexical_log("TOK_OP_SUB");
            return TOK_OP_SUB;
        }

        // ---------------------------------------------------------------
        // character !
        // ---------------------------------------------------------------
        STATE(S_EXCL) {
            if (SRC_GET(c) == '=') {
                _ifj_lexical_log("TOK_OP_NEQ");
                return TOK_OP_NEQ;
            }
            else {
                SRC_UNGET(c);
                NEXTSTATE(S_ERROR);
            }
        }

        // ---------------------------------------------------------------
        // character =
        // ---------------------------------------------------------------
        STATE(S_EQUAL) {
            if (SRC_GET(c) == '=') {
                _ifj_lexical_log("TOK_OP_EQ");
                return TOK_OP_EQ;
            }
            else {
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_OP_ASSIGN");
                return TOK_OP_ASSIGN;
            }
        }

        // ---------------------------------------------------------------
        // character *
        // ---------------------------------------------------------------
        STATE(S_ASTERISK) {
            if (SRC_GET(c) == '*') {
                _ifj_lexical_log("TOK_OP_POW");
                return TOK_OP_POW;
            }
            else {
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_OP_MULT");
                return TOK_OP_MULT;
            }
        }

        // ---------------------------------------------------------------
        // character (
        // ---------------------------------------------------------------
        STATE(S_LPAR) {
            _ifj_lexical_log("TOK_LPAR");
            return TOK_LPAR;
        }

        // ---------------------------------------------------------------
        // character )
        // ---------------------------------------------------------------
        STATE(S_RPAR) {
            _ifj_lexical_log("TOK_RPAR");
            return TOK_RPAR;
        }

        // ---------------------------------------------------------------
        // character [
        // ---------------------------------------------------------------
        STATE(S_LBRACK) {
            _ifj_lexical_log("TOK_LBRACK");
            return TOK_LBRACK;
        }

        // ---------------------------------------------------------------
        // character ]
        // ---------------------------------------------------------------
        STATE(S_RBRACK) {
            _ifj_lexical_log("TOK_RBRACK");
            return TOK_RBRACK;
        }

        // ---------------------------------------------------------------
        // character <
        // ---------------------------------------------------------------
        STATE(S_LCHEV) {
            if (SRC_GET(c) == '=') {
                _ifj_lexical_log("TOK_OP_LE");
                return TOK_OP_LE;
            }
            else {
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_OP_LT");
                return TOK_OP_LT;
            }
        }

        // ---------------------------------------------------------------
        // character >
        // ---------------------------------------------------------------
        STATE(S_RCHEV) {
            if (SRC_GET(c) == '=') {
                _ifj_lexical_log("TOK_OP_GE");
                return TOK_OP_GE;
            }
            else {
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_OP_GT");
                return TOK_OP_GT;
            }
        }

        // ---------------------------------------------------------------
        // character :
        // ---------------------------------------------------------------
        STATE(S_COLON) {
            _ifj_lexical_log("TOK_COLON");
            return TOK_COLON;
        }

        // ---------------------------------------------------------------
        // character ,
        // ---------------------------------------------------------------
        STATE(S_COMMA) {
            _ifj_lexical_log("TOK_COMMA");
            return TOK_COMMA;
        }

        // ---------------------------------------------------------------
        // character /
        // ---------------------------------------------------------------
        STATE(S_SLASH) {
            switch (SRC_GET(c)) {
            case '/':
                NEXTSTATE(S_COMMENT);

            case '*':
                NEXTSTATE(S_COMMENT_BLOCK);

            default:
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_OP_DIV");
                return TOK_OP_DIV;
            }
        }

        // ---------------------------------------------------------------
        // string -- inside quotes
        // ---------------------------------------------------------------
        STATE(S_QUOT) {
            SRC_GET(c);
            if (c == '"') {
                NEXTSTATE(S_STRING);
            }
            else if (c == '\\') {
                NEXTSTATE(S_ESCAPECHAR);
            }
            else if (isprint(c)) {
                CHECKED(string_append(&s, c), alloc_failed);
                NEXTSTATE(S_QUOT);
            }
            else {
                NEXTSTATE(S_ESTRING);
            }
        }

        // ---------------------------------------------------------------
        // string -- escape characters
        // ---------------------------------------------------------------
        STATE(S_ESCAPECHAR) {
            switch (SRC_GET(c)) {
            case 'n':
                CHECKED(string_append(&s, '\n'), alloc_failed);
                NEXTSTATE(S_QUOT);

            case 't':
                CHECKED(string_append(&s, '\t'), alloc_failed);
                NEXTSTATE(S_QUOT);

            case '"':
                CHECKED(string_append(&s, '"'), alloc_failed);
                NEXTSTATE(S_QUOT);

            case '\\':
                CHECKED(string_append(&s, '\\'), alloc_failed);
                NEXTSTATE(S_QUOT);

            case 'x':
                NEXTSTATE(S_HEXCHAR);

            default:
                NEXTSTATE(S_ESTRING);
            }
        }

        // ---------------------------------------------------------------
        // string -- character in hexadecimal notation
        // ---------------------------------------------------------------
        STATE(S_HEXCHAR) {
            unsigned char val = 0;

            if (isxdigit(SRC_GET(c))) {
                val = HEX_TO_INT(c) << 4;
            }
            else {
                NEXTSTATE(S_ESTRING);
            }

            if (isxdigit(SRC_GET(c))) {
                val += HEX_TO_INT(c);
            }
            else {
                NEXTSTATE(S_ESTRING);
            }

            CHECKED(string_append(&s, val), alloc_failed);
            NEXTSTATE(S_QUOT);
        }

        // ---------------------------------------------------------------
        // numeric -- integral part
        // ---------------------------------------------------------------
        STATE(S_INTEGRAL) {
            CHECKED(string_append(&s, c), alloc_failed);

            SRC_GET(c);
            if (isdigit(c)) {
                NEXTSTATE(S_INTEGRAL);
            }
            else if (c == '.') {
                CHECKED(string_append(&s, c), alloc_failed);
                SRC_GET(c);

                if (isdigit(c)) {
                    NEXTSTATE(S_DECIMAL);
                }
                else {
                    NEXTSTATE(S_ENUM);
                }
            }
            else if (c == 'e') {
                CHECKED(string_append(&s, c), alloc_failed);
                NEXTSTATE(S_EXPONENT_E);
            }
            else {
                NEXTSTATE(S_ENUM);
            }
        }

        // ---------------------------------------------------------------
        // numeric -- decimal part
        // ---------------------------------------------------------------
        STATE(S_DECIMAL) {
            CHECKED(string_append(&s, c), alloc_failed);

            SRC_GET(c);
            if (isdigit(c)) {
                NEXTSTATE(S_DECIMAL);
            }
            else if (c == 'e') {
                CHECKED(string_append(&s, c), alloc_failed);
                NEXTSTATE(S_EXPONENT_E);
            }
            else {
                SRC_UNGET(c);
                NEXTSTATE(S_NUMERIC);
            }
        }

        // ---------------------------------------------------------------
        // numeric -- exponent part
        // ---------------------------------------------------------------
        STATE(S_EXPONENT_E) {
            SRC_GET(c);
            if (c == '+' || c == '-') {
                CHECKED(string_append(&s, c), alloc_failed);
                SRC_GET(c);
            }

            if (isdigit(c)) {
                NEXTSTATE(S_EXPONENT);
            }
            else {
                NEXTSTATE(S_ENUM);
            }
        }

        STATE(S_EXPONENT) {
            CHECKED(string_append(&s, c), alloc_failed);

            if (isdigit(SRC_GET(c))) {
                NEXTSTATE(S_EXPONENT);
            }
            else {
                SRC_UNGET(c);
                NEXTSTATE(S_NUMERIC);
            }
        }

        // ---------------------------------------------------------------
        // identifier
        // ---------------------------------------------------------------
        STATE(S_IDENTIFIER) {
            CHECKED(string_append(&s, c), alloc_failed);
            if (isalnum_l(SRC_GET(c), c_locale) || c == '_') {
                NEXTSTATE(S_IDENTIFIER);
            }

            SRC_UNGET(c);

#define X_(pstate, str, e) \
    if (0 == strcmp(string_getstr(s), (str))) { \
        kw = e; \
        string_dispose(s); s = NULL; \
        NEXTSTATE(pstate); \
    } else

#define X(a,b) X_(S_KEYWORD, b, KW_##a)
#include "keywords.def"
            X_KEYWORD
#undef X
#define X(a,b) X_(S_BUILTIN, b, FN_##a)
#include "builtin.def"
                //X_BUILTIN_FN
#undef X
#define X(a,b) X_(S_RESERVED_KEYWORD, b, RKW_##a)
#include "keywords.def"
                X_KEYWORD_RESERVED
#undef X
#undef X_
                // -------------------------------------------------------
                // identifier
                // -------------------------------------------------------
            {                   // else
                *attr = s;
                _ifj_lexical_log("TOK_TYPE_ID \"%s\"", string_getstr(s));
                return TOK_TYPE_ID;
            }
        }

        // ---------------------------------------------------------------
        // reserved keyword
        // ---------------------------------------------------------------
        STATE(S_RESERVED_KEYWORD) {
            switch (kw) {
            default:
                break;

#define X(a,b) \
            case (RKW_##a): \
                _ifj_lexical_log("TOK_TYPE_RKW \"%s\"", (b)); \
                break;
#include "keywords.def"
                X_KEYWORD_RESERVED
#undef X
            }

            *attr = (void *) kw;
            return TOK_TYPE_RKW;
        }

        // ---------------------------------------------------------------
        // built-in function
        // ---------------------------------------------------------------
#if 0
        STATE(S_BUILTIN) {
            switch (kw) {
            default:
                break;

#define X(a,b) \
            case (FN_##a): \
                _ifj_lexical_log("TOK_TYPE_FN \"%s\"", (b)); \
                break;
#include "builtin.def"
                //X_BUILTIN_FN
#undef X
            }

            *attr = (void *) kw;
            return TOK_TYPE_FN;
        }
#endif

        // ---------------------------------------------------------------
        // term -- numeric
        // ---------------------------------------------------------------
        STATE(S_NUMERIC) {
            *attr = s;
            _ifj_lexical_log("TOK_TYPE_NUM \"%s\"", string_getstr(s));
            return TOK_TYPE_NUM;
        }

        STATE(S_KEYWORD) {
            switch (kw) {
                // -----------------------------------------------------------
                // keyword
                // -----------------------------------------------------------
            default:
                switch (kw) {
                default:
                    break;

#define X(a,b) \
                case (KW_##a): \
                    _ifj_lexical_log("TOK_TYPE_KW \"%s\"", (b)); \
                    break;
#include "keywords.def"
                    X_KEYWORD
#undef X
                }

                *attr = (void *) kw;
                return TOK_TYPE_KW;

                // -----------------------------------------------------------
                // term -- boolean
                // -----------------------------------------------------------
            case KW_TRUE:
                *attr = (void *) true;
                _ifj_lexical_log("TOK_TYPE_BOOL \"true\"");
                return TOK_TYPE_BOOL;

            case KW_FALSE:
                _ifj_lexical_log("TOK_TYPE_BOOL \"false\"");
                *attr = (void *) false;
                return TOK_TYPE_BOOL;

                // -----------------------------------------------------------
                // term -- nil
                // -----------------------------------------------------------
            case KW_NIL:
                _ifj_lexical_log("TOK_TYPE_NIL \"Nil\"");
                *attr = NULL;
                return TOK_TYPE_NIL;
            }
        }

        // ---------------------------------------------------------------
        // term -- string
        // ---------------------------------------------------------------
        STATE(S_STRING) {
            *attr = s;
            _ifj_lexical_log("TOK_TYPE_STR \"%s\"", string_getstr(s));
            return TOK_TYPE_STR;
        }

        // ---------------------------------------------------------------
        // comment
        // ---------------------------------------------------------------
        STATE(S_COMMENT) {
            SRC_GET(c);
            if (c == EOF) {
                NEXTSTATE(S_EOF);
            }
            else if (c == '\n') {
                NEXTSTATE(S_EOL);
            }
            else {
                NEXTSTATE(S_COMMENT);
            }
        }

        // ---------------------------------------------------------------
        // block comment
        // ---------------------------------------------------------------
        STATE(S_COMMENT_BLOCK) {
            SRC_GET(c);
            if (c == EOF) {
                NEXTSTATE(S_EEOF);
            }
            else if (c != '*') {
                NEXTSTATE(S_COMMENT_BLOCK);
            }
            else {
                NEXTSTATE(S_COMMENT_BLOCK_2);
            }
        }

        STATE(S_COMMENT_BLOCK_2) {
            SRC_GET(c);
            if (c == EOF) {
                NEXTSTATE(S_EEOF);
            }
            else if (c == '/') {
                NEXTSTATE(S_INIT);
            }
            else {
                NEXTSTATE(S_COMMENT_BLOCK);
            }
        }

        // ---------------------------------------------------------------
        // eol
        // ---------------------------------------------------------------
        STATE(S_EOL) {
            ++_line;
            _col = 0;

            if (SRC_GET(c) == '\n') {
                NEXTSTATE(S_EOL);
            }
            else {
                SRC_UNGET(c);
                _ifj_lexical_log("TOK_EOL");
                return TOK_EOL;
            }
        }

        // ---------------------------------------------------------------
        // eof
        // ---------------------------------------------------------------
        STATE(S_EOF) {
            _ifj_lexical_log("TOK_EOF");
            return TOK_EOF;
        }

        // ---------------------------------------------------------------
        // error states
        // ---------------------------------------------------------------
        STATE(S_ERROR) {
            _ifj_error_log("%s:%zu:%zu: Neznamy znak %c",
                           src_name, getCurrLine(), getCurrCol(), c);
            _ifj_lexical_log("TOK_EINVAL");
            return TOK_EINVAL;
        }

        STATE(S_EEOF) {
            _ifj_error_log("%s:%zu:%zu: Neocekavany konec souboru",
                           src_name, getCurrLine(), getCurrCol());
            _ifj_lexical_log("TOK_EEOF");
            return TOK_EEOF;
        }

        STATE(S_ESTRING) {
            _ifj_error_log
                ("%s:%zu:%zu: Neocekavany znak v retezci %c",
                 src_name, getCurrLine(), getCurrCol(), c);
            _ifj_lexical_log("TOK_ESTRING");
            return TOK_EINVAL;
        }

        STATE(S_ENUM) {
            _ifj_error_log("%s:%zu:%zu: Neocekavany tvar cisla %c",
                           src_name, getCurrLine(), getCurrCol(), c);
            _ifj_lexical_log("TOK_ENUM");
            return TOK_ENUM;
        }
    }

    NEXTSTATE(S_ERROR);

  alloc_failed:
    string_dispose(s);
    _ifj_error_log("%s", strerror(ENOMEM));
    _ifj_lexical_log("TOK_ENOMEM");
    return TOK_ENOMEM;
}
