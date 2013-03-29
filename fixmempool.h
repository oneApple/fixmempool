/*
 * fixmempool.h
 *
 *  Created on: 2013-3-29
 *      Author: keym
 */

#ifndef FIXMEMPOOL_H_
#define FIXMEMPOOL_H_

#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>

template<class type>
union mem_node{
	type node;
	union mem_node<type> *next;
};

template<typename type>
struct mem_block{
	mem_node<type> *head;
	mem_node<type> *tail;
	struct mem_block<type> *next;
};

template<typename type>
class mem_pool{
private:
	enum {MAXNUM = 2};
	static mem_pool<type> *Instance;
	mem_pool(int num = MAXNUM):head(NULL),tail(NULL),nhead(NULL),total(0),left(0),block_size(num){}

	class release{
	public:
		~release()
		{
			if(Instance != NULL)
			{
				delete Instance;
				Instance = NULL;
			}
		}
	};
	static release rel;
private:
	mem_block<type> *head;
	mem_block<type> *tail;
	mem_node<type> *nhead;
	unsigned total;
	unsigned left;
	unsigned block_size;
private:
	int add_block(int num);
public:
	void destory()
	{
		mem_block<type> *next,*cur;
		cur = this->head;
		next = NULL;
		while(cur != NULL)
		{
			next = cur->next;
			free(cur->head);
			free(cur);
			cur = next;
		}
		this->head = this->tail = NULL;
		this->nhead = NULL;
		this->total = this->left = this->block_size = 0;
	}
	static mem_pool<type> *getInstance()
		{
		if(mem_pool<type>::Instance == NULL)
		{
			mem_pool<type>::Instance = new mem_pool<type>;
		}
		return mem_pool<type>::Instance;
		}
public:
	type *mem_pool_alloc();
	void mem_pool_release(type *ret);
	~ mem_pool()
	{
		this->destory();
	}

};

template<typename type>
mem_pool<type>* mem_pool<type>::Instance = NULL;

template<class type>
typename mem_pool<type>::release mem_pool<type>::rel;

template<typename type>
int mem_pool<type>::add_block(int num)
{
	mem_block<type> *btemp = new mem_block<type>;
	if((btemp->head = (mem_node<type> *)malloc(sizeof(mem_node<type>) * num)) == NULL)
	{
		//分配失败
		return 0;
	}
	btemp->tail = btemp->head + this->block_size - 1;
	mem_node<type> *p;
	//将数组组成链表
	for(p = btemp->head;p < btemp->tail;++ p)
	{
		p->next = (p + 1);
	}
	p->next = NULL;

	btemp->next = this->head;
	this->head = btemp;
	this->nhead = btemp->head;

	if(this->tail == NULL)
	{
		this->tail = btemp;
	}
	this->total += num;
	this->left  += num;
	return 1;
}

template<typename type>
type *mem_pool<type>::mem_pool_alloc()
{
	if(this->nhead == NULL)
	{
		if(!this->add_block(this->block_size))
		{
			return 0;
		}
	}
	mem_node<type> *p = this->nhead;
	this->nhead = p->next;
	-- this->left;
	printf("total:%d,left:%d\n",this->total,this->left);
	return &p->node;

}

template<typename type>
void mem_pool<type>::mem_pool_release(type *ret)
{
	if(ret == NULL)
	{
		return;
	}
	((mem_node<type> *)ret)->next = this->nhead;
	this->nhead = (mem_node<type> *)ret;
	++ this->left;
	printf("total:%d,left:%d\n",this->total,this->left);
}


#endif /* FIXMEMPOOL_H_ */
