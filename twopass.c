#include "hash.h"
#include "loadhash.h"
#include "symtab.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define LINESIZE          100
#define INVALID_OPCODE   -10
#define MULTI_DEC_LABEL   -11
#define SYMBOL_NOT_SET   -12
#define SYMBOL_NOT_FOUND -13
#define MALLOC_FAILED    -14



int START_ADDRESS=0;                        //holds the start address of program
int END_ADDRESS=0;                          //holds the start address of program
int LOCCTR=0;

/*
struct instruction                //structure holds the instruction
{
  char *label;               //should end with :
  char *opcode;
  char *operand;         
  int comment;                  //set to one if line is a comment line
  int error_code; 
  long int loc;            
  long int machinecode;
  long int address;            //address of the current instruction
  struct instruction *next;
};
*/

//typedef struct instruction inst;

int passone(FILE *,Hashtab */*,Hashtab **/);
void passtwo(inst *,Hashtab *);
int decode(char *,inst *);

void usage()
{
    printf("\n./onepass <inputfile>\n");
}


int main(int argc,char *argv[])
{
  //-------load hash table into memory----------------------///
    Hashtab *optab=NULL;
    //Hashtab *symtab=NULL;
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
//---------end of loading hashtable-------------------///
    if(argc!=2)
    {
	printf("\nNo input files.\n");
	usage();
	exit(0);
    }
    char filename[20];
    int len;
    strcpy(filename,argv[1]);
    len=strlen(filename);
    FILE *source=NULL;
    source=fopen(filename,"r");
    if(source==NULL)
    {
	printf("\n file cannot be opened check weather it exists or its permission.\n");
    }
    //symtab=create_hashtab(33);
    if(!passone(source,optab/*,symtab*/))
    {
	printf("\nProgram contains errors\n");
    }
    destroy_hashtab(optab);
    freesymtab();
    fclose(source);
    exit(0);
}

