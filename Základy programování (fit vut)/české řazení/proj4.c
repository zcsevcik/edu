/*
 * Soubor:  proj4.c
 * Datum:   2010/12/13
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Ceske razeni, projekt c. 4 pro predmet IZP
 * Extra:   parametr usort a codepage
 */

#include "proj4.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_ERROR(err) { \
    if (err != E_OK) { \
        printError(err); \
        return EXIT_FAILURE; \
    } \
}

#define _countof(static_array) (sizeof(static_array) / sizeof(*static_array))

#define SPACE_CHARS " \t\n"
#define MAX_BUF_LEN 1000

#define VALUE_CH 11

const int PRIMARY_CP28592[] = {
    [0x20] = 1,    [0xA0] = 1,    [0xAD] = 1,
    [0x41] = 2,    [0x61] = 2,    [0xC1] = 2,    [0xE1] = 2,
    [0x42] = 3,    [0x62] = 3,
    [0x43] = 4,    [0x63] = 4,
    [0xC8] = 5,    [0xE8] = 5,
    [0x44] = 6,    [0x64] = 6,    [0xCF] = 6,    [0xEF] = 6,
    [0x45] = 7,    [0x65] = 7,    [0xC9] = 7,    [0xE9] = 7,    [0xCC] = 7,    [0xEC] = 7,
    [0x46] = 8,    [0x66] = 8,
    [0x47] = 9,    [0x67] = 9,
    [0x48] = 10,   [0x68] = 10,
    //reseved for ch
    [0x49] = 12,   [0x69] = 12,   [0xCD] = 12,   [0xED] = 12,
    [0x4A] = 13,   [0x6A] = 13,
    [0x4B] = 14,   [0x6B] = 14,
    [0x4C] = 15,   [0x6C] = 15,
    [0x4D] = 16,   [0x6D] = 16,
    [0x4E] = 17,   [0x6E] = 17,   [0xD2] = 17,   [0xF2] = 17,
    [0x4F] = 18,   [0x6F] = 18,   [0xD3] = 18,   [0xF3] = 18,
    [0x50] = 19,   [0x70] = 19,
    [0x51] = 20,   [0x71] = 20,
    [0x52] = 21,   [0x72] = 21,
    [0xD8] = 22,   [0xF8] = 22, //ø
    [0x53] = 23,   [0x73] = 23,
    [0xA9] = 24,   [0xB9] = 24, //¹
    [0x54] = 25,   [0x74] = 25,   [0xAB] = 25,   [0xBB] = 25,
    [0x55] = 26,   [0x75] = 26,   [0xDA] = 26,   [0xFA] = 26,   [0xD9] = 26,   [0xF9] = 26,
    [0x56] = 27,   [0x76] = 27,
    [0x57] = 28,   [0x77] = 28,
    [0x58] = 29,   [0x78] = 29,
    [0x59] = 30,   [0x79] = 30,   [0xDD] = 30,   [0xFD] = 30,
    [0x5A] = 31,   [0x7A] = 31,
    [0xAE] = 32,   [0xBE] = 32, //¾
};

const int SECONDARY_CP28592[] = {
    [0x41] = 1,    [0x61] = 1,
    [0xC1] = 2,    [0xE1] = 2,
    [0x44] = 3,    [0x64] = 3,
    [0xCF] = 4,    [0xEF] = 4,
    [0x45] = 5,    [0x65] = 5,
    [0xC9] = 6,    [0xE9] = 6,
    [0xCC] = 7,    [0xEC] = 7,
    [0x49] = 8,    [0x69] = 8,
    [0xCD] = 9,    [0xED] = 9,
    [0x4E] = 10,   [0x6E] = 10,
    [0xD2] = 11,   [0xF2] = 11,
    [0x4F] = 12,   [0x6F] = 12,
    [0xD3] = 13,   [0xF3] = 13,
    [0x54] = 14,   [0x74] = 14,
    [0xAB] = 15,   [0xBB] = 15,
    [0x55] = 16,   [0x75] = 16,
    [0xDA] = 17,   [0xFA] = 17,
    [0xD9] = 18,   [0xF9] = 18,
    [0x59] = 19,   [0x79] = 19,
    [0xDD] = 20,   [0xFD] = 20,
};

