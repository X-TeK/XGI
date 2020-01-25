#ifndef List_h
#define List_h

#include <stdbool.h>

typedef struct List
{
	void ** Data;
	int Capacity;
	int Count;
} * List; //! A dynamic array of void pointers

/*!
 Allocates and initializes a list
 @return The newly created list
 */
List ListCreate(void);

/*!
 Gives the number of values in a list
 @return The number of values in the list
 */
int ListCount(List list);

/*!
 Inserts a value at a specified index
 @param value The value to insert
 @param index The index to insert at
 */
void ListInsert(List list, void * value, int index);

/*!
 Removes the value at the specifed index
 @param index The index to remove from
 */
void ListOutsert(List list, int index);

/*!
 Adds a value to the end of a list
 @param value The value to append
 */
void ListPush(List list, void * value);

//! Removes the last value added, if any, from a list
void ListPop(List list);

/*!
 Removes the first instance of a value from a list
 @param value The value to remove
 */
void ListRemoveFirst(List list, void * value);

/*!
Removes the last instance of a value from a list
@param value The value to remove
*/
void ListRemoveLast(List list, void * value);

/*!
Removes all instances of a value from a list
@param value The value to remove
*/
void ListRemoveAll(List list, void * value);

/*!
 Returns the value at the specifeid index in a list
 @param index The desired index
 @return The value at the specified index
 */
void * ListIndex(List list, int index);

/*!
 Looks if the list contains the specified value
 @param value The value to look for
 @return Whether or not the list contains that value
 */
bool ListContains(List list, void * value);

//! Removes all values from a list
void ListClear(List list);

//! Cleans up and frees the allocated memory from a list
void ListDestroy(List list);

#endif
