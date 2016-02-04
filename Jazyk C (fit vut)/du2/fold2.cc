/*
 * fold2.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-06
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 1b).
 */

/* ==================================================================== */
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/* ==================================================================== */
static int width = 80;
static int cut = 0;
static std::string filename;

/* ==================================================================== */
static int process_file(std::istream &is)
{
    is.sync_with_stdio(false);

    bool first_cut = true;
    int cursor = 0;
    for (std::string line; std::getline(is, line); ) {
        if (line.empty()) {
            if (cursor != 0) printf("\n");
            std::cout << "\n";
            cursor = 0;
            continue;
        }

        std::istringstream iss(line);
        for (std::string word; iss >> word; ) {
            if (cut && word.length() > static_cast<size_t>(width)) {
                word.erase(width);

                if (first_cut) {
                    std::cerr << "slovo bylo oriznuto" << std::endl;
                    first_cut = false;
                }
            }

            if (cursor + word.length() + 1 > static_cast<size_t>(width)) {
                std::cout << "\n";
                cursor = 0;
            }
            else if (cursor != 0) {
                std::cout << " ";
                ++cursor;
            }

            std::cout << word;
            cursor += word.length();
        }
    }
    std::cout << std::endl;

    if (is.bad()) {
        std::cerr << "chyba pri cteni" << std::endl;
        return 1;
    }

    return 0;
}

/* ==================================================================== */
static int parse_cmdargs(int argc, char *argv[])
{
    while (--argc) {
        const char *p = *(++argv);

        if (strcmp(p, "-c") == 0) {
            cut = 1;
        }
        else if (strcmp(p, "-w") == 0) {
            if (--argc == 0) return 1;
            if (1 != sscanf(*(++argv), "%d", &width)) { return 1; }
            if (width <= 0) { return 1; }
        }
        else if (strncmp(p, "-", 1) == 0) {
            return 2;
        }
        else {
            break;
        }
    }

    /* ================================================================ */
    if (argc == 0) {
        return 0;               // read from stdin
    }
    else if (argc == 1) {
        filename = *argv;       // read from file
        return 0;
    }
    else {
        return 2;
    }
}

/* ==================================================================== */
int main(int argc, char *argv[])
{
    switch (parse_cmdargs(argc, argv)) {
    /* ================================================================ */
    case 0:
        break;
    /* ================================================================ */
    default:
        return 1;
    /* ================================================================ */
    case 1:
        std::cerr << "chyba pri zadani parametru -w" << std::endl;
        return 1;
    /* ================================================================ */
    case 2:
        std::cerr << "chyba, neznamy parametr" << std::endl;
        return 1;
    /* ================================================================ */
    }

    if (filename.empty()) {
        process_file(std::cin);
    }
    else {
        std::ifstream is(filename);
        process_file(is);
    }

    return 0;
}