const int PRIMARY_CP1250[] = {
    [0x20] = 1,    [0xA0] = 1,    [0xAD] = 1,
    [0x41] = 2,    [0x61] = 2,    [0xC1] = 2,    [0xE1] = 2,
    [0x42] = 3,    [0x62] = 3,
    [0x43] = 4,    [0x63] = 4,
    [0xC8] = 5,    [0xE8] = 5,
    [0x44] = 6,    [0x64] = 6,    [0xCF] = 6,    [0xEF] = 6,
    [0x45] = 7,    [0x65] = 7,    [0xC9] = 7,    [0xE9] = 7,    [0xCC] = 7,    [0xEC] = 7,
    [0x46] = 8,    [0x66] = 8,
    [0x47] = 9,    [0x67] = 9,
    [0x48] = 10,   [0x68] = 10,
    //reseved for ch
    [0x49] = 12,   [0x69] = 12,   [0xCD] = 12,   [0xED] = 12,
    [0x4A] = 13,   [0x6A] = 13,
    [0x4B] = 14,   [0x6B] = 14,
    [0x4C] = 15,   [0x6C] = 15,
    [0x4D] = 16,   [0x6D] = 16,
    [0x4E] = 17,   [0x6E] = 17,   [0xD2] = 17,   [0xF2] = 17,
    [0x4F] = 18,   [0x6F] = 18,   [0xD3] = 18,   [0xF3] = 18,
    [0x50] = 19,   [0x70] = 19,
    [0x51] = 20,   [0x71] = 20,
    [0x52] = 21,   [0x72] = 21,
    [0xD8] = 22,   [0xF8] = 22, //ø
    [0x53] = 23,   [0x73] = 23,
    [0x8A] = 24,   [0x9A] = 24, //¹
    [0x54] = 25,   [0x74] = 25,   [0x8D] = 25,   [0x9D] = 25,
    [0x55] = 26,   [0x75] = 26,   [0xDA] = 26,   [0xFA] = 26,   [0xD9] = 26,   [0xF9] = 26,
    [0x56] = 27,   [0x76] = 27,
    [0x57] = 28,   [0x77] = 28,
    [0x58] = 29,   [0x78] = 29,
    [0x59] = 30,   [0x79] = 30,   [0xDD] = 30,   [0xFD] = 30,
    [0x5A] = 31,   [0x7A] = 31,
    [0x8E] = 32,   [0x9E] = 32, //¾
};

const int SECONDARY_CP1250[] = {
    [0x41] = 1,    [0x61] = 1,
    [0xC1] = 2,    [0xE1] = 2,
    [0x44] = 3,    [0x64] = 3,
    [0xCF] = 4,    [0xEF] = 4,
    [0x45] = 5,    [0x65] = 5,
    [0xC9] = 6,    [0xE9] = 6,
    [0xCC] = 7,    [0xEC] = 7,
    [0x49] = 8,    [0x69] = 8,
    [0xCD] = 9,    [0xED] = 9,
    [0x4E] = 10,   [0x6E] = 10,
    [0xD2] = 11,   [0xF2] = 11,
    [0x4F] = 12,   [0x6F] = 12,
    [0xD3] = 13,   [0xF3] = 13,
    [0x54] = 14,   [0x74] = 14,
    [0xAB] = 15,   [0xBB] = 15,
    [0x55] = 16,   [0x75] = 16,
    [0xDA] = 17,   [0xFA] = 17,
    [0xD9] = 18,   [0xF9] = 18,
    [0x59] = 19,   [0x79] = 19,
    [0xDD] = 20,   [0xFD] = 20,
};

