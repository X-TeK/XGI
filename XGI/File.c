#include "File.h"

File FileOpen(const char * path, FileMode mode)
{
	File file = malloc(sizeof(struct File));
	*file = (struct File) { .Path = path, };
	const char * fileMode = "";
	switch (mode)
	{
		case FileModeRead: fileMode = "r"; break;
		case FileModeReadBinary: fileMode = "rb"; break;
		case FileModeWrite: fileMode = "w"; break;
		case FileModeWriteBinary: fileMode = "wb"; break;
		case FileModeAppend: fileMode = "a"; break;
		case FileModeAppendBinary: fileMode = "ab"; break;
		case FileModeReadWrite: fileMode = "r+"; break;
		case FileModeReadWriteBinary: fileMode = "r+b"; break;
	}
	file->RW = SDL_RWFromFile(path, fileMode);
	if (file->RW == NULL)
	{
		exit(-1);
	}
	file->Size = SDL_RWsize(file->RW);
	return file;
}

unsigned long FileSize(File file)
{
	return file->Size;
}

void FileRead(File file, unsigned long offset, unsigned long size, void * data)
{
	SDL_RWseek(file->RW, offset, RW_SEEK_SET);
	SDL_RWread(file->RW, data, size, 1);
	SDL_RWseek(file->RW, 0, RW_SEEK_SET);
}

void FileWrite(File file, unsigned long offset, unsigned long size, void * data)
{
	SDL_RWseek(file->RW, offset, RW_SEEK_SET);
	SDL_RWwrite(file->RW, data, size, 1);
	SDL_RWseek(file->RW, 0, RW_SEEK_SET);
}

void FileClose(File file)
{
	SDL_RWclose(file->RW);
	free(file);
}