int passone(FILE *source,Hashtab *hash_t/*,Hashtab *s_tab*/)
{
    char* linebuff;
    int flag=0;
    int error=0;
    linebuff=(char *)malloc(LINESIZE*sizeof(char));
    bzero(linebuff,LINESIZE);
    //read first input line
    linebuff=readline(source); 
    inst *start_node=NULL,*current_node=NULL,*end_node=NULL;
    inst *inode=(inst*)malloc(sizeof(inst)); 
    if(inode==NULL)
    {
	printf("\ncannot allocate memory\n");
	return(MALLOC_FAILED);
    }
    end_node=start_node=inode;               //instruction queue start and end.
    current_node=inode;
    decode(linebuff,inode);
    
    //if OPCODE ='START'
    if(strcmp(inode->opcode,"START")==0)
      {
	//save #OPCODE as the starting address
	START_ADDRESS=strtol(inode->operand,NULL,16);
	//initialize LOCCTR as starting address;
	LOCCTR=START_ADDRESS;
	inode->address=START_ADDRESS;
	inode->loc=LOCCTR;
	inode->label=NULL;
	inode->machinecode=-1;
	inode->address=-1;
	//read next input line
	linebuff=readline(source);	
    }
    //  end of if(OPCODE="START)
    else
    {
      //set LOCCTR to zero
      flag=1; 
      LOCCTR=0;
      START_ADDRESS=0;
      inode->address=LOCCTR;
      inode->loc=LOCCTR;
    }

//while(OPCODE!="END") do 
    while(strcmp(linebuff,"END")!=0)
    {
	if(!flag)
	{
	  inode=(inst *)malloc(sizeof(inst));
	  //inode->line=(char *)malloc(strlen(linebuff)+1);
	  //strcpy(inode->line,linebuff);
	  decode(linebuff,inode);
	  current_node->next=inode;
	  end_node=current_node=inode;
	}
      //if the line is not a comment line
	if(inode->comment!=1)
	{
	    //if there is a symbol in the LABEL field
	    if(inode->label!=NULL)
	    {
		//search SYMTAB for the LABEL
		//long int *found;
		//found=(long int *)get_data(s_tab,inode->label);           arjun
		long int found;  
		found=search_symtab(inode->label);     
		//if(found) then
		if(found==NOT_FOUND)
		{
		    // insert(s_tab,inode->label,LOCCTR);           arjun
		    insert_into_symtab(inode->label,LOCCTR);		
		    
		}
		else
		{
		    inode->error_code=MULTI_DEC_LABEL;
                    //print line number
		}
	    }//end if there is a label in LABEL field
	}//end if not a comment line
      
      //search OPTAB for opcode,return opcode value if found
      int found;
      long int symval=0;
      found=INVALID_OPCODE;
      if(inode->opcode)
	{
	  int *f;
	  f=(int *)get_data(hash_t,inode->opcode);
	  if(f==NULL)
	    {
	      found=INVALID_OPCODE;
	    }
	  else
	    {
	      found=*f;
	    }
	}
      //if OPCODE found then
      if(found!=INVALID_OPCODE)
      {//found contains opcode value
	  //add 3 (instruction length to LOCCTR
	  inode->loc=LOCCTR;
	  LOCCTR+=3;
      }//end, if opcode found
      
      else if(strcmp(inode->opcode,"WORD")==0)
	{
	  
	    // inode->machinecode=strtol(inode->operand,NULL,16);
	    //inode->address=-1;
	  inode->loc=LOCCTR;
	  LOCCTR+=3;
	}
      else if(strcmp(inode->opcode,"RESW")==0)
	{
	  
	    //inode->machinecode=-1;
	    //inode->address=-1;
	  inode->loc=LOCCTR;
	  LOCCTR+=(3*strtol(inode->operand,NULL,16));
	}
      else if(strcmp(inode->opcode,"RESB")==0)
	{
	  
	    //inode->machinecode=-1;
	    //inode->address=-1;
	  inode->loc=LOCCTR;
	  LOCCTR+=strtol(inode->operand,NULL,16);
	}
      else if(strcmp(inode->opcode,"BYTE")==0)
	{
	  //find the length of the constant in bytes 
	  //add length to LOCCTR 
	  int len;
	  len=strlen(inode->operand);
	  inode->loc=LOCCTR;
	  LOCCTR+=len-3;
	}
      else
	{
	  //invalid opcode
	  inode->error_code=INVALID_OPCODE;
	  printf("\n Invalid opcode\n");
	}
      bzero(linebuff,LINESIZE);
      linebuff=readline(source);      
    }//end of while(OPCODE!="END") loop
    
    END_ADDRESS=LOCCTR;
    //write the structure to intermediate file
    FILE *outfile=NULL;
    if(!(outfile=fopen("intermediate","w")))
    {
	printf("\ncannot open output file\n");
	exit(0);
    }
    inst *tnode=NULL,*old=NULL;
    tnode=start_node;
    while(tnode)
    {
	if(tnode->comment!=1)
	{
	    if(tnode->label==NULL)
	    {
		fprintf(outfile,"%lX\t%s\t%s",tnode->loc,tnode->opcode,tnode->operand);
		if(inode->error_code!=0)
		{
		    error=1;
		}
		fprintf(outfile,"\n");
	    }
	   
	}
//	old=tnode;
	tnode=tnode->next;
//	free(old);
    }
    fclose(outfile);
    passtwo(start_node,hash_t);
    return(error);
}



