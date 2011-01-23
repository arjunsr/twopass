#ifndef _LOADHASH_H
#define _LOADHASH_H


#include "hash.h"
#include<stdio.h>

#define LINESIZE 100
#define OPEN_OPTAB_FAILED -1

static char *readline(FILE *);


void parse(const char *line,char *key,void *value)
{
  char dat[4];
  sscanf(line,"%s%s",key,dat);
  *(int *)value=(int)strtol(dat,NULL,16);
}


int load_hash_table(Hashtab *hash_t)
{
  FILE *opfile=NULL;
  if(!(opfile=fopen("optab.txt","r")))
    {
      printf("\n Cannot open file optab.txt \n");
      return (OPEN_OPTAB_FAILED);
    }

  char *line=NULL;
 readline(opfile);
  char key[10];
  int value=0;

  while(!feof(opfile))
    {
      line=readline(opfile);
      if(!strcmp(line,"END"))
	{break;}
      parse(line,key,&value);
      insert(hash_t,key,&value);
    }
  fclose(opfile);
  return (0);
}


static char *readline(FILE *fpt)
{
  static char buff[LINESIZE];
  int i=0;
  while(!feof(fpt))
    {
      if((buff[i++]=getc(fpt))=='\n')
	{break;}
      
    }
  buff[i-1]='\0';
  return buff;
}

/*
int main()
{
  Hashtab *optab=NULL;
  FILE *opfile=NULL;
  if(!(opfile=fopen("optab.txt","r")))
    {
      printf("\n Cannot open file optab.txt \n");
      return (OPEN_OPTAB_FAILED);
    }

  char *line=NULL,temp[10];
  line=readline(opfile);
  int sizeofoptab=0;
  sscanf(line,"%s%d",temp,&sizeofoptab);
  optab=create_hashtab(sizeofoptab);

   if(!load_hash_table(optab))
    {
      printf("\n Opcode table loaded\n");
    }

   //////////////////////////////////////////////////
  char op[10];
  int data=0;
  while(1)
    {
      printf("\nInput opcode:");
      scanf("%s",op);
      data=*((int *)get_data(optab,op));
      printf("\n******%x********\n",data);
    }
  return(0);
}
*/
#endif
