/*
 * main.cpp
 *
 *  Created on: 2013-3-29
 *      Author: keym
 */

#include"fixmempool.h"

#include<stdio.h>

int main()
{
	mem_pool<int>* instance = mem_pool<int>::getInstance();

	int i;
	int **p = new int*[10];
	for(i = 0;i < 9;++ i )
	{
		p[i] = instance->mem_pool_alloc();
		*p[i] = 1;
		printf("%d\n",*p[i]);
	}
	for(i = 0;i < 9;++ i )
	{
		instance->mem_pool_release(p[i]);
	}
	instance->destory();
	return 0;
}
