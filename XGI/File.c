#include <stdbool.h>
#include "File.h"
#include "log.h"

static void ValidateFileObject(File file)
{
	if (file == NULL)
	{
		log_fatal("Trying to perform operations on an uninitialized File object.\n");
		exit(1);
	}
}

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
		default: log_fatal("Trying to open a file with an unknown FileMode %i.\n", mode); exit(1);
	}
	file->RW = SDL_RWFromFile(path, fileMode);
	if (file->RW == NULL)
	{
		log_fatal("Trying to open file %s that doesn't exist.\n", path);
		exit(1);
	}
	file->Size = SDL_RWsize(file->RW);
	return file;
}

unsigned long FileGetSize(File file)
{
	ValidateFileObject(file);
	return file->Size;
}

const char * FileGetPath(File file)
{
	ValidateFileObject(file);
	return file->Path;
}

void FileRead(File file, unsigned long offset, unsigned long size, void * data)
{
	ValidateFileObject(file);
	if (offset > file->Size)
	{
		log_fatal("Trying to read from the file, but offset + size (%l + %l) is outside the bounds of the file size %l.\n", offset, size, file->Size);
		exit(1);
	}
	if (data == NULL)
	{
		log_fatal("Trying to copy data from the file into the pointer, but the supplied pointer is NULL.\n");
		exit(1);
	}
	SDL_RWseek(file->RW, offset, RW_SEEK_SET);
	SDL_RWread(file->RW, data, size, 1);
	SDL_RWseek(file->RW, 0, RW_SEEK_SET);
}

void FileWrite(File file, unsigned long offset, unsigned long size, void * data)
{
	ValidateFileObject(file);
	if (offset > file->Size)
	{
		log_fatal("Trying to write to the file, but offset + size (%l + %l) is outside the bounds of the file size %l.\n", offset, size, file->Size);
		exit(1);
	}
	if (data == NULL)
	{
		log_fatal("Trying to copy data from the pointer into the file, but the supplied pointer is NULL.\n", file);
		exit(1);
	}
	SDL_RWseek(file->RW, offset, RW_SEEK_SET);
	SDL_RWwrite(file->RW, data, size, 1);
	SDL_RWseek(file->RW, 0, RW_SEEK_SET);
}

bool FileExists(const char * path)
{
	SDL_RWops * file = SDL_RWFromFile(path, "r");
	if (file == NULL) { return false; }
	else { SDL_RWclose(file); return true; }
}

void FileClose(File file)
{
	ValidateFileObject(file);
	SDL_RWclose(file->RW);
	free(file);
}

char * FileCurrentPath()
{
	return SDL_GetBasePath();
}
