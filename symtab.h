#ifndef _SYMTAB_H
#define _SYMTAB_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>

const long int    EMPTY=-3; //flag for empty symbol tab
const long int  INSERT_S=2;
const long int  INSERT_F=3;
const long int DUPLICATE=-11;   //flag for duplicate symtab search request foe sym,add pairs
const long int  NOT_SET=-12;    //flag for valid label whose forward reference has not yet solved
const long int NOT_FOUND=-13;   //flag for invalid node for search..indicating to
const long int  FOR_INSERT=7;

struct instruction                //structure holds the instruction
{
    char *label;               //should end with :
    char *opcode;
    char *operand;
    char *line;         
    int comment;                  //set to one if line is a comment line
    int error_code;             
    long int machinecode;
    long int address;            //address of the current instruction
    long int loc;
    struct instruction *next;
};

typedef struct instruction inst;
struct Symtab
{
  char* symbol;   //the Label field
  int address;    //the real defined address using RESW or RESB etc...
  int** addr;     //list of LOCCTR having $symbol as label
  int rcount;     //number of addresses where the LOCCTR is saved to resolve 
  int ncount;     //number of nodes where machine code has to be replaced with address
  int done;       //set when address is valid
  struct Symtab* next;
  inst** node; //list nodes having $symbol as label
};
typedef struct Symtab symtab;

symtab* START =NULL;
symtab* END   =NULL;
symtab* temp  =NULL;
inst* nodetemp=NULL;

int insert_into_symtab(char sym[],int add)
{
    int len=0,count,i=0,nc,rc;
    
    // it is used to traverse the list of labels finds the match with "sym"to replace machinecode by the received address
    //"add". Once match is found it traverses the container list of instruction type nodes till the limit of ncount(node count)
    //is valid and set the temp->(node[i]->machinecode)=add	  
    //printf("\nWithin FOR_INSERT\n");
    
    if(START==NULL) //symtab is empty
    {
	
	if( (temp=(symtab*)malloc(sizeof(symtab)))==NULL)
	{
	    printf("\n[*]Malloc failed in:%s at line:%d of file:%s\n",__FUNCTION__, __LINE__,__FILE__);
	    exit(-1);
	}
	START=temp;
	len=strlen(sym);
	if( (temp->symbol=malloc(len+1))==NULL)
	{
	    printf("\n[*]Malloc failed in:%s at line:%d of file:%s\n",__FUNCTION__, __LINE__,__FILE__);
	    exit(-1);
	}
	temp->address=add;
	
	strncpy(temp->symbol,sym,len);
	temp->symbol[len]='\0';
	temp->done=1;
	END=temp;
	START->next=END;
	END->next=NULL;
    }
    else //Add the data to the end of the queue
    {
	if( (temp=(symtab*)malloc(sizeof(symtab)))==NULL)
	{
	    printf("\n[*]Malloc failed in:%s at line:%d of file:%s",__FUNCTION__,__LINE__,__FILE__);
	    exit(-1);
	}
	len=strlen(sym);
	if( (temp->symbol=malloc(len+1))==NULL)
	{
	    printf("\nMalloc failed in:%s at line:%d of file:%s",__FUNCTION__,__LINE__,__FILE__);
	    exit(-1);
	}
	temp->address=add;
	strncpy(temp->symbol,sym,len);
	temp->symbol[len]='\0';
	temp->done=1;
	END->next=temp;
	END=END->next;
    }
    return INSERT_S;    
}

long int search_symtab(char* sym )
{
    if(START==NULL)
    {
//	printf("%s","\n!!!...The symtab is empty...!!! \nEntering value....");
	return(NOT_FOUND);
    }
    else
    {
	temp=START;
	while(temp!=NULL && temp->symbol!=NULL)
	{
	    
	    if( (strcmp(temp->symbol,sym)==0) && (temp->done==1))
	    {
		return temp->address;
	    }
	    temp=temp->next;
	}
	if(temp==NULL)
	    return NOT_FOUND;
    }
}

void freesymtab()
{
    symtab* obj;
    symtab* otemp;
    for(obj=START;obj!=NULL;)
    {
	
	printf("\nFreeing Symtab for:%s\n",obj->symbol);
	otemp=obj;
	free(otemp->symbol);
	obj=obj->next;
	free(otemp);
    }         
}
#endif
