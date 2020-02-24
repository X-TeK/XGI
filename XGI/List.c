#include <stdlib.h>
#include <stdio.h>
#include "List.h"
#include "log.h"

List ListCreate()
{
	List list = malloc(sizeof(struct List));
	*list = (struct List)
	{
		.Count = 0,
		.Capacity = 1,
		.Data = malloc(sizeof(void *)),
	};
	return list;
}

static void ValidateListObject(List list)
{
	if (list == NULL)
	{
		log_error("Trying to perform operations on an uninitialized list object.\n");
		exit(1);
	}
}

int ListCount(List list)
{
	ValidateListObject(list);
	return list->Count;
}

void ListAdd(List list, void * value, int index)
{
	ValidateListObject(list);
	if (index < 0 || index > list->Count)
	{
		log_fatal("Trying to add a value to a list, but the index is out of bounds.\n");
		exit(1);
	}
	
	list->Count++;
	if (list->Count == list->Capacity)
	{
		list->Capacity *= 2;
		list->Data = realloc(list->Data, list->Capacity * sizeof(void *));
	}
	for (int j = list->Count - 2; j >= index; j--) { list->Data[j + 1] = list->Data[j]; }
	list->Data[index] = value;
}

void ListRemove(List list, int index)
{
	ValidateListObject(list);
	if (index < 0 || index > list->Count)
	{
		log_fatal("Trying to remove a value from a list, but the index is out of bounds.\n");
		exit(1);
	}
	
	for (int j = index + 1; j < list->Count; j++) { list->Data[j - 1] = list->Data[j]; }
	list->Count--;
	if (list->Count == list->Capacity / 2 - 1)
	{
		list->Capacity /= 2;
		list->Data = realloc(list->Data, list->Capacity * sizeof(void *));
	}
}

void ListPush(List list, void * value)
{
	ValidateListObject(list);
	ListAdd(list, value, list->Count);
}

void ListPop(List list)
{
	ValidateListObject(list);
	if (list->Count == 0)
	{
		log_fatal("Trying to pop from an empty list.\n");
		exit(1);
	}
	ListRemove(list, list->Count - 1);
}

void ListRemoveAll(List list, void * value)
{
	ValidateListObject(list);
	for (int i = 0; i < list->Count; i++)
	{
		if (ListIndex(list, i) == value)
		{
			ListRemove(list, i);
			i--;
		}
	}
}

void * ListIndex(List list, int index)
{
	ValidateListObject(list);
	if (index < 0 || index > list->Count)
	{
		log_fatal("Trying to retrieve a value from a list, but the index is out of bounds.\n");
		exit(1);
	}
	
	return list->Data[index];
}

void ListSetIndex(List list, int index, void * value)
{
	ValidateListObject(list);
	if (index < 0 || index > list->Count)
	{
		log_fatal("Trying to set a value in a list, but the index is out of bounds.\n");
		exit(1);
	}
	
	list->Data[index] = value;
}

bool ListContains(List list, void * value)
{
	ValidateListObject(list);
	for (int i = 0; i < list->Count; i++)
	{
		if (ListIndex(list, i) == value) { return true; }
	}
	return false;
}

void ListClear(List list)
{
	ValidateListObject(list);
	for (int i = list->Count - 1; i >= 0; i--)
	{
		ListRemove(list, i);
	}
}

void ListDestroy(List list)
{
	ValidateListObject(list);
	free(list->Data);
	free(list);
}
