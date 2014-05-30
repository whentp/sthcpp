#ifndef _FILE_H__
#define _FILE_H__

int getFileLength(const char* filename);
void loadFile(const char* filename,simplestring* abc);
void createEmptyFile(const char* filename);
void ensureFileExists(const char* filename);

#endif