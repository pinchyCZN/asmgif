#include "gifhead.h"
#include "vgapal.h"

int frame_count=0;
unsigned char buffers[64*W*H];
Palette P;
Buffer B;

#ifdef _WIN32
#include "gui.h"
#endif

extern "C"{
	void	*newgif ( void **gifimage, int width, int height,
	int *colors, int bgindex );
	int	animategif ( void *gs, int nrepetitions,
		int delay, int tcolor, int disposal );
	int	putgif ( void *gs, void *pixels );
	int	endgif ( void *gs );
};

void SaveFrame(bool clear)
{
	unsigned char *b=B.Get();
	if(frame_count<64){
		memcpy(buffers+(frame_count*W*H),b,W*H);
		if(clear)
			memset(b,0,W*H);
		frame_count++;
	}
}



int test()
{
 I f,i,s,x,y;for(f=0;f<60;f++,C){static D t,a,b,w[S*9]={W/2,H/2};w[2]=70+cos(f*PI/15)*20;D *d=w,*n=w+3,*e;for(s=0;s<5;s++)for(e=n;d<e;d+=3){t=d[2];for(a=0;a<2*PI;a+=PI/4,n+=3){b=a+f*PI/30;n[0]=d[0]+cos(b)*t;n[1]=d[1]+sin(b)*t;n[2]=d[2]*(0.05*sin(f*PI/15)+0.25);}B(d[1],d[0])=32+s*3;}}
return 0;
}

int vga()
{
	int i;
	for (i=0;i<256;i++){
		unsigned char *p=P.Get();
		int o=i*3;
		p[o]=vgapal[o];
		p[o+1]=vgapal[o+1];
		p[o+2]=vgapal[o+2];
	}
	return 0;
}
int main()
{
	vga();


	PG;
	I i,j,k,s[100*3];
	for(i=0;i<100;i++){
		s[i*3]=rand()%H;
		s[i*3+1]=rand()%W;
		s[i*3+2]=rand()%255;
	}
	for(k=0;k<64;k++){
		for(i=0;i<100;i++){
			B(s[i*3],s[i*3+1])=s[i*3+2];
			I d=s[i*3+2]/10;
			if(d==0)
				d=2;
			s[i*3+1]=(s[i*3+1]+d)%W;
		}
		C;
	}


	if(0)
	if(frame_count>0)
	{
		static void *gifimage=0;
		static void *gsdata=0;
		unsigned char *tmp=P.Get();
		int ctable[256*3];
		int i;
		for(i=0;i<256;i++){
			ctable[i*3]=tmp[i*3];
			ctable[i*3+1]=tmp[i*3+1];
			ctable[i*3+2]=tmp[i*3+2];
		}
		gifimage=0;
		gsdata=newgif(&gifimage,W,H,ctable,0);
		if(gsdata!=0){
			int i;
			int glen;
			FILE *f;
			char *fname="c:\\temp\\temp.gif";
			animategif(gsdata,0,5,0,2);
			for(i=0;i<frame_count;i++){
				putgif(gsdata,buffers+(i*H*W));
			}
			glen=endgif(gsdata);
			f=fopen(fname,"wb");
			if(f){
				fwrite(gifimage,1,glen,f);
				fclose(f);
				printf("wrote %i frames to %s\n",frame_count,fname);
			}
		}
	}


#ifdef _WIN32
	DialogBox(0,(LPSTR)IDD_DIALOG1,0,dlg);
#endif
	return 0;
}