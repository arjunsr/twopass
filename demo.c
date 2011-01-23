#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main()
{
  char* string="OF";
  char* dest;
  //  snprintf(dest,80,"%x","EOF");
  printascii(dest,string);
  puts(dest);
  return 0;
}

int printascii(char* dest,char* source)
{
  int length=strlen(source);
  if( !(dest=(char*)malloc(length*2*sizeof(char))))
    {
      printf("\nMalloc failed\n");
    }
    else
      {
	int i=0;
	for(i=0;i<length;i++)
	  snprintf(dest,2,"%X",(int)source);
	source++;
	dest=dest+2;
      }
    return 0;

}

