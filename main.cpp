#include "gifhead.h"
#include "vgapal.h"
#include <setjmp.h>

jmp_buf jb;
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
	if(frame_count>=64)
		longjmp(jb,1);
}


/*===============================================*/
int test()
{
// I f,i,s,x,y;for(f=0;f<64;f++,C){static D t,a,b,w[S*9]={W/2,H/2};w[2]=70+cos(f*PI/15)*20;D *d=w,*n=w+3,*e;for(s=0;s<5;s++)for(e=n;d<e;d+=3){t=d[2];for(a=0;a<2*PI;a+=PI/4,n+=3){b=a+f*PI/30;n[0]=d[0]+cos(b)*t;n[1]=d[1]+sin(b)*t;n[2]=d[2]*(0.05*sin(f*PI/15)+0.25);}B(d[1],d[0])=32+s*3;}}
//for(D f=0;f<2*M_PI;f+=0.1,C)for( D x=-1;x<1;x+=0.02)for(D z=-1;z<1;z+=0.02){ D y=sin(2*f)*sqrt(x*x+z*z),c=cos(f),s=sin(f),d=cos(-7),t=sin(-7),k=c*x-s*z,l=t*s*x+d*y+c*t*z,m=2+d*s*x-t*y+d*c*z;X(l/m,k/m)=15;}
//int i,j,S=999,W=320,H=240,k=0;float D=S,x,y,z,f,e;for(;k++<16;){for(j=1;j++<D;){e=j/D;z=log(2*e);for(i=0;i++<S;){f=2*M_PI*i/S;x=W*e*cos(f)+160;y=-25*z+H*e*sin(f)+60;x=x<0?0:x>(W-1)?W-1:x;y=y<0?0:y>(H-1)?H-1:y;B[(int)y][(int)x]=((j/6+k)&15)*16+((i/8+k)&15);}}F;}
//typedef float f;for(int c=0;c<15;++c){f b=0.1+6.3*c/15.0,s=sin(b);f q=cos(b)*s,r=sin(b)*s;s=cos(b);for(int i=0;i<W*H;++i){int p=i%256;P[p][0]=P[p][1]=P[p][2]=p;f t=0;for (int p=0;p<15;++p){f h=(2*(i%W)/f(W)-1)*t;f j=(1-2*(i/W)/f(H))*t;f k=0.8*t-0.65;f ev=h*q+j*r+k*s;f l=h-q*ev;f m=j-r*ev;f n=k-s*ev;f o=0.35/sqrt(l*l+m*m+n*n);f u=h-l*o;f v=j-m*o;f w=k-n*o;t+=sqrt(u*u+v*v+w*w)-0.1;} B[0][i]=255/(1+t);} F;}
int i,x,y;for(float f=1;f>0;f-=0.1) {;for(y=0;y<H;y++) for(x=0;x<W;x++) { std::complex<double> c(-0.1011-f+(2*f*x)/W,0.9563-f+(2*f*y)/H),z(0,0); for(i=50;i>0 && std::abs(z) < 2;--i) z=z*z+c; if (i) B[y][x]=i; }F;memset(B,0,S);}
return 0;
}
/*===============================================*/

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
int main(int argc,char **argv)
{
	vga();
	memset(buffers,0,sizeof(buffers));

	if(setjmp(jb)!=0)
		goto frame_limit;

/*===============================================*/
/*
	PG;
	I i,j,k,s[100*3];
	for(i=0;i<100;i++){
		s[i*3]=rand()%H;
		s[i*3+1]=rand()%W;
		s[i*3+2]=rand()%255;
	}
	for(k=0;k<4;k++){
		for(i=0;i<100;i++){
			B(s[i*3],s[i*3+1])=s[i*3+2];
			I d=s[i*3+2]/10;
			if(d==0)
				d=2;
			s[i*3+1]=(s[i*3+1]+d)%W;
		}
		C;
	}
*/
test();
/*===============================================*/

frame_limit:
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

#ifdef USEWX
wxEntry(0,0,0,SW_SHOWNORMAL);
#else
#ifdef _WIN32
	DialogBox(0,(LPSTR)IDD_DIALOG1,0,dlg);
#endif
#endif // USEWX
	return 0;
}
