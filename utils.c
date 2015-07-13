#include <windows.h>
#include <stdio.h>

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
		if(colors[i]==0){
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
int emit_color(int color,unsigned char *str,int *bitcount)
{
	int i;
	int index=*bitcount;
	int LEN=2;
	str[index++]=1;
	for(i=0;i<LEN;i++)
		str[index++]=(color>>(LEN-1-i))&1;
	logdata(str+*bitcount,LEN+1);
	*bitcount=index;
}
int emit_len(int len,unsigned char *str,int *bitcount)
{
	int i;
	int index=*bitcount;
	int LEN=3;
	str[index++]=0;
	for(i=0;i<LEN;i++){
		str[index++]=(len>>(LEN-1-i))&1;
	}
	logdata(str+*bitcount,LEN+1);
	*bitcount=index;
}
int emit_stuff(int color,int len,unsigned char *str,int *bitcount)
{
	int i;
	int index=*bitcount;
	int LEN=2;
	for(i=0;i<LEN;i++){
		str[index++]=(color>>(LEN-1-i))&1;
	}
	LEN=3;
	for(i=0;i<LEN;i++){
		str[index++]=(len>>(LEN-1-i))&1;
	}
	logdata(str+*bitcount,5);
	*bitcount=index;
}

int export_image()
{

	FILE *f,*fout;


	DeleteFile("B:\\out.txt");

	f=fopen("B:\\zelda2.raw","rb");
	if(f){
		int colors[100]={0};
		int current_color=0;
		int bitcount=0;
		int streak=0;
		int x=0;
		int IMAGE_WIDTH=13;
		int MAX_STREAK=6;
		unsigned char str[2000]={0};
		unsigned char data[0x1000]={0};
		int len,i;
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
				emit_stuff(0,7,str,&bitcount);
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
	}
	printf("\ndone\n");
	getch();
	exit(0);
}
