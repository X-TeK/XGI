#include <stdlib.h>
#include <stdio.h>
#include "List.h"

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

int ListCount(List list)
{
	return list->Count;
}

void ListInsert(List list, void * value, int index)
{
	if (index <= list->Count && index >= 0)
	{
		list->Count++;
		if (list->Count == list->Capacity)
		{
			list->Capacity *= 2;
			list->Data = realloc(list->Data, list->Capacity * sizeof(void *));
		}
		for (int j = list->Count - 2; j >= index; j--) { list->Data[j + 1] = list->Data[j]; }
		list->Data[index] = value;
	}
	else
	{
		printf("index out of bounds\n");
	}
}

void ListOutsert(List list, int index)
{
	if (index < list->Count && index >= 0)
	{
		for (int j = index + 1; j < list->Count; j++) { list->Data[j - 1] = list->Data[j]; }
		list->Count--;
		if (list->Count == list->Capacity / 2 - 1)
		{
			list->Capacity /= 2;
			list->Data = realloc(list->Data, list->Capacity * sizeof(void *));
		}
	}
}

void ListPush(List list, void * value)
{
	ListInsert(list, value, list->Count);
}

void ListPop(List list)
{
	ListOutsert(list, list->Count - 1);
}

void ListRemoveAll(List list, void * value)
{
	for (int i = 0; i < list->Count; i++)
	{
		if (ListIndex(list, i) == value)
		{
			ListOutsert(list, i);
			i--;
		}
	}
}

void * ListIndex(List list, int index)
{
	if (index < list->Count && index >= 0) { return list->Data[index]; }
	else
	{
		printf("Index out of bounds\n");
		exit(-1);
		return NULL;
	}
}

void ListSetIndex(List list, int index, void * value)
{
	if (index < list->Count && index >= 0) { list->Data[index] = value; }
}

bool ListContains(List list, void * value)
{
	for (int i = 0; i < list->Count; i++)
	{
		if (ListIndex(list, i) == value) { return true; }
	}
	return false;
}

void ListClear(List list)
{
	for (int i = list->Count - 1; i >= 0; i--)
	{
		ListOutsert(list, i);
	}
}

/*
static List RemoveDuplicates(List list)
{
	List new_list = List2.Create();
	for (int i = 0; i < list->Count; i++)
	{
		void * value = List2.Index(list, i);
		if (!List2.Contains(new_list, value))
		{
			List2.Add(new_list, value);
		}
	}
	return new_list;
}
 */

void ListDestroy(List list)
{
	free(list->Data);
	free(list);
}
