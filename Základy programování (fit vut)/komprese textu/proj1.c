/*
 * Soubor:  proj1.c
 * Datum:   2010/10/07
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Jednoducha komprese textu, projekt c. 1 pro predmet IZP
 * Popis:   Program komprimuje nebo dekomprimuje text podle zadane delky bloku.
 */

#include "proj1.h"
#include "proj1circularbuffer.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// @return E_IOFMT pokud operace s bufferem selhala
#define RETURN_IOFMT_FAIL(func) { \
    if (!(func)) { goto RETURN_IOFMT; } \
}

// @return E_IOFMT pokud IO operace selhala
#define RETURN_IOFMT_EOF(func) { \
    if ((func) == EOF) { goto RETURN_IOFMT; } \
}

// zapise zbyvajici data na bufferech, pokud nastalo EOF
// @return E_OK pokud nastalo EOF
#define RETURN_OK_EOF_FLUSH(in, out, cb1, cb2) { \
    if (ferror(in) || ferror(out)) { \
        goto RETURN_IOFMT; \
    } \
    if (feof(out)) { \
        goto RETURN_IOFMT; \
    } \
    if (feof(in)) { \
        RETURN_IOFMT_FAIL(cb_write((cb1), (out))); \
        RETURN_IOFMT_FAIL(cb_write((cb2), (out))); \
        goto RETURN_OK; \
    } \
}

// @return EXIT_FAILURE pokud operace nevratila E_OK
#define CHECK_ERROR(err) { \
    if (err != E_OK) { \
        printError(err); \
        return EXIT_FAILURE; \
    } \
}

// zkomprimuje text pomoci dvojice kruhovych bufferu
// naplnenim obou bufferu a jejich vzajemnym porovnani
// @param lBlock delka bloku
// @param fInput vstupni stream
// @param fOutput vystupni stream
// @return E_IOFMT pokud nastala chyba pri zpracovani/cteni/zapisu; jinak E_OK
Error compress(const unsigned long lBlock, FILE* fInput, FILE* fOutput)
{
    assert(fInput != NULL);
    assert(fOutput != NULL);

    CircularBuffer buf;
    CircularBuffer buf2;
    
    if (!cb_init(&buf, lBlock)) {
        return E_ALLOCATION_FAILED;
    }
    if (!cb_init(&buf2, lBlock)) {
        cb_free(&buf);
        return E_ALLOCATION_FAILED;
    }

    while (!feof(fInput)) {
        // naplneni bufferu
        RETURN_IOFMT_FAIL(cb_read(&buf, fInput));
        RETURN_OK_EOF_FLUSH(fInput, fOutput, &buf, &buf2);

        RETURN_IOFMT_FAIL(cb_read(&buf2, fInput));
        RETURN_OK_EOF_FLUSH(fInput, fOutput, &buf, &buf2);

        // hledani opakovani
        size_t repeat;
        for (repeat = 1; !feof(fInput) && repeat < REPEAT_MAX && cb_compare(&buf, &buf2); ++repeat) {
            cb_clear(&buf2);
            RETURN_IOFMT_FAIL(cb_read(&buf2, fInput));
        }

        assert(repeat <= REPEAT_MAX);
        if (repeat > 1) {
            // zapise opakovani
            RETURN_IOFMT_EOF(fputc(repeat + '0', fOutput));
            RETURN_IOFMT_FAIL(cb_write(&buf, fOutput));

            // pokud nenastal EOF vymenime buffery
            RETURN_OK_EOF_FLUSH(fInput, fOutput, &buf, &buf2);
            if (!feof(fInput)) {
                cb_swap(&buf, &buf2);
                cb_clear(&buf2);
            }
        }
        else {
            // zapise 1 znak
            RETURN_IOFMT_FAIL(cb_write_n(&buf, fOutput, 1));
            RETURN_OK_EOF_FLUSH(fInput, fOutput, &buf, &buf2);

            // pokud je misto a ma co zapsat tak posune buffery
            assert(cb_count(&buf) < buf.size_max && cb_count(&buf2) > 0);
            cb_push_back(&buf, cb_pop_front(&buf2));

            // pri dalsim pruchodu cyklu precte dalsi znak
        }
    }

  RETURN_OK:
    cb_free(&buf);
    cb_free(&buf2);
    return E_OK;

  RETURN_IOFMT:
    cb_free(&buf);
    cb_free(&buf2);
    return E_IOFMT;
}

// dekomprimuje text
// @param lBlock delka bloku
// @param fInput vstupni stream
// @param fOutput vystupni stream
// @return E_IOFMT pokud nastala chyba pri zpracovani/cteni/zapisu; jinak E_OK
Error decompress(const unsigned long lBlock, FILE* fInput, FILE* fOutput)
{
    assert(fInput != NULL);
    assert(fOutput != NULL);

    char* buf = (char*)malloc(lBlock);
    if (buf == NULL) {
        return E_ALLOCATION_FAILED;
    }

    int current = EOF;

    while (EOF != (current = fgetc(fInput))) {
        if (isdigit(current)) {
            size_t repeat = current - '0';

            if (!(repeat > 1 && repeat <= REPEAT_MAX)) {
                goto RETURN_IOFMT;
            }

            // nacte lBlock znaku
            for (char* lpBuf = buf; lpBuf != buf + lBlock; ++lpBuf) {
                current = fgetc(fInput);

                // za cislem musi nasledovat lBlock znaku
                RETURN_IOFMT_EOF(current);

                // znak musi byt tisknutelny a nesmi byt cislo
                if (!isprint(current) || isdigit(current)) {
                    goto RETURN_IOFMT;
                }

                *lpBuf = (char)current;
            }

            // vypise repeat-krat lBlock znaku
            for (; repeat > 0; --repeat) {
                for (const char* lpBuf = buf; lpBuf != buf + lBlock; ++lpBuf) {
                    RETURN_IOFMT_EOF(fputc((int)*lpBuf, fOutput));
                }
            }
        } // end isdigit
        else if (isprint(current) || isspace(current)) {
            // tisknutelny znak nebo novy radek
            RETURN_IOFMT_EOF(fputc(current, fOutput));
        }
        else {
            goto RETURN_IOFMT;
        }
    }

//RETURN_OK:
    free(buf);
    return E_OK;

  RETURN_IOFMT:
    free(buf);
    return E_IOFMT;
}