const int PRIMARY_CP852[] = {
    [0x20] = 1,    [0xFF] = 1,    [0xF0] = 1,
    [0x41] = 2,    [0x61] = 2,    [0xB5] = 2,    [0xA0] = 2,
    [0x42] = 3,    [0x62] = 3,
    [0x43] = 4,    [0x63] = 4,
    [0xAC] = 5,    [0x9F] = 5,
    [0x44] = 6,    [0x64] = 6,    [0xD2] = 6,    [0xD4] = 6,
    [0x45] = 7,    [0x65] = 7,    [0x90] = 7,    [0x82] = 7,    [0xB7] = 7,    [0xD8] = 7,
    [0x46] = 8,    [0x66] = 8,
    [0x47] = 9,    [0x67] = 9,
    [0x48] = 10,   [0x68] = 10,
    //reseved for ch
    [0x49] = 12,   [0x69] = 12,   [0xD6] = 12,   [0xA1] = 12,
    [0x4A] = 13,   [0x6A] = 13,
    [0x4B] = 14,   [0x6B] = 14,
    [0x4C] = 15,   [0x6C] = 15,
    [0x4D] = 16,   [0x6D] = 16,
    [0x4E] = 17,   [0x6E] = 17,   [0xD5] = 17,   [0xE5] = 17,
    [0x4F] = 18,   [0x6F] = 18,   [0xE0] = 18,   [0xA2] = 18,
    [0x50] = 19,   [0x70] = 19,
    [0x51] = 20,   [0x71] = 20,
    [0x52] = 21,   [0x72] = 21,
    [0xFC] = 22,   [0xFD] = 22, //ø
    [0x53] = 23,   [0x73] = 23,
    [0xE6] = 24,   [0xE7] = 24, //¹
    [0x54] = 25,   [0x74] = 25,   [0x9B] = 25,   [0x9C] = 25,
    [0x55] = 26,   [0x75] = 26,   [0xE9] = 26,   [0xA3] = 26,   [0xDE] = 26,   [0x85] = 26,
    [0x56] = 27,   [0x76] = 27,
    [0x57] = 28,   [0x77] = 28,
    [0x58] = 29,   [0x78] = 29,
    [0x59] = 30,   [0x79] = 30,   [0xED] = 30,   [0xEC] = 30,
    [0x5A] = 31,   [0x7A] = 31,
    [0xA6] = 32,   [0xA7] = 32, //¾
};

const int SECONDARY_CP852[] = {
    [0x41] = 1,    [0x61] = 1,
    [0xB5] = 2,    [0xA0] = 2,
    [0x44] = 3,    [0x64] = 3,
    [0xD2] = 4,    [0xD4] = 4,
    [0x45] = 5,    [0x65] = 5,
    [0x90] = 6,    [0x82] = 6,
    [0xB7] = 7,    [0xD8] = 7,
    [0x49] = 8,    [0x69] = 8,
    [0xD6] = 9,    [0xA1] = 9,
    [0x4E] = 10,   [0x6E] = 10,
    [0xD5] = 11,   [0xE5] = 11,
    [0x4F] = 12,   [0x6F] = 12,
    [0xE0] = 13,   [0xA2] = 13,
    [0x54] = 14,   [0x74] = 14,
    [0x9B] = 15,   [0x9C] = 15,
    [0x55] = 16,   [0x75] = 16,
    [0xE9] = 17,   [0xA3] = 17,
    [0xDE] = 18,   [0x85] = 18,
    [0x59] = 19,   [0x79] = 19,
    [0xED] = 20,   [0xEC] = 20,
};

const int* PRIMARY_CP[] = {
    [CP28592] = PRIMARY_CP28592,
    [CP1250] = PRIMARY_CP1250,
    [CP852] = PRIMARY_CP852,
};

const int* SECONDARY_CP[] = {
    [CP28592] = SECONDARY_CP28592,
    [CP1250] = SECONDARY_CP1250,
    [CP852] = SECONDARY_CP852,
};

