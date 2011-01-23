#ifndef _HASH_H
#define _HASH_H


#include<stdio.h>
#include<string.h>
#include<stdlib.h>


struct hnode {
  char *key;
  void *data;
 struct  hnode *next;
};
typedef struct hnode hashnode;

struct htab {
  hashnode **nodes;
  size_t size;
};

typedef struct htab Hashtab;



size_t hashfunc(const char *);
Hashtab *create_hashtab(size_t size);
void destroy_hashtab(Hashtab *);
int insert(Hashtab *,const char *,void *);
void *get_data(Hashtab *,const char *);



Hashtab *create_hashtab(size_t size)
{
  Hashtab *hash_t;
  if(!(hash_t=malloc(sizeof(Hashtab))))
    {
      printf("\nCannot allocate memmory for hash table\n");
      return NULL;
    }
  if(!(hash_t->nodes=calloc(size,sizeof(hashnode))))
    {
      printf("\nCould not allocate memmory for nodes\n");
      free(hash_t);
      return NULL;
    }
  hash_t->size=size;
    return hash_t;
}


void destroy_hashtab(Hashtab *hash_t)
{
  size_t i;
  hashnode *node,*oldnode;
  for(i=0;i<hash_t->size;i++)
    {
      node=hash_t->nodes[i];
      while(node)
	{
	  free(node->data);
	  free(node->key);
	  oldnode=node;
	  node=oldnode->next;
	  free(oldnode);
	}
    }
  free(hash_t->nodes);
  free(hash_t);
  printf("\n Hashtable unloaded\n");
}


int insert(Hashtab *hash_t,const char *key,void *data)
{
  size_t hash;
  hash=hashfunc(key)%hash_t->size;
  hashnode *node;
  node=hash_t->nodes[hash];
  while(node)
    {
      if(!strcmp(node->key,key))
	{
	  *((int *)node->data)=*(int *)data;
	  return(0);
	}
      node=node->next;
    }
  if(!(node=malloc(sizeof(node))))
    {
      return(-1);
    }
  if(!((node->key=malloc(sizeof(key)+1))&&(node->data=malloc(sizeof(int)))))
    {
      return(-2);
    }
  strcpy(node->key,key);
  *((int *)node->data)=*(int *)data;
  node->next=hash_t->nodes[hash];
  hash_t->nodes[hash]=node;
  return(0);
    
}

void *get_data(Hashtab * hash_t,const char *key)
{
  hashnode *node;
  size_t hash;
  hash=hashfunc(key)%hash_t->size;
  node=hash_t->nodes[hash];
  while(node)
    {
      if(!strcmp(node->key,key))
	{
	  return(node->data);
	}
      node=node->next;
    }
  return NULL;
}

size_t hashfunc(const char *key)
{
  size_t hash;
  hash=0;
  char *k;
  int i=0;
  int len=strlen(key);
  k=(char *)key;
  while(i<len)
    {
      hash+=(int)*(key++);
      i++;
    }
  return hash;
}




#endif

