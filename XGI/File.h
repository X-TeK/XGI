#ifndef File_h
#define File_h

#include <stdio.h>
#include <SDL2/SDL.h>

typedef enum FileMode
{
	FileModeRead, FileModeReadBinary,
	FileModeWrite, FileModeWriteBinary,
	FileModeAppend, FileModeAppendBinary,
	FileModeReadWrite, FileModeReadWriteBinary,
} FileMode;

typedef struct File
{
	const char * Path;
	unsigned long Size;
	SDL_RWops * RW;
} * File;

File FileOpen(const char * filePath, FileMode mode);
unsigned long FileSize(File file);
void FileRead(File file, unsigned long offset, unsigned long size, void * data);
void FileWrite(File file, unsigned long offset, unsigned long size, void * data);
bool FileExists(const char * path);
void FileClose(File file);

#endif
