#ifndef File_h
#define File_h

#include <stdio.h>
#include <SDL2/SDL.h>

typedef enum FileMode
{
	/// Reads text from a file.
	/// The file must exist
	FileModeRead,
	/// Reads binary data from a file.
	/// The file must exist
	FileModeReadBinary,
	/// Writes text to a file.
	/// If the file exists then it's erased.
	/// If the file doesn't exist then it's created
	FileModeWrite,
	/// Writes binary data to a file.
	/// If the file exists then it's erased.
	/// If the file doesn't exist then it's created
	FileModeWriteBinary,
	/// Appends text to the end of a file.
	/// The file is created if it doesn't exist
	FileModeAppend,
	/// Appends binary data to the end of a file.
	/// The file is created if it doesn't exist
	FileModeAppendBinary,
	/// Reads and writes text to a file.
	/// The file must exist
	FileModeReadWrite,
	/// Reads and writes binary data to a file.
	/// The file must exist
	FileModeReadWriteBinary,
} FileMode;

typedef struct File
{
	const char * Path;
	unsigned long Size;
	SDL_RWops * RW;
} * File;

/// Opens a file from the given path for read/write operations
/// \param filePath The file path
/// \param mode The read/write mode to open the file with
/// \return The file object
File FileOpen(const char * filePath, FileMode mode);

/// Gets the size from a file object
/// \param file The file object
/// \return The file size
unsigned long FileGetSize(File file);

/// Gets the path from a file object
/// \param file The file object
/// \return The file path
const char * FileGetPath(File file);

/// Reads data from a file
/// \param file The file object to read from
/// \param offset The offset in bytes to read from
/// \param size The size in bytes to read
/// \param data A preallocated pointer that the memory will be copied to
void FileRead(File file, unsigned long offset, unsigned long size, void * data);

/// Writes data to a file
/// \param file The file object to write to
/// \param offset The offset in bytes to read at
/// \param size The size in bytes to write
/// \param data A pointer to the data to write
void FileWrite(File file, unsigned long offset, unsigned long size, void * data);

/// Checks if a file exists
/// \param path The path to check
/// \return Either true or false if it exists
bool FileExists(const char * path);

/// Closes the file object
/// \param file The file object to close
void FileClose(File file);

#endif