// Vytiskne napovedu
void printHelp()
{
    printf("%s",
        "Jednoducha komprese textu, projekt c. 1 pro predmet IZP.\n"
        "\n"
        "Program komprimuje nebo dekomprimuje text podle zadane delky bloku.\n"
        "Autor: Radek Sevcik (c) 2010\n"
        "\n"
        "Pouziti: proj1 -h\n"
        "         proj1 -c <N>\n"
        "         proj1 -d <N>\n"
        "         proj1 --extra\n"
        "\n"
        "Popis parametru:\n"
        "  -h           Vypise tuto napovedu.\n"
        "  -c <N>       Zkomprimuje text o delce bloku N ze vstupu.\n"
        "  -d <N>       Dekomprimuje text o delce bloku N na vystup.\n"
        "  --extra      Vlastni rozsireni programu.\n"
        "\n"
        "Vstupni data mohou obsahovat pouze tisknutelne znaky bez diakritiky a cislic.\n"
        "Pro dekompresi textu se musi zadat stejna delka bloku jako pri kompresi.\n");
}

// Vytiskne chybu
// @param err chyba
void printError(Error err)
{
    switch (err) {
        case E_NOT_IMPLEMENTED:
            fprintf(stderr, "Operace neni implementovana.\n");
            break;

        case E_TOO_MANY_ARGS:
            fprintf(stderr, "Zadali jste prilis mnoho parametru.\n");
            break;

        case E_UNKNOWN_PARAM:
            fprintf(stderr, "Neznamy prikaz.\n");
            break;

        case E_INVALID_BLOCK_LENGTH:
            fprintf(stderr, "Neplatna delka bloku. Zadejte velikost bloku v rozmezi 1..%lu.\n", ULONG_MAX);
            break;

        case E_IOFMT:
            fprintf(stderr, "Nastala chyba pri zpracovani vstupu/vystupu.\n");
            break;

        case E_OK:
            fprintf(stderr, "OK.\n");
            break;

        case E_ALLOCATION_FAILED:
            fprintf(stderr, "Neni dostatek volne pameti. Ukoncete vsechny spustene programy a zkuste to znovu.\n");
            break;

        default:
            fprintf(stderr, "Neznama chyba.\n");
            break;
    }
}

// Otestuje velikost bloku zadaneho z prikazove radky
// @param blockLength delka bloku
bool checkBlockLength(unsigned long blockLength)
{
    if (blockLength == 0) {
        return false;
    }
    if (blockLength == ULONG_MAX && errno == ERANGE) {
        return false;
    }

    return true;
}

// zpracuje parametry prikazove radky
// @param params zaobaleni parametru
// @return E_TOO_MANY_ARGS pri zadani hodne parametru
// @return E_INVALID_BLOCK_LENGTH pri zadani chybne delky bloku
// @return E_NOT_IMPLEMENTED pokud operace jeste nebyla implementovana
// @return E_UNKNOWN_PARAM pokud je zadan neplatny argument
// @return E_OK pokud zpracovani problehlo uspesne
Error parseParams(const int argc, char* argv[], ProgParams* const params)
{
    assert(argc >= 1);
    assert(argv != NULL);
    assert(params != NULL);

    if (argc > 3 ) {
        return E_TOO_MANY_ARGS;
    }
    else if (argc >= 2) {
        if (strcmp(argv[1], PARAMS_COMPRESS) == 0) {
            if (argc != 3) {
                return E_INVALID_BLOCK_LENGTH;
            }

            params->progState = PS_COMPRESS;
            params->blockLength = strtoul(argv[2], NULL, 10);
            return checkBlockLength(params->blockLength) ? E_OK : E_INVALID_BLOCK_LENGTH;
        }
        else if (strcmp(argv[1], PARAMS_DECOMPRESS) == 0) {
            if (argc != 3) {
                return E_INVALID_BLOCK_LENGTH;
            }

            params->progState = PS_DECOMPRESS;
            params->blockLength = strtoul(argv[2], NULL, 10);
            return checkBlockLength(params->blockLength) ? E_OK : E_INVALID_BLOCK_LENGTH;
        }
        else if (strcmp(argv[1], PARAMS_EXTRA) == 0) {
            params->progState = PS_EXTRA;
            return E_NOT_IMPLEMENTED;
        }
        else if (strcmp(argv[1], PARAMS_HELP) == 0) {
            params->progState = PS_HELP;
            return E_OK;
        }
    }
    else {
        params->progState = PS_HELP;
        return E_OK;
    }

    return E_UNKNOWN_PARAM;
}

int main(int argc, char* argv[])
{
    ProgParams state;
    
    Error err = parseParams(argc, argv, &state);
    CHECK_ERROR(err);

    switch (state.progState) {
        case PS_HELP:
            printHelp();
            break;

        case PS_DECOMPRESS:
            err = decompress(state.blockLength, stdin, stdout);
            CHECK_ERROR(err);
            break;
            
        case PS_COMPRESS:
            err = compress(state.blockLength, stdin, stdout);
            CHECK_ERROR(err);
            break;

        default:
            printError(E_NOT_IMPLEMENTED);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
