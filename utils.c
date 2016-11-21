#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "vgapal.h"


int getcolor(unsigned char *buf)
{
	int c;
	c=(buf[0]<<16)|(buf[1]<<8)|buf[2];
	return c;
}
int addcolor(int color,int *colors)
{
	int i,result=0;
	for(i=0;i<100;i++){
		if(colors[i]==-1){
			colors[i]=color;
			result=i;
			break;
		}else if(colors[i]==color){
			result=i;
			break;
		}
	}
	return i;
}
int logdata(unsigned char *str,int count)
{
	FILE *f;
	f=fopen("b:\\out.txt","a+");
	if(f){
		int i;
		for(i=0;i<count;i++){
			fprintf(f,"%i",str[i]);
		}
		fprintf(f,"\n");
		fclose(f);
	}
}
int emit_stuff(int color,int len,unsigned char *str,int *bitcount)
{
	int i;
	int index=*bitcount;
	int COLOR_BITS=3;
	int STREAK_BITS=2;
	for(i=0;i<COLOR_BITS;i++){
		str[index++]=(color>>(COLOR_BITS-1-i))&1;
	}
	for(i=0;i<STREAK_BITS;i++){
		str[index++]=(len>>(STREAK_BITS-1-i))&1;
	}
	logdata(str+*bitcount,COLOR_BITS+STREAK_BITS);
	*bitcount=index;
}
int folder_exists(char *path)
{
	int attr;
	int result=FALSE;
	attr=GetFileAttributes(path);
	if(attr!=0xFFFFFFFF){
		if(attr&FILE_ATTRIBUTE_DIRECTORY)
			result=TRUE;
	}
	return result;
}
int color_distance(int a,int b)
{
	int r1,r2,g1,g2,b1,b2;
	double f;
	r1=(a>>16)&0xFF;
	r2=(b>>16)&0xFF;
	g1=(a>>8)&0xFF;
	g2=(b>>8)&0xFF;
	b1=a&0xFF;
	b2=b&0xFF;
	f=pow(r1-r2,2)+pow(g1-g2,2)+pow(b1-b2,2);
	f=sqrt(f);
	return f;
}
int export_image()
{

	FILE *f,*fout;
	char *path="b:\\";
	if(folder_exists(path))
		SetCurrentDirectory(path);
	else
		SetCurrentDirectory("c:\\temp\\");

	DeleteFile("out.txt");

	f=fopen("5.raw","rb");
	if(f){
		int colors[100];
		int current_color=-1;
		int bitcount=0;
		int streak=0;
		int x=0;
		int IMAGE_WIDTH=65536;
		int MAX_STREAK=3;
		unsigned char str[2000]={0};
		unsigned char data[0x1000]={0};
		int len,i;
		memset(colors,-1,sizeof(colors));
		len=fread(data,1,0x1000,f);
		for(i=0;i<len;i+=3){
			int c;
			c=getcolor(data+i);
			if(c!=current_color){
				int ci;
				ci=addcolor(c,colors);
				if(streak>0){
					ci=addcolor(current_color,colors);
					emit_stuff(ci,streak,str,&bitcount);
					streak=0;
				}
				current_color=c;
			}
			streak++;
			x++;
			if(x>=IMAGE_WIDTH){
				if(streak>0){
					int ci;
					ci=addcolor(current_color,colors);
					emit_stuff(ci,streak,str,&bitcount);
					streak=0;
				}
				emit_stuff(0,0,str,&bitcount);
				x=0;
			}
			if(streak>=MAX_STREAK){
				int ci;
				ci=addcolor(current_color,colors);
				emit_stuff(ci,streak,str,&bitcount);
				streak=0;
			}
			if(i>=(len-3)){
				int ci;
				ci=addcolor(current_color,colors);
				emit_stuff(ci,streak,str,&bitcount);
			}
		}
		fclose(f);
		{
			int i;
			int BITLEN=5;
			for(i=0;i<bitcount;i+=BITLEN){
				int j,a=0;
				for(j=0;j<BITLEN;j++){
					a<<=1;
					a|=str[i+j];
				}
				a|=0x40;
				if(a=='\\')
					printf("%c",a);
				printf("%c",a);
			}
		}
		{
			int i;
			printf("\n");
			for(i=0;i<sizeof(colors)/sizeof(int);i++){
				int c;
				c=colors[i];
				if(c!=-1){
					int j,closest=-1;
					unsigned int d,max;
					max=-1;
					for(j=0;j<sizeof(vgapal);j+=3){
						int tmp=getcolor(vgapal+j);
						d=color_distance(c,tmp);
						if(d<max){
							max=d;
							closest=j/3;
						}
					}
					printf("color %i=%08X closest=%i\n",i,c,closest);
				}
			}

		}
	}
	else
		printf("unable to open file\n");
	printf("\ndone\n");
	getch();
	exit(0);
}
