#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "simplestring.c"
#include "file.c"
#include "splitword.c"
#include "structure.c"

#ifndef _construct__index__
#define _construct__index__

int CompressIndex();
unsigned int GetFileNumber(const char* filename);
void ShowFile();
void addfiletoindex(char* filename);
int SortIndex();

int CompressIndex()
{
	int filelength=GetFileLength(fraw);
	int tmpint=filelength/sizeof(indexnode);
	int i,currentp=0,m1;
	FILE* filenameindex;
	FILE* compressf;
	FILE* keyf;
	keynode tmpkeynode;
	comparenode *tmp;
	filenameindex=fopen(fraw,"rb");
	fseek(filenameindex,0,SEEK_SET);
	tmp=(comparenode*)malloc(filelength);
	fread(tmp,1,filelength,filenameindex);
	fclose(filenameindex);
	compressf=fopen(fcompressindex,"wb");
	rewind(compressf);
	keyf=fopen(fkeyindex,"wb");
	tmpkeynode.key=0;
	tmpkeynode.start=0;
	tmpkeynode.length=0;
	for(i=0;i<tmpint;i++)
	{
		if(tmpkeynode.key!=tmp[i].n1)
		{
			m1=ftell(compressf);
			tmpkeynode.length=m1-currentp;
			fwrite(&tmpkeynode,sizeof(tmpkeynode),1,keyf);
			tmpkeynode.key=tmp[i].n1;
			tmpkeynode.start=m1;
			currentp=m1;
		}
		fwrite(&(tmp[i].n2),sizeof(int),1,compressf);
	}
	m1=ftell(compressf);
	tmpkeynode.length=m1-currentp;
	fwrite(&tmpkeynode,sizeof(tmpkeynode),1,keyf);
	free(tmp);
	fclose(compressf);
	fclose(keyf);
	printf("Compress index OK...\n");
	return tmpint;
}

unsigned int GetFileNumber(const char* filename)
{
	unsigned int num;
	FILE* sss;
	FileNode aaaa;
	CheckFileExists(fcontainer);
	num=GetFileLength(fcontainer)/sizeof(FileNode);
	sss=fopen(fcontainer,"rb+");
	fseek(sss,0,SEEK_END);
	strcpy(aaaa,filename);
	fwrite(aaaa,sizeof(FileNode),1,sss);
	fclose(sss);
	return num;
}

void ShowFile()
{
	FileNode aaaa;
	FILE* sss;
	int i;
	sss=fopen(fcontainer,"rb+");
	fseek(sss,0,SEEK_SET);
	for (i=0;i<GetFileLength(fcontainer)/sizeof(aaaa) ;i++ )
	{
		fread(aaaa,1,sizeof(FileNode),sss);
		printf("%d -> %s\n",i,aaaa);
	}
	fclose(sss);
}

void addfiletoindex(char* filename)
{
	FILE* rawfile;
	WordList list;
	simplestring* aaa=(simplestring*)malloc(sizeof(simplestring));
	int filelength;
	unsigned int filecount=GetFileNumber(filename);
	int i;
	indexnode tmpnode;
    LoadTextFile(filename,aaa);
	filelength=aaa->length;
	CheckFileExists(fraw);
	rawfile=fopen(fraw,"rb+");
	fseek(rawfile,0, SEEK_END);

	SplitString(aaa,&list,1);

	for (i=0;i<list.size ;i++ )
	{
		//tmpnode.offset=i;
		tmpnode.file=makefilenode(filecount,i);
		//printf("%s\n",list.item[i]);
		tmpnode.key=HashString(list.item[i],0);
		fwrite(&tmpnode,sizeof(indexnode),1,rawfile);
	}
	printf("Processing %s , length: %d bytes.   %f\n",filename,filelength,((float)list.size)/aaa->length);
	FreeWordList(&list);
	freestring(aaa);
    fclose(rawfile);
}

int SortIndex()
{
	int i;
	int tmpkey;
	int filelength=GetFileLength(fraw);
	int tmpint=filelength/sizeof(comparenode);
	FILE* filenameindex=fopen(fraw,"rb+");
	comparenode *tmp=(comparenode*)malloc(filelength);
	
	printf("Sort index start...\n");
	fseek(filenameindex,0,SEEK_SET);
	rewind(filenameindex);
	fread(tmp,sizeof(comparenode),tmpint,filenameindex);
	Quick_Sort(tmp,tmpint,sizeof(comparenode),compareindex);
	fseek(filenameindex,0,SEEK_SET);
	rewind(filenameindex);
	fwrite(tmp,sizeof(comparenode),tmpint,filenameindex);
	fclose(filenameindex);
	printf("Sort index OK...\n");
	//tmpkey=tmp[0].n1;
	/*for(i=1;i<tmpint;i++)
	{
		if(tmpkey>tmp[i].n1)
		{
			printf("error at item %X:  %o---%o\n",i*sizeof(comparenode),((unsigned int)tmpkey),((unsigned int)tmp[i].n1));
		}
		tmpkey=tmp[i].n1;
		//printf("%d\n",tmp[i].n1);
	}*/
	free(tmp);
	return tmpint;
}

int main()
{
	indexnode a;
	FILE* sss;
	int len;
	char fileitem[256];
	InitSplit();
	printf("Add files to index...\n");

	while(scanf("%s",fileitem)==1)
	{
		addfiletoindex(fileitem);
	}

	//ShowFile();
	SortIndex();
	CompressIndex();	

	return 0; 
}

#endif