int mystrcmp(const char* str1, const char* str2, Codepage cp)
{
    if (str1 == str2) {
        return 0;
    }

    const char* pstr1 = str1;
    const char* pstr2 = str2;

    const int* pcp = PRIMARY_CP[cp];
    const int* scp = SECONDARY_CP[cp];

    while (*pstr1 != '\0' && *pstr2 != '\0') {
        int val1;
        int val2;

        if ((*pstr1 == 0x43 || *pstr1 == 0x63) &&
           (*(pstr1 + 1) == 0x48 || *(pstr1 + 1) == 0x68))
        {
            val1 = VALUE_CH;
            ++pstr1;
        }
        else {
            val1 = pcp[(unsigned char)(*pstr1)];
        }

        if ((*pstr2 == 0x43 || *pstr2 == 0x63) &&
           (*(pstr2 + 1) == 0x48 || *(pstr2 + 1) == 0x68))
        {
            val2 = VALUE_CH;
            ++pstr2;
        }
        else {
            val2 = pcp[(unsigned char)*pstr2];
        }

        if (val1 == 0) { val1 = *pstr1; }
        if (val2 == 0) { val2 = *pstr2; }

        if (val1 != val2) {
            return val1 > val2 ? 1 : -1;
        }

        pstr1++;
        pstr2++;
    }

    bool sameLength = *pstr1 == *pstr2;
    if (sameLength) {
        const char* pstr1 = str1;
        const char* pstr2 = str2;

        while (*pstr1 != '\0' && *pstr2 != '\0') {
            int val1 = scp[(unsigned char)*pstr1];
            int val2 = scp[(unsigned char)*pstr2];

            if (val1 == 0) { val1 = *pstr1; }
            if (val2 == 0) { val2 = *pstr2; }

            if (val1 != val2) {
                return val1 > val2 ? 1 : -1;
            }

            pstr1++;
            pstr2++;
        }
    }
    else {
        return *pstr1 == '\0' ? -1 : 1;
    }

    return 0;
}

size_t getCollumnID(char* collumn, Header* header, size_t len)
{
    assert(collumn != NULL);
    assert(header != NULL);

    for (size_t i = 0; i < len; ++i) {
        if (strcmp(header[i].name, collumn) == 0) {
            return header[i].id;
        }
    }

    return 0;
}

void usort(Record* recs, size_t* len, Codepage cp)
{
    assert(recs != NULL);
    assert(len != NULL);

    sort(recs, *len, cp);

    size_t ulen = 0;

    for (size_t i = 0; i < *len - 1; ++i) {
        if (mystrcmp(recs[i], recs[i + 1], cp) != 0) {
            recs[ulen++] = recs[i];
        }
    }
    recs[ulen++] = recs[*len - 1];

    *len = ulen;
}

void sort(Record* recs, size_t len, Codepage cp)
{
    assert(recs != NULL);

    for (size_t i = 0; i < len; ++i) {
        for (size_t j = len - 1; j > i; --j) {
            if (mystrcmp(recs[j - 1], recs[j], cp) > 0) {
                Record rec = recs[j - 1];
                recs[j - 1] = recs[j];
                recs[j] = rec;
            }
        }
    }
}

Error select(const Table* tbl, Record** recs, size_t* len, size_t col, RowSelect selType, size_t selCol, const char* selStr, Codepage cp)
{
    assert(tbl != NULL);
    assert(recs != NULL);
    assert(len != NULL);

    switch (selType) {
        case RS_ALL: {
            *len = tbl->rows;
            *recs = (Record*)malloc(tbl->rows * sizeof(Record));
            if (*recs == NULL) {
                return E_ALLOC_FAILED;
            }
            for (size_t i = 0; i < tbl->rows; ++i) {
                (*recs)[i] = table_getitem(tbl, i, col - 1);
            }
        }   break;

        case RS_AFTER: {
            assert(col > 0);
            *len = 0;
            for (size_t i = 0; i < tbl->rows; ++i) {
                *recs = (Record*)realloc(*recs, (*len + 1) * sizeof(Record));
                if (*recs == NULL) {
                    return E_ALLOC_FAILED;
                }
                if (mystrcmp(table_getitem(tbl, i, selCol - 1), selStr, cp) >= 0) {
                    (*recs)[(*len)++] = table_getitem(tbl, i, col - 1);
                }
            }
        }   break;

        case RS_BEFORE: {
            assert(col > 0);
            *len = 0;
            for (size_t i = 0; i < tbl->rows; ++i) {
                *recs = (Record*)realloc(*recs, (*len + 1) * sizeof(Record));
                if (*recs == NULL) {
                    return E_ALLOC_FAILED;
                }
                if (mystrcmp(table_getitem(tbl, i, selCol - 1), selStr, cp) <= 0) {
                    (*recs)[(*len)++] = table_getitem(tbl, i, col - 1);
                }
            }
        }   break;

        default:
            assert(false);
            break;
    }

    return E_OK;
}

