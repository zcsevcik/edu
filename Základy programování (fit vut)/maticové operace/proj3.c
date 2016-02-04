/*
 * Soubor:  proj3.c
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#include "proj3.h"
#include "proj3io.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// return EXIT_FAILURE; pokud funkce nevratila E_OK
#define CHECK_ERROR(err) { \
    if (err != E_OK) { \
        printError(err); \
        return EXIT_FAILURE; \
    } \
}

// Vytiskne napovedu
void printHelp()
{
    printf(
        "Maticove operace, projekt c. 3 pro predmet IZP.\n"
        "Autor: Radek Sevcik (c) 2010\n"
        "\n"
        "Program pro praci s maticemi (nasobeni, vyraz), vektory (scitani, skalarni\n"
        "soucin) a operacemi (osmismerka, pocet bublin, bludiste, kulecnik).\n"
        "Vstup nacita ze souboru.\n"
        "\n"
        "Pouziti: proj3 -h\n"
        "         proj3 --test <file.txt>\n"
        "         proj3 --vadd <vector1.txt> <vector2.txt>\n"
        "         proj3 --vscal <vector1.txt> <vector2.txt>\n"
        "         proj3 --mmult <matrix1.txt> <matrix2.txt>\n"
        "         proj3 --mexpr <matrix1.txt> <matrix2.txt>\n"
        "         proj3 --eight <vector.txt> <matrix.txt>\n"
        "         proj3 --bubbles <matrix.txt>\n"
        "         proj3 --extbubbles <matrix.txt>\n"
        "         proj3 --maze <matrix.txt>\n"
        "         proj3 --carom <coord> <dir> <power> <matrix.txt>\n"
        "\n"
        "Popis parametru:\n"
        "  -h\t\tVypise tuto napovedu.\n"
        "  --test\tProvede syntaktickou kontrolu souboru.\n"
        "  --vadd\tProvede vektorovy soucet.\n"
        "  --vscal\tProvede skalarni soucin vektoru.\n"
        "  --mmult\tProvede soucin matic.\n"
        "  --mexpr\tProvede maticovy vyraz.\n"
        "  --eight\tVyhleda vektor v matici.\n"
        "  --bubbles\tSpocita bubliny v matici.\n"
        "  --extbubbles\tSpocita bubliny v 3D matici.\n"
        "  --maze\tVyhleda cestu z bludiste v matici.\n"
        "  --carom\tSleduje cestu kulecnikove koule po stole v matici.\n"
        "\n"
        "Parametr coord se zadava ve formatu 'row,col'.\n"
        "Parametr dir muze nabyvat hodnot: S SV JV J JZ SZ.\n"
        "Parametr power je celociselny typ.\n");
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

    if (argc >= 2) {
        if (strcmp(argv[1], PARAMS_HELP) == 0) {
            params->progState = PS_HELP;
            if (argc != 2) { return E_INVALID_PARAM; }
        }
        else if (strcmp(argv[1], PARAMS_TEST) == 0) {
            params->progState = PS_TEST;
            if (argc != 3) { return E_INVALID_PARAM; }
            else { params->filename1 = argv[2]; }
        }
        else if (strcmp(argv[1], PARAMS_VECTOR_ADDITION) == 0) {
            params->progState = PS_VECTOR_ADDITION;
            if (argc != 4) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[2];
                params->filename2 = argv[3];
            }
        }
        else if (strcmp(argv[1], PARAMS_VECTOR_SCALAR_MULTIPLICATION) == 0) {
            params->progState = PS_VECTOR_SCALAR_MULTIPLICATION;
            if (argc != 4) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[2];
                params->filename2 = argv[3];
            }
        }
        else if (strcmp(argv[1], PARAMS_MATRIX_MULTIPLICATION) == 0) {
            params->progState = PS_MATRIX_MULTIPLICATION;
            if (argc != 4) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[2];
                params->filename2 = argv[3];
            }
        }
        else if (strcmp(argv[1], PARAMS_MATRIX_EXPRESSION) == 0) {
            params->progState = PS_MATRIX_EXPRESSION;
            if (argc != 4) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[2];
                params->filename2 = argv[3];
            }
        }
        else if (strcmp(argv[1], PARAMS_EIGHT) == 0) {
            params->progState = PS_EIGHT;
            if (argc != 4) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[2];
                params->filename2 = argv[3];
            }
        }
        else if (strcmp(argv[1], PARAMS_BUBBLES) == 0) {
            params->progState = PS_BUBBLES;
            if (argc != 3) { return E_INVALID_PARAM; }
            else { params->filename1 = argv[2]; }
        }
        else if (strcmp(argv[1], PARAMS_BUBBLES_3D) == 0) {
            params->progState = PS_BUBBLES_3D;
            if (argc != 3) { return E_INVALID_PARAM; }
            else { params->filename1 = argv[2]; }
        }
        else if (strcmp(argv[1], PARAMS_MAZE) == 0) {
            params->progState = PS_MAZE;
            if (argc != 3) { return E_INVALID_PARAM; }
            else { params->filename1 = argv[2]; }
        }
        else if (strcmp(argv[1], PARAMS_CAROM) == 0) {
            params->progState = PS_CAROM;
            if (argc != 6) { return E_INVALID_PARAM; }
            else {
                params->filename1 = argv[5];
            }
        }
        else {
            return E_UNKNOWN_PARAM;
        }
    }
    else {
        params->progState = PS_HELP;
    }

    return E_OK;
}

void bubbles_markNeighborDir(const Matrix* mat, size_t row, size_t col, CoordArr* coords, Direction dir)
{
    assert(mat != NULL && mat->items != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);
    assert(row < mat->rows);
    assert(col < mat->cols);
    assert(coords != NULL && coords->coords != NULL);

    switch (dir) {
        case LeftToRight:
            if (col + 1 >= mat->cols) {
                return;
            }
            ++col;
            break;

        case RightToLeft:
            if (col == 0) {
                return;
            }
            --col;
            break;

        case TopDown:
            if (row + 1 >= mat->rows) {
                return;
            }
            ++row;
            break;

        case BottomUp:
            if (row == 0) {
                return;
            }
            --row;
            break;

        default:
            assert(false);
            return;
    }

    if (matrix_getitem(mat, row, col) == 0 &&
       !bubbles_isMarked(coords, row, col))
    {
        bubbles_markNeighbor(mat, row, col, coords);
    }
}

void bubbles_markNeighbor(const Matrix* mat, size_t row, size_t col, CoordArr* coords)
{
    assert(mat != NULL && mat->items != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);
    assert(row < mat->rows);
    assert(col < mat->cols);
    assert(coords != NULL && coords->coords != NULL);

    struct Coordinates* current = &coords->coords[coords->count++];
    current->row = row;
    current->col = col;

    bubbles_markNeighborDir(mat, row, col, coords, LeftToRight);
    bubbles_markNeighborDir(mat, row, col, coords, RightToLeft);
    bubbles_markNeighborDir(mat, row, col, coords, BottomUp);
    bubbles_markNeighborDir(mat, row, col, coords, TopDown);
}

bool bubbles_isMarked(const CoordArr* coords, size_t row, size_t col)
{
    assert(coords != NULL && coords->coords != NULL);

    for (size_t i = 0; i < coords->count; ++i) {
        if (coords->coords[i].row == row && coords->coords[i].col == col) {
            return true;
        }
    }

    return false;
}

int bubbles(const Matrix* mat)
{
    assert(mat != NULL && mat->items != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);

    int count = 0;
    CoordArr coords = { .count = 0, .coords = NULL };
    coords.coords = (struct Coordinates *)calloc(mat->rows * mat->cols, sizeof(struct Coordinates));
    if (coords.coords == NULL) {
        return -1;
    }

    // topdown
    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->cols; ++j) {
            int item = matrix_getitem(mat, i, j);
            if (item < 0) {
                free(coords.coords);
                return -1;
            }
            if (item == 0 && !bubbles_isMarked(&coords, i, j)) {
                ++count;
                bubbles_markNeighbor(mat, i, j, &coords);
            }
        }
    }

    free(coords.coords);
    return count;
}

bool eight_check(const Matrix* mat, const Vector* vect, size_t row, size_t col, Direction dir)
{
    assert(mat != NULL && mat->items != NULL);
    assert(vect != NULL && vect->items != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);
    assert(vect->count > 0);
    assert(row < mat->rows);
    assert(col < mat->cols);

    // osetreni rozsahu
    switch (dir) {
        case LeftToRight:
            if (col + vect->count > mat->cols) {
                return false;
            }
            break;

        case RightToLeft:
            if (col + 1 < vect->count) {
                return false;
            }
            break;

        case TopDown:
            if (row + vect->count > mat->rows) {
                return false;
            }
            break;

        case BottomUp:
            if (row + 1 < vect->count) {
                return false;
            }
            break;

        case TopRight:
            if (col + vect->count > mat->cols || row + 1 < vect->count) {
                return false;
            }
            break;

        case BottomRight:
            if (col + vect->count > mat->cols || row + vect->count > mat->rows) {
                return false;
            }
            break;

        case TopLeft:
            if (col + 1 < vect->count || row + 1 < vect->count) {
                return false;
            }
            break;

        case BottomLeft:
            if (col + 1 < vect->count || row + vect->count > mat->rows) {
                return false;
            }
            break;

        default:
            assert(false);
            return false;
    }

    for (size_t i = 0; i < vect->count; ++i) {
        if (matrix_getitem(mat, row, col) != vect->items[i]) {
            return false;
        }

        switch (dir) {
            case LeftToRight:
                ++col;
                break;

            case RightToLeft:
                --col;
                break;

            case TopDown:
                ++row;
                break;

            case BottomUp:
                --row;
                break;

            case TopRight:
                ++col;
                --row;
                break;

            case BottomRight:
                ++col;
                ++row;
                break;

            case TopLeft:
                --col;
                --row;
                break;

            case BottomLeft:
                --col;
                ++row;
                break;

            default:
                assert(false);
                return false;
        }
    }

    return true;
}

bool eight(const Matrix* mat, const Vector* vect)
{
    assert(mat != NULL && mat->items != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);
    assert(vect != NULL && vect->items != NULL);
    assert(vect->count > 0);

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->cols; ++j) {
            if (eight_check(mat, vect, i, j, LeftToRight)) { return true; }
            if (eight_check(mat, vect, i, j, RightToLeft)) { return true; }
            if (eight_check(mat, vect, i, j, BottomUp)) { return true; }
            if (eight_check(mat, vect, i, j, TopDown)) { return true; }
            if (eight_check(mat, vect, i, j, TopRight)) { return true; }
            if (eight_check(mat, vect, i, j, TopLeft)) { return true; }
            if (eight_check(mat, vect, i, j, BottomRight)) { return true; }
            if (eight_check(mat, vect, i, j, BottomLeft)) { return true; }
        }
    }

    return false;
}

// hlavni funkce main
int main(int argc, char* argv[])
{
    ProgParams state;
    Error err = parseParams(argc, argv, &state);
    CHECK_ERROR(err);

    switch (state.progState) {
        case PS_HELP:
            printHelp();
            break;

        case PS_TEST:
            if (!test(state.filename1, stdout)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            break;

        case PS_VECTOR_ADDITION: {
            Vector vect1 = { .count = 0, .items = NULL };
            Vector vect2 = { .count = 0, .items = NULL };
            if (!vector_load(&vect1, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (!vector_load(&vect2, state.filename2)) {
                vector_free(&vect1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (vect1.count != vect2.count) {
                vector_free(&vect2);
                vector_free(&vect1);
                noop_print(stdout);
                return EXIT_SUCCESS;
            }

            Vector vectDest = { .count = vect1.count, .items = NULL };
            if (!vector_alloc(&vectDest)) {
                vector_free(&vect2);
                vector_free(&vect1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }

            vector_add(&vect1, &vect2, &vectDest);
            vector_print(&vectDest, stdout);

            vector_free(&vectDest);
            vector_free(&vect2);
            vector_free(&vect1);
        }   break;

        case PS_VECTOR_SCALAR_MULTIPLICATION: {
            Vector vect1 = { .count = 0, .items = NULL };
            Vector vect2 = { .count = 0, .items = NULL };
            if (!vector_load(&vect1, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (!vector_load(&vect2, state.filename2)) {
                vector_free(&vect1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (vect1.count != vect2.count) {
                vector_free(&vect2);
                vector_free(&vect1);
                noop_print(stdout);
                return EXIT_SUCCESS;
            }

            fprintf(stdout, "%d\n", vector_scalar_mult(&vect1, &vect2));

            vector_free(&vect2);
            vector_free(&vect1);
        }   break;

        case PS_MATRIX_MULTIPLICATION: {
            Matrix mat1 = { .rows = 0, .cols = 0, .items = NULL };
            Matrix mat2 = { .rows = 0, .cols = 0, .items = NULL };
            if (!matrix_load(&mat1, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (!matrix_load(&mat2, state.filename2)) {
                matrix_free(&mat1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (mat1.cols != mat2.rows) {
                matrix_free(&mat2);
                matrix_free(&mat1);
                noop_print(stdout);
                return EXIT_SUCCESS;
            }

            Matrix matDest = { .rows = mat1.rows, .cols = mat2.cols, .items = NULL };
            if (!matrix_alloc(&matDest)) {
                matrix_free(&mat2);
                matrix_free(&mat1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }

            matrix_mult(&mat1, &mat2, &matDest);
            matrix_print(&matDest, stdout);

            matrix_free(&matDest);
            matrix_free(&mat2);
            matrix_free(&mat1);
        }   break;

        case PS_MATRIX_EXPRESSION: {
            Matrix mat1 = { .rows = 0, .cols = 0, .items = NULL };
            Matrix mat2 = { .rows = 0, .cols = 0, .items = NULL };
            if (!matrix_load(&mat1, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (!matrix_load(&mat2, state.filename2)) {
                matrix_free(&mat1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (mat1.cols != mat2.rows) {
                matrix_free(&mat2);
                matrix_free(&mat1);
                noop_print(stdout);
                return EXIT_SUCCESS;
            }

            Matrix matAcu = { .rows = mat1.rows, .cols = mat2.cols, .items = NULL };
            if (!matrix_alloc(&matAcu)) {
                matrix_free(&mat2);
                matrix_free(&mat1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            matrix_mult(&mat1, &mat2, &matAcu);

            if (matAcu.cols != mat1.rows) {
                matrix_free(&matAcu);
                matrix_free(&mat2);
                matrix_free(&mat1);
                noop_print(stdout);
                return EXIT_SUCCESS;
            }

            Matrix matDest = { .rows = matAcu.rows, .cols = mat1.cols, .items = NULL };
            if (!matrix_alloc(&matDest)) {
                matrix_free(&matAcu);
                matrix_free(&mat2);
                matrix_free(&mat1);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            matrix_mult(&matAcu, &mat1, &matDest);

            matrix_print(&matDest, stdout);
            
            matrix_free(&matDest);
            matrix_free(&matAcu);
            matrix_free(&mat2);
            matrix_free(&mat1);
        }   break;

        case PS_BUBBLES: {
            Matrix mat = { .rows = 0, .cols = 0, .items = NULL };
            if (!matrix_load(&mat, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }

            int countBubbles = bubbles(&mat);
            if (countBubbles == -1) {
                matrix_free(&mat);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            else {
                fprintf(stdout, "%d\n", countBubbles);
            }
            
            matrix_free(&mat);
        }   break;

        case PS_EIGHT: {
            Matrix mat = { .rows = 0, .cols = 0, .items = NULL };
            Vector vect = { .count = 0, .items = NULL };

            if (!vector_load(&vect, state.filename1)) {
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (!matrix_load(&mat, state.filename2)) {
                vector_free(&vect);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }
            if (vect.count > mat.rows || vect.count > mat.cols) {
                vector_free(&vect);
                matrix_free(&mat);
                CHECK_ERROR(E_UNKNOWN_ERROR);
            }

            fprintf(stdout, "%s\n", eight(&mat, &vect) ? "true" : "false");
            
            vector_free(&vect);
            matrix_free(&mat);
        }   break;

        default:
            printError(E_NOT_IMPLEMENTED);
            return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
