#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h> 
#include "hash_table.c"
#include "file.c"
#include "simplestring.c"
#include "splitword.c"
#include "structure.c"

#ifndef _construct__index__
#define _construct__index__

void InitSearch()
{
	keynodelength=GetFileLength(fkeyindex)/sizeof(keynode);
	InitSplit();
}

int getmemblock(comparenode node,memblock* mem)
{
	FILE* indexfile;
	int i;
	if(node.n2<=0)
	{
		mem->length=0;
		mem->block=NULL;
		return 0;
	}
	indexfile=fopen(fcompressindex,"rb");
	fseek(indexfile,node.n1,SEEK_SET);
	mem->block=(unsigned int*)malloc(node.n2);
	mem->length=node.n2;
	fread(mem->block,1,node.n2,indexfile);
	fclose(indexfile);

	return 1;
}

void makenext(memblock* m1)
{
	unsigned int* tmp=m1->block;
	int length=m1->length/sizeof(int);
	int i;
	for(i=0;i<length;i++)
	{
		(*tmp)++;
		tmp++;
	}
}

int compareblock(memblock* m1,memblock* m2,memblock* m3)
{
	unsigned int* a=m1->block;
	unsigned int* b=m2->block;
	int i=(m1->length)/sizeof(int),j=(m2->length)/sizeof(int);
	int count=0;
	unsigned int* rrr;
	unsigned int* tmp;
	m3->block=(unsigned int*)malloc(m1->length < m2->length ? m1->length : m2->length);
	rrr=m3->block;
	tmp=m3->block;
	while(i>0 && j>0)
	{
		if(*a==*b)
		{
			count++;
			i--;j--;
			*tmp=*a;
			a++;b++;
			tmp++;
		}
		else if (*a < *b)
		{
			i--;
			a++;
		}
		else if(*a > *b)
		{
			j--;
			b++;
		}
	}
	m3->length=count*sizeof(int);
	m3->block=rrr;

	return count>0;
}

int showresult(memblock* m1)
{
	int i,length=m1->length/sizeof(unsigned int);
	FileNode tmpf;
	FILE* filenameindex=fopen(fcontainer,"rb");
	unsigned int* tmp;
	unsigned int filenumber;
	tmp=m1->block;

	for(i=0;i<length;i++)
	{
		filenumber=getfilenumber(tmp[i]);
		fseek(filenameindex,filenumber*sizeof(FileNode),SEEK_SET);
		fread(tmpf,sizeof(FileNode),1,filenameindex);
		printf("%x\t->\t%s\n",filenumber,tmpf);
	}
	fclose(filenameindex);
	return 0;
}

int ShowCompleteResult(SearchResult* abc)
{
	int i,length=abc->resultcount;
	FileNode tmpf;
	FILE* filenameindex=fopen(fcontainer,"rb");
	unsigned int* tmp;
	unsigned int filenumber;
	tmp=abc->result_index;
	printf("\n--------\nFind  %d results. Time elapse:%g s\n--------\n",length,abc->elapsetime);
	//memset(&tmpf,sizeof(FileNode),0);
	for(i=0;i<length;i++)
	{
		filenumber=getfilenumber(tmp[i]);
		fseek(filenameindex,filenumber*sizeof(FileNode),SEEK_SET);
		fread(tmpf,sizeof(FileNode),1,filenameindex);
		printf("%x\t->\t%s\n",filenumber,tmpf);
	}
	fclose(filenameindex);
	return 0;
}

struct comparenode findkey(unsigned int key)
{
	int l,m,r,tmp;
	keynode tmpk;
	comparenode returnvalue;
	FILE* keyindexf=fopen(fkeyindex,"rb");
	returnvalue.n1=0;
	returnvalue.n2=0;
	l=1;r=keynodelength-1;
	m=(l+r)/2;
	while(l<=m && m<=r)
	{
		fseek(keyindexf,m*sizeof(keynode),SEEK_SET);
		fread((char*)(&tmpk),sizeof(keynode),1,keyindexf);
		if(tmpk.key==key)
		{
			returnvalue.n1=tmpk.start;
			returnvalue.n2=tmpk.length;
			break;
		}
		else if(tmpk.key>key)
		{
			r=m-1;m=(l+r)/2;
		}
		else if(tmpk.key<key)
		{
			l=m+1;m=(l+r)/2;
		}
	}
	fclose(keyindexf);
	return returnvalue;
}

int dividestring(unsigned int** keylist,const char* str)
{
	WordList list;
	simplestring* aaa=(simplestring*)malloc(sizeof(simplestring));
	int filelength;
	int i;
	comparenode ggg;
	indexnode tmpnode;
	aaa->string=str;
	aaa->length=strlen(str);
	SplitString(aaa,&list,1);
	*keylist=(unsigned int*)malloc((sizeof(int))*(aaa->length));
	for (i=0;i<list.size ;i++ )
	{
		(*keylist)[i]=HashString(list.item[i],0);
	}
	FreeWordList(&list);
	free(aaa);
	return i;
}

void Search(SearchResult* abc,const char* keyword)
{
	char abcd[256];
	unsigned int* keylist;
	int i,lsize;
	keynode cccc;
	comparenode aaa;
	memblock m1,m2,m3;
	int resultcount;
	clock_t start, finish;
	double  duration;
	start = clock();

	strcpy(abcd,keyword);
	lsize=dividestring(&keylist,abcd);
	if (lsize<=0)
	{
		finish = clock();
		duration = (double)(finish-start)/CLOCKS_PER_SEC;

		abc->elapsetime=duration;
		abc->resultcount=0;
		abc->result_index=NULL;
		abc->result=NULL;
		free(keylist);
		return;
	}
	m1.length=0;

	aaa=findkey(keylist[0]);
	if (aaa.n2==0)
	{
		finish = clock();
		duration = (double)(finish-start)/CLOCKS_PER_SEC;

		abc->elapsetime=duration;
		abc->resultcount=0;
		abc->result_index=NULL;
		abc->result=NULL;
		free(keylist);
		return;
	}

	m3.block=NULL;
	m3.length=0;

	m1.block=NULL;
	m1.length=0;

	if(getmemblock(aaa,&m1))
	{
		for(i=1;i<lsize;i++)
		{
			aaa=findkey(keylist[i]);
			if (getmemblock(aaa,&m2))
			{
				makenext(&m1);
				if (!compareblock(&m1,&m2,&m3))
				{
					FreeMemBlock(&m1);
					FreeMemBlock(&m2);
					FreeMemBlock(&m3);
					break;
				}
				FreeMemBlock(&m1);
				FreeMemBlock(&m2);
				m1.length=m3.length;
				m1.block=m3.block;
			}
		}
	}

	finish = clock();
	duration = (double)(finish-start)/CLOCKS_PER_SEC;
	
	free(keylist);

	abc->elapsetime=duration;
	abc->result_index=m1.block;
	abc->result=NULL;
	abc->resultcount=m1.length/sizeof(int);

}

int main()
{
	char abcd[256];
	SearchResult ddd;
	InitSearch();
	scanf("%s",abcd);
	Search(&ddd,abcd);
	ShowCompleteResult(&ddd);
}

#endif