Record table_getitem(const Table* tbl, size_t row, size_t col)
{
    assert(tbl != NULL);
    assert(row < tbl->rows);
    assert(col < tbl->cols);

    return tbl->records[row * tbl->cols + col];
}

Error table_load(FILE* fp, Table* tbl)
{
    assert(fp != NULL);
    assert(tbl != NULL);

    // nacteni radku
    char buf[MAX_BUF_LEN];
    char* curr = fgets(buf, _countof(buf), fp);
    if (curr == NULL) {
        return E_IO;
    }

    // nacteni hlavicky
    curr = strtok(curr, SPACE_CHARS);
    for (tbl->cols = 0; curr != NULL; tbl->cols++) {
        tbl->headers = (Header*)realloc(tbl->headers, (tbl->cols + 1) * sizeof(Header));
        if (tbl->headers == NULL) {
            table_free(tbl);
            return E_ALLOC_FAILED;
        }

        tbl->headers[tbl->cols].id = tbl->cols + 1;
        tbl->headers[tbl->cols].name = (char*)malloc((strlen(curr) + 1) * sizeof(char));
        if (tbl->headers[tbl->cols].name == NULL) {
            table_free(tbl);
            return E_ALLOC_FAILED;
        }
        strcpy(tbl->headers[tbl->cols].name, curr);

        curr = strtok(NULL, SPACE_CHARS);
    }

    // nacteni dat
    size_t rowAlloc = 0;
    curr = fgets(buf, _countof(buf), fp);
    for (tbl->rows = 0; curr != NULL; tbl->rows++) {
        rowAlloc = (tbl->rows + 1) * tbl->cols;
        tbl->records = (Record*)realloc(tbl->records, rowAlloc * sizeof(Record));
        if (tbl->records == NULL) {
            table_free(tbl);
            return E_ALLOC_FAILED;
        }

        curr = strtok(buf, SPACE_CHARS);
        for (size_t i = 0; i < tbl->cols; ++i) {
            if (curr == NULL) {
                return E_IO;
            }

            Record* record = &tbl->records[tbl->rows * tbl->cols + i];
            *record = (Record)malloc((strlen(curr) + 1) * sizeof(char));
            if (*record == NULL) {
                return E_ALLOC_FAILED;
            }
            
            strcpy(*record, curr);

            curr = strtok(NULL, SPACE_CHARS);
        }

        curr = fgets(buf, _countof(buf), fp);
    }

    return E_OK;
}

void record_print(const Record* record, size_t len, FILE* fp)
{
    for (size_t i = 0; i < len; ++i) {
        fprintf(fp, "%s\n", record[i]);
    }
}

void table_free(Table* tbl)
{
    assert(tbl != NULL);

    Record* end = &tbl->records[tbl->rows * tbl->cols];
    for (Record* i = tbl->records; i < end; ++i) {
        free(*i);
    }
    free(tbl->records);

    for (size_t i = 0; i < tbl->cols; ++i) {
        free(tbl->headers[i].name);
    }
    free(tbl->headers);

    tbl->records = NULL;
    tbl->headers = NULL;
    tbl->rows = 0;
    tbl->cols = 0;
}