int decode(char *line,inst *ins)
{
    ins->line=(char *)malloc(strlen(line)+1);
    strcpy(ins->line,line);
    char buff1[20];
    char buff2[20];
    char buff3[20];
    char c;
    int no_tokens;
    sscanf(line,"%s",buff1);
    c=buff1[0];
    ins->next=NULL;
    if(c==';')                                          //if the line is a comment line ignore the line
    {
	ins->comment=1;                                 //initializing fields
	ins->label=NULL;
	ins->opcode=NULL;
	ins->operand=NULL;
	ins->machinecode=-1;
	ins->address=-1;
	ins->error_code=0;
	return(0);
    }  
    else                                               //if not a comment line
    {
	ins->comment=0;                                //initializing fields                            
	ins->machinecode=-1;
	ins->address=-1;   
	ins->error_code=0;
	
	no_tokens=sscanf(line,"%s%s%s",buff1,buff2,buff3);
	if(no_tokens==3)
	{
	    ins->label=(char *)malloc(sizeof(char)*strlen(buff1));
	    ins->opcode=(char *)malloc(sizeof(char)*strlen(buff2));
	    ins->operand=(char *)malloc(sizeof(char)*strlen(buff3));
	    if((ins->label==NULL)||(ins->opcode==NULL)||(ins->operand==NULL))
	    {
		free(ins->label);
		free(ins->opcode);
		free(ins->operand);
		printf("\nMalloc failed : decode()\n");
		return(MALLOC_FAILED);
	    }
	    strcpy(ins->label,buff1);
	    strcpy(ins->opcode,buff2);
	    strcpy(ins->operand,buff3);
	}
	else if(no_tokens==2)
	{
	    ins->opcode=(char *)malloc(sizeof(char)*strlen(buff1));
	    ins->operand=(char *)malloc(sizeof(char)*strlen(buff2));
	    if((ins->opcode==NULL)||(ins->operand==NULL))
	    {
		free(ins->opcode);
		free(ins->operand);
		printf("\nMalloc failed : decode()\n");
		return(MALLOC_FAILED);
	    }
	    strcpy(ins->opcode,buff1);
	    strcpy(ins->operand,buff2);
	    ins->label=NULL;
	    /*
	      if((ins->opcode[strlen(ins->opcode)]-1)==':')
	      {
	      if(!(ins->label=(char *)malloc(strlen(ins->opcode)+1)))
	      {
	      printf("\nMalloc failed %s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	      return(0);
	      }
	      strcpy(ins->label,ins->opcode);
	      ins->label[strlen(ins->label)-1]='\0';
	      free(ins->opcode);
	      }
	    */
	}
	else if(no_tokens==1)
	{
	    ins->label=NULL;
	    ins->operand=NULL;
	    if(!(ins->opcode=(char *)malloc(sizeof(char)*strlen(buff1))))
	  {
	      free(ins->opcode);
	      printf("\nMalloc failed : decode()\n");
	      return(MALLOC_FAILED);
	  }
	  strcpy(ins->opcode,buff1);
	}
	else
      {
	  ins->label=NULL;
	  ins->operand=NULL;
	  ins->opcode=NULL;
      }
	
	return(0);
    }
}




void passtwo(inst * first_node,Hashtab *ht)
{
    inst *tnode;
    tnode=first_node;
    FILE *output;
    int *opcode=NULL;
    long int symval=0;
    output=fopen("output","w");
    fprintf(output,"\n%lX\t%s\t",tnode->loc,tnode->line);
    tnode=tnode->next;
    while(tnode)
    {
	fprintf(output,"\n%lX\t%s\t",tnode->loc,tnode->line);
	if(tnode->comment!=1)
	{
	    printf("\n Inside not comment");
	    if(tnode->label==NULL)
	    {
		opcode=(int *)get_data(ht,tnode->opcode);
		if(opcode!=NULL)
		{
		    tnode->machinecode=*opcode;
		}
		symval=search_symtab(tnode->operand);
		if(symval!=NOT_FOUND)
		{
		    tnode->address=symval;
		}
		//	printf("\n i am here");
		fprintf(output,"%lX%lX",tnode->machinecode,tnode->address);
	    }
//	    fprintf(output,"\n");
	}
	else
	{
//	    fprintf(output,"\n");
	}
	tnode=tnode->next;
    }
    fprintf(output,"\n");
    fclose(output);
}
