#ifndef List_h
#define List_h

#include <stdbool.h>

/// A dynamic array of void pointers
typedef struct List
{
	void ** Data;
	int Capacity;
	int Count;
} * List;

/// Allocates and initializes a list
/// \return The newly created list
List ListCreate(void);

/// Gets he number of elements in a list
/// \param list The list
/// \return The number of values in the list
int ListCount(List list);

/// Inserts a value at a specified index
/// \param list The list to insert at
/// \param value The value to insert
/// \param index The index to insert at
void ListAdd(List list, void * value, int index);

/// Removes the value at the specifed index
/// \param list The list to remove from
/// \param index The index to remove from
void ListRemove(List list, int index);

/// Adds a value to the end of a list
/// \param list The list to append at
/// \param value The value to append
void ListPush(List list, void * value);

/// Removes the last value added, if any, from a list
/// \param list The list to remove from
void ListPop(List list);

/// Removes the first instance of a value from a list
/// \param list The list to remove from
/// \param value The value to remove
void ListRemoveFirst(List list, void * value);

/// Removes the last instance of a value from a list
/// \param list The list to remove from
/// \param value The value to remove
void ListRemoveLast(List list, void * value);

/// Removes all instances of a value from a list
/// \param list The list to remove from
/// \param value The value to remove
void ListRemoveAll(List list, void * value);

/// Returns the value at the specifeid index in a list
/// \param list The list to index
/// \param index The desired index
/// \return The value at the specified index
void * ListIndex(List list, int index);

/// Sets the value at the specified index in a list
/// \param list The list to set
/// \param index The index to set at
/// \param value The value to set
void ListSetIndex(List list, int index, void * value);

/// Checks if the list contains the specified value
/// \param list The list to check
/// \param value The value to look for
/// \return Whether or not the list contains that value
bool ListContains(List list, void * value);

/// Removes all values from a list
/// \param list The list to clear
void ListClear(List list);

/// Cleans up and frees the allocated memory from a list
/// \param list The list to destroy
void ListDestroy(List list);

#endif