// Vytiskne napovedu
void printHelp()
{
    printf(
        "Ceske razeni, projekt c. 4 pro predmet IZP.\n"
        "Autor: Radek Sevcik (c) 2010\n"
        "\n"
        "Pouziti: proj4 [parametry] vstupni_soubor vystupni_soubor\n"
        "\n"
        "Popis parametru:\n"
        "  -h\t\tVypise tuto napovedu.\n"
        "  --print COL\tPovinny parametr. Vyber sloupce pro tisk.\n"
        "  --sort\tNepovinny parametr. Vzestupne seradi hodnoty urcene pro tisk.\n"
        "  --usort\tROZSIRENI! Nepovinny parametr. Unikatni razeni.\n"
        "  --codepage\tROZSIRENI! Nepovinny parametr. Urceni znakove sady.\n"
        "\n"
        "  Kriterium vyberu radku:\n"
        "  --before COL STRING\tNepovinny parametr. Vybere radky pred.\n"
        "  --after COL STRING\tNepovinny parametr. Vybere radky za.\n"
        "\n"
        "Platne hodnoty parametru codepage: win1250 iso-8859-2 cp852.\n"
        "\n");
}

// Vytiskne chybu
void printError(Error err)
{
    switch (err) {
        case E_NOT_IMPLEMENTED:
            fprintf(stderr, "Operace neni implementovana.\n");
            break;

        case E_UNKNOWN_PARAM:
            fprintf(stderr, "Neznamy prikaz.\n");
            break;

        case E_INVALID_PARAM:
            fprintf(stderr, "Neplatny parametr.\n");
            break;

        case E_UNKNOWN_ERROR:
            fprintf(stderr, "Nastala chyba v programu (file format error, malloc fail).\n");
            break;

        case E_OK:
            fprintf(stderr, "OK.\n");
            break;

        case E_IO:
            fprintf(stderr, "Nastala chyba pri cteni nebo zapisu.\n");
            break;

        case E_ALLOC_FAILED:
            fprintf(stderr, "Nastala chyba pri alokaci.\n");
            break;

        case E_BAD_COLLUMN:
            fprintf(stderr, "Neplatny sloupec.\n");
            break;

        default:
            fprintf(stderr, "Neznama chyba.\n");
            break;
    }
}

// parsuje parametry prikazove radky do struktury ProgParams
Error parseParams(int argc, char* argv[], ProgParams* const params)
{
    assert(argc >= 1);
    assert(argv != NULL);
    assert(params != NULL);

    if (argc == 2 && strcmp(argv[1], PARAMS_HELP) == 0) {
        params->progState = PS_HELP;
        if (argc != 2) { return E_INVALID_PARAM; }
    }
    else if (argc >= 5) {
        params->progState = PS_PRINT;
        params->selectRow = RS_ALL;

        for (int i = 1; i < argc - 2; ++i) {
            if (strcmp(argv[i], PARAMS_ROW_AFTER) == 0) {
                if (params->selectRow != RS_ALL) {
                    return E_INVALID_PARAM;
                }
                if (argc < i + 2) {
                    return E_INVALID_PARAM;
                }

                params->selectRow = RS_AFTER;
                params->selectCol = argv[i + 1];
                params->selectString = argv[i + 2];
                i += 2;
            }
            else if (strcmp(argv[i], PARAMS_ROW_BEFORE) == 0) {
                if (params->selectRow != RS_ALL) {
                    return E_INVALID_PARAM;
                }
                if (argc < i + 2) {
                    return E_INVALID_PARAM;
                }

                params->selectRow = RS_BEFORE;
                params->selectCol = argv[i + 1];
                params->selectString = argv[i + 2];
                i += 2;
            }
            else if (strcmp(argv[i], PARAMS_SORT) == 0) {
                if (params->sort != SORT_NONE) {
                    return E_INVALID_PARAM;
                }

                params->sort = SORT_BUBBLE;
            }
            else if (strcmp(argv[i], PARAMS_USORT) == 0) {
                if (params->sort != SORT_NONE) {
                    return E_INVALID_PARAM;
                }

                params->sort = SORT_UNIQUE;
            }
            else if (strcmp(argv[i], PARAMS_PRINT) == 0) {
                if (argc < i + 1) {
                    return E_INVALID_PARAM;
                }

                params->printCol = argv[++i];
            }
            else if (strcmp(argv[i], PARAMS_CODEPAGE) == 0) {
                if (argc < i + 1) {
                    return E_INVALID_PARAM;
                }

                if (strcmp(argv[i + 1], PARAMS_CODEPAGE_CP1250) == 0) {
                    params->codepage = CP1250;
                }
                else if (strcmp(argv[i + 1], PARAMS_CODEPAGE_CP28592) == 0) {
                    params->codepage = CP28592;
                }
                else if (strcmp(argv[i + 1], PARAMS_CODEPAGE_CP852) == 0) {
                    params->codepage = CP852;
                }
                else {
                    return E_INVALID_PARAM;
                }
                ++i;
            }
            else {
                return E_INVALID_PARAM;
            }
        }

        if (params->printCol == NULL) {
            return E_INVALID_PARAM;
        }

        params->inputFile = argv[argc - 2];
        params->outputFile = argv[argc - 1];
    }
    else if (argc == 1) {
        params->progState = PS_HELP;
    }
    else {
        return E_INVALID_PARAM;
    }

    return E_OK;
}

