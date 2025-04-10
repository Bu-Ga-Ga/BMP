#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SWAP(a,b) {int t; t=(a); (a)=(b); (b)=t;} 
typedef unsigned char uchar;//Из unsigned char делаем uchar 
#pragma pack(1)
void *Allocate2DArray(int m,int n, int sz);
typedef struct BMPHEAD_
	{
		unsigned short int Signature; 
		unsigned int FileLenght;
		unsigned int Zero;
		unsigned int Ptr;
		unsigned int Version;
		unsigned int Width;         
		unsigned int Height;
		unsigned short int Planes;
		unsigned short int BitsPerPixel;  
		unsigned int Compression;
		unsigned int SizeImage;
		unsigned int XPelsPerMeter;
		unsigned int YPelsPerMeter;
		unsigned int ClrUsed ;
		unsigned int ClrImportant ;
		uchar pal[255][4];
		uchar (**v)[4];
	}BMPHEAD;
	
#pragma pack()

void *Allocate2DArray(int m,int n, int sz)//Создает двумерный массив m*n, размер одного элемента
{
	char **v;
	int i;
	v = (char**)malloc(sizeof(void*)*m+m*n*sz); 
	v[0]=(char*)(v+m);
	for(i=1;i<m;i++) v[i]=v[i-1]+n*sz;
	return v;
}

int main(void) 
{   
char sf[]="1.bmp",sf2[]="res.bmp";
	BMPHEAD h;   //h-начало заголовка   
	FILE *f;     
	int i,j,delta,lbuf;
	uchar *buf=NULL; 
	f=fopen(sf,"rb"); 
	fread(&h,54,1,f);
	printf("Width=%d Heigh=%d BitsPerPixel=%d\n",(int)h.Width,(int)h.Height,(int)h.BitsPerPixel);
	
	if(h.BitsPerPixel==8)
	{
		fread (h.pal, h.ClrImportant,4,f);
		delta=(int)h.Ptr-(54+h.ClrImportant*4);
		printf("8bmp: delta=%d\n",delta);
		lbuf=(int)(h.SizeImage/h.Height);//Длинна строки
		buf=(uchar*)malloc(lbuf);//Буфер 
		for(i=0;i<delta;i++) fread(buf,1,1,f);
		h.v=(uchar (**)[4]) Allocate2DArray(h.Height,h.Width,4);
		
		for(i=0;i<(int)h.Height;i++)
		{
			fread(buf,lbuf,1,f);
			for(j=0;j<(int)h.Width;j++)
			{
				h.v[i][j][0]=h.pal[buf[j]][0];
				h.v[i][j][1]=h.pal[buf[j]][1];
				h.v[i][j][2]=h.pal[buf[j]][2];
				
			}
		}
	}
	
	
	else if (h.BitsPerPixel==24)
	{
		delta=(int)h.Ptr-54;
		printf("24bmp: delta=%d\n",delta);
		lbuf=(int)(h.SizeImage/h.Height);
		buf=(uchar*)malloc(lbuf);
		for(i=0;i<delta;i++) fread(buf,1,1,f);
		h.v=(uchar (**)[4])Allocate2DArray(h.Height,h.Width,4);
		
		for(i=0;i<(int)h.Height;i++)//Считывем строки(пиксели)
		{
			fread(buf,lbuf,1,f);
			for(j=0;j<(int)h.Width;j++)//Считывем столбцы(пиксели)
			{
				h.v[i][j][0]=buf[j*3+0];
				h.v[i][j][1]=buf[j*3+1];
				h.v[i][j][2]=buf[j*3+2];
				
			}
		}
	}
	free(buf);
	buf=NULL;	
	fclose(f);
//-----------------------------	
	f=fopen(sf2,"wb");
	delta=84;
	SWAP(h.Width,h.Height);
	h.BitsPerPixel=24;
	lbuf=(h.Width*3+3)/4*4;
	buf=(uchar*)malloc(lbuf);
	h.SizeImage=h.Height*lbuf;
	h.FileLenght=54+h.SizeImage;
	//h.Ptr=54; 
	fwrite(&h,54,1,f);
	for(i=0;i<delta;i++) fwrite(buf,1,1,f);
	
	for(i=0;i<(int)h.Height;i++)
		{
			for(j=0;j<(int)h.Width;j++)
			{
				buf[j*3+0]=h.v[j][i][0];
				buf[j*3+1]=h.v[j][i][1];
				buf[j*3+2]=h.v[j][i][2];
				fwrite(buf,lbuf,1,f);
			}
		}
	free(h.v);
	h.v=NULL;
	return 0;
}
