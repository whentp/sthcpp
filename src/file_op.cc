#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "file_op.h"
#include "common.h"

// for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif

#ifdef __posix
#include <dirent.h>
#else
#include <dirent_win.h>
#endif

namespace bible {
using namespace std;

// source: http://stackoverflow.com/questions/10402499/mkdir-c-function
/**
 * Checks if a folder exists
 * @param foldername path to the folder to check.
 * @return true if the folder exists, false otherwise.
 */
bool folder_exists(std::string foldername) {
    struct stat st;
    stat(foldername.c_str(), &st);
    return st.st_mode & S_IFDIR;
}

// source: http://stackoverflow.com/questions/10402499/mkdir-c-function
/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int _mkdir(const char *path) {
#ifdef _WIN32
    return ::_mkdir(path);
#else
#if _POSIX_C_SOURCE
    return ::mkdir(path);
#else
    return ::mkdir(path, 0755); // not sure if this works on mac
#endif
#endif
}

// source: http://stackoverflow.com/questions/10402499/mkdir-c-function
/**
 * Recursive, portable wrapper for mkdir.
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int mkdir(const char *path) {
    std::string current_level = "";
    std::string level;
    std::stringstream ss(path);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')) {
        current_level += level; // append folder to the current level

        // create current level
        if (!folder_exists(current_level) && _mkdir(current_level.c_str()) != 0)
            return -1;

        current_level += "/"; // don't forget to append a slash
    }

    return 0;
}

void ensureFileExists(const char *filename) {
    if (getFileLength(filename) <= 0) {
        createEmptyFile(filename);
    }
}
void createEmptyFile(const char *filename) {
    FILE *tmp = fopen(filename, "w");
    fclose(tmp);
}

///< attentio: add \0 to the end of the file.
void loadTextFile(const char *filename, char *&memblock, size_t &filesize) {
    memblock = NULL;
    size_t size;
    ifstream file (filename, ios::in | ios::binary | ios::ate);

    if (file.is_open()) {
        size = file.tellg();
        filesize = size;
        memblock = new char[size + 1];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        memblock[size] = 0;
        file.close();
        debug_print("read file %s content.", filename);
    }
    else {
        cout << "Unable to open file";
        exit(0);
    }
}

void loadFile(const char *filename, char *&memblock, size_t &filesize) {
    memblock = NULL;
    size_t size;
    ifstream file (filename, ios::in | ios::binary | ios::ate);

    if (file.is_open()) {
        size = file.tellg();
        filesize = size;
        memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();
        debug_print("read file %s content.", filename);
    }
    else {
        cout << "Unable to open file";
        exit(0);
    }
}

size_t getFileLength(const char *filename) {
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        return 0;
    }
    else {
        return buf.st_size;
    }
}

bool checkFileExists(const char *filename) {
    // this method comes from http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

vector<string> *getFilesInDirectory(const char *directory) {
    DIR *dir;
    struct dirent *ent;
    struct stat eStat;
    auto res = new vector<string>;
    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            lstat(ent->d_name, &eStat);
            //cout << ent->d_name << "\t" << S_ISDIR(eStat.st_mode)<<endl;
            //if(!S_ISDIR(eStat.st_mode)){
            res->push_back(ent->d_name);
            //}
        }
        closedir (dir);
    }
    return res;
}

} // end namespace bible.