int main(int argc, char* argv[])
{
    ProgParams state = {
        .progState = PS_HELP, .sort = SORT_NONE, .printCol = NULL,
        .selectRow = RS_ALL, .selectCol = NULL, .selectString = NULL,
        .inputFile = NULL, .outputFile = NULL, .codepage = CP28592,
    };

    Error err = parseParams(argc, argv, &state);
    CHECK_ERROR(err);

    switch (state.progState) {
        case PS_HELP:
            printHelp();
            break;

        case PS_PRINT: {
            FILE* fpin = NULL;
            FILE* fpout = NULL;
            
            fpin = fopen(state.inputFile, "r");
            if (fpin == NULL) {
                perror(state.inputFile);
                goto closeFile;
            }

            fpout = fopen(state.outputFile, "w");
            if (fpout == NULL) {
                perror(state.outputFile);
                goto closeFile;
            }

            Table tbl = { .rows = 0, .cols = 0, .headers = NULL, .records = NULL };
            Error hr = table_load(fpin, &tbl);
            if (hr != E_OK) {
                table_free(&tbl);
                printError(hr);
                goto closeFile;
            }

            if (tbl.rows == 0) {
                table_free(&tbl);
                goto closeFile;
            }

            size_t col = getCollumnID(state.printCol, tbl.headers, tbl.cols);
            if (col == 0) {
                table_free(&tbl);
                printError(E_BAD_COLLUMN);
                goto closeFile;
            }

            size_t lRecords = 0;
            Record* records = NULL;
            size_t selCol = 0;
            
            if (state.selectRow != RS_ALL) {
                selCol = getCollumnID(state.selectCol, tbl.headers, tbl.cols);
                if (selCol == 0) {
                    table_free(&tbl);
                    printError(E_BAD_COLLUMN);
                    goto closeFile;
                }
            }
            else {
                selCol = 0;
            }

            hr = select(&tbl, &records, &lRecords, col, state.selectRow, selCol, state.selectString, state.codepage);
            if (hr != E_OK) {
                table_free(&tbl);
                printError(hr);
                goto closeFile;
            }
            if (lRecords == 0) {
                table_free(&tbl);
                goto closeFile;
            }
            
            switch (state.sort) {
                case SORT_NONE:
                    break;

                case SORT_BUBBLE:
                    sort(records, lRecords, state.codepage);
                    break;

                case SORT_UNIQUE:
                    usort(records, &lRecords, state.codepage);
                    break;

                default:
                    assert(false);
                    break;
            }

            record_print(records, lRecords, fpout);

            free(records);
            table_free(&tbl);

          closeFile:
            if (fpout != NULL && fclose(fpout) != 0) {
                perror(state.outputFile);
            }
            if (fpin != NULL && fclose(fpin) != 0) {
                perror(state.outputFile);
            }

        }   break;

        default:
            printError(E_NOT_IMPLEMENTED);
            return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
