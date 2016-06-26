#include "gifhead.h"
#include "vgapal.h"
#include <setjmp.h>
#include <stdio.h>


extern "C" int export_image();

jmp_buf jb;
int frame_count=0;
unsigned char buffers[64*W*H];
Palette P;
Buffer B;

unsigned char anim_palette[768*64];

#ifdef _WIN32
#include "gui.h"
#endif

extern "C"{
	void	*newgif ( void **gifimage, int width, int height,
	int *colors, int bgindex );
	int	animategif ( void *gs, int nrepetitions,
		int delay, int tcolor, int disposal );
	int	putgif ( void *gs, void *pixels );
	int	fputgif ( void *gs, int left, int top, int width, int height,	void *pixels, int *colors );
	int	endgif ( void *gs );
};

void SaveFrame(bool clear)
{
	if(frame_count<64){
		unsigned char *b=B.Get();
		unsigned char *p=P.Get();
		memcpy(buffers+(frame_count*W*H),b,W*H);
		memcpy(anim_palette+(frame_count*768),p,768);
		if(clear)
			memset(b,0,W*H);
		frame_count++;
	}
	if(frame_count>=64)
		longjmp(jb,1);
}
/*
//rotate by z
cx=1
cy=1
cz=cos(rz/(57.2957795));

sx=0
sy=0
sz=sin(rz/(57.2957795));

tx=x*cz+y*sz;
ty=x*(-sz)+y*(cz);
tz=z;

//rotate by x
cx=cos(rx/(57.2957795));
cy=1
cz=1

sx=sin(rx/(57.2957795));
sy=0
sz=0

tx=x;
ty=y*cx+z*sx;
tz=y*(-sx)+z*cx;

//rotate by y
cx=1
cy=cos(ry/(57.2957795));
cz=1

sx=0
sy=sin(ry/(57.2957795));
sz=0

tx=x*cy-z*sy;
ty=y;
tz=x*sy+z*cy;

//rotate by xy
cx=cos(rx/(57.2957795));
cy=cos(ry/(57.2957795));
cz=1

sx=sin(rx/(57.2957795));
sy=sin(ry/(57.2957795));
sz=0

tx=x*cy-z*sy;
ty=x*(sx*sy)+y*cx+z*sx*cy;
tz=x*(cx*sy)+y*(-sx)+x*cx*cy;


//rotate by xz
cy=1;
sy=0;
tx=x*cz+y*sz;
ty=x*(-cx*sz)+y*(cx*cz)+z*sx;
tz=x*(sx*sz)+y*(-sx*cz)+z*cx;

int rotate_3d(float x,float y,float z,float rx,float ry,float rz)
{
	float cx,cy,cz,sx,sy,sz;
	float tx,ty,tz;
#define RID 57.2957795
	cx=cos(rx/(RID));
	cy=cos(ry/(RID));
	cz=cos(rz/(RID));
	sx=sin(rx/(RID));
	sy=sin(ry/(RID));
	sz=sin(rz/(RID));
	tx=x*cy*cz+y*cy*sz-z*sy;
	ty=x*(sx*sy*cz-cx*sz)+y*(sx*sy*sz+cx*cz)+z*sx*cy;
	tz=x*(cx*sy*cz+sx*sz)+y*(cx*sy*sz-sx*cz)+z*cx*cy;
	x=tx;
	y=ty;
	z=tz;
	return TRUE;
}
		x1=x*scale/(z1);
		y1=y*scale/(z1);
		x1+=BUF_WIDTH/2;
		y1+=BUF_HEIGHT/2;

a1 a2 a3
b1 b2 b3
c1 c2 c3

det A=a1*b2*c2+a2*b3*c1+a3*b1*c2-a3*b1*c1-a2*b1*c3-a1*b3*c2
inv A=(b2*c3-c2*b3)(a3*c2-c3*a2)(a2*b3-b2*a3)
	  (b3*c1-c3*b1)(a1*c3-c1*a3)(a3*b1-b3*a1)
	  (b1*c2-c1*b3)(a2*c1-c2*a1)(a1*b2-b1*a2)
*/

/*===============================================*/
int printbits(int v,int count)
{
	FILE *f;
	int i;
	static int done=0;
	return 0;
	if(done==0){
		done=1;
		f=fopen("b:\\out2.txt","wb");
		if(f)
			fclose(f);

	}
	f=fopen("b:\\out2.txt","a+");
	if(f==0)
		return 0;
	for(i=0;i<count;i++){
		int c=v>>(count-1-i);
		c&=1;
		fprintf(f,"%i",c);
	}
	fprintf(f,"\n");
	fclose(f);
	return 0;
}
/*
#define noiseWidth W
#define noiseHeight H

double noise[noiseWidth][noiseHeight]; //the noise array

double smoothNoise(double x, double y)
{  
   //get fractional part of x and y
   double fractX = x - int(x);
   double fractY = y - int(y);
   
   //wrap around
   int x1 = (int(x) + noiseWidth) % noiseWidth;
   int y1 = (int(y) + noiseHeight) % noiseHeight;
   
   //neighbor values
   int x2 = (x1 + noiseWidth - 1) % noiseWidth;
   int y2 = (y1 + noiseHeight - 1) % noiseHeight;

   //smooth the noise with bilinear interpolation
   double value = 0.0;
   value += fractX       * fractY       * noise[x1][y1];
   value += fractX       * (1 - fractY) * noise[x1][y2];
   value += (1 - fractX) * fractY       * noise[x2][y1];
   value += (1 - fractX) * (1 - fractY) * noise[x2][y2];

   return value;
}
void generateNoise()
{
    for (int x = 0; x < noiseWidth; x++)
    for (int y = 0; y < noiseHeight; y++)
    {
        //noise[x][y] = (rand() % 32768) / 32768.0;
		noise[x][y] = rand() % 32768;
    }
}
double turbulence(double x, double y, double size)
{
    double value = 0.0, initialSize = size;
    
    while(size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }
    
    return(128.0 * value / initialSize);
}
*/
int test()
{
/*
int i,j,S=500,W=320,H=240,k=0;
float D=S,x,y,z,f,e;
for(; k++<16;) {
    for(j=1; j++<D;) {
        e=j/D;
        z=log(2*e);
        for(i=0; i++<S;) {
            f=2*M_PI*i/S;
            x=W*e*cos(f)+160;
            y=-25*z+H*e*sin(f)+60;
            x=x<0?0:x>(W-1)?W-1:x;
            y=y<0?0:y>(H-1)?H-1:y;
            B[(int)y][(int)x]=((j/2+k)&15)*16+((i/8+k)&0xfff);
        }
    }
    F;
}

D a,f,i,x,y,t=0,r=0,d=0;
a=i=0;
PG;
t=56*4*PI/360;
for(f=0; f<64; f++){
	for(x=0;x<H;x++){
		for(y=0;y<W;y++){
			D cy,sy,tx,ty,tz,z=10*sin(a)*sin(x/8+r)*cos(y/8);
			cy=cos(t);
			sy=sin(t);
			tx=(x+d)*cy-z*sy;
			ty=y;
			tz=(x+d)*sy+z*cy;
			tz+=50;
			if(tz!=0){
				I i,j;
				//printf("%f\n",tz);
				i=tx*400/tz+H;
				j=ty*400/tz+0*W/8;
				tz=-tz+200;
				if(tz>255)
					tz=255;
				else if(tz<0)
					tz=0;
				B(i,j)=tz;
			}
		}
	}
	r+=12*PI/360;
	//d-=5;
	a+=PI/360*25;
	t+=2*PI/360;
	//t=0;
	C;
}
*/
/*
 Octants:
  \2|1/
  3\|/0
 ---+---
  4/|\7
  /5|6\
text print c=char,x,y,s=scale
U8 *f=(U8*)DOSCHAR;
for(a=0; a<8*s; a++)for(b=0; b<8*s; b++)if(f[c*8+(I)(b/s)]&(0x80>>(I)(a/s)))B(y+b,x+a)=15;

*/

#if 1
//D t,a,b,x,y;int i;const int SZ=20;D cx[SZ],cy[SZ],r[SZ];for(a=0;a<256;a++){P=255*sin(PI/2*256.0/(a+.01));P=255*sin(PI/2*256.0/((a/2)+.01));P=0;}for(i=0;i<SZ;i++){cx[i]=rand()%W;cy[i]=rand()%H;r[i]=((D)(rand()%6283))/1000.0;}for(t=0;t<64;t++){for(i=0;i<SZ;i++){for(x=0;x<W;x++){for(y=0;y<H;y++){a=sin(r[i])*40;b=pow(x-cx[i],2)+pow(y-cy[i],2);if(a>0 && b<=(a*a)){a=cos((a-b)/(a*a))*0xFF;B(y,x)=a;}}}}for(i=0;i<SZ;i++){r[i]+=PI/20;} C;}
//D t,a,b,x,y;int i,j;for(i=0;i<256;i++){P=i;P=0;P=0;}for(t=0;t<64;t++){for(x=0;x<W;x++){if(rand()&1)i=rand()%300;if(i>255)i=255;B(H-1,x)=i;}for(x=0;x<W;x++){for(y=0;y<H;y++){i=B(y-1,x);i+=B(y,x-1);i+=B(y,x+1);i+=B(y+1,x);i/=4;for(j=1;j<=5;j++)B(y-j,x)=i;}}F;}
//const int T=40; D t,a,b,x,y,z,c[T];int i,j;z=2;for(i=0;i<T;i++)c[i]=rand()%H;for(t=0;t<32;t++){for(j=0;j<T;j++){for(a=0;a<30;a++){for(x=0;x<z;x++){y=50*sin(c[j]/100+x/10)+a+c[j]+H/4;B(y,x)=c[j];y=50*sin(c[j]/100+(W-x)/10)+a+c[j]+H/4;B(y,W-x)=c[j];}}}z+=20;if(z>W/2){for(x=0;x<z-W/2;x++)for(y=0;y<H;y++){a=W/2-(z-W/2)/2+x;B(y,a)=55;}}F;}
//const int T=100;int i,t,a,x,y,c[T],dx[T],dy[T];D vx[T],vy[T];for(i=0;i<T;i++){c[i]=rand()%(H/2);dx[i]=rand()%H;dy[i]=rand()%H;vx[i]=(rand()%8)-4;vy[i]=(rand()%8)-4;}for(t=0;t<64;t++){for(i=0;i<T;i++){for(x=0;x<c[i];x++){for(y=0;y<c[i];y++){a=B(y+dy[i],x+dx[i])+c[i];B(y+dy[i],x+dx[i])=a%255;}}}for(i=0;i<T;i++){dx[i]+=vx[i];dy[i]+=vy[i];}C;}
//int i,j,t;D a,b,c,d,x,y,f[40];for(t=0;t<64;t++){for(j=0;j<H;j++){for(i=0;i<40;i++)f[i]=((D)(rand()%(1000+t*2))+1)/10.0;for(x=0;x<W;x++){d=0;for(i=0;i<40;i++){if(i&1)d+=sin(f[i]+x/f[i]);else d+=cos(f[i]+x/f[i]);}y=d*2+j*4;B(y,x)=t+j;}}C;}
//int i,t;D a,b,x,y,s;PG;for(t=0;t<64;t++){for(i=0;i<20;i++){s=20+i*20;x=20*sin(t/8.+i/3.)-s/2;y=20*cos(t/8.+i/3.)-s/2;for(a=0;a<s;a++){for(b=0;b<s;b++){int f=a<s*.1;f|=a>s*.9;f|=b<s*.1;f|=b>s*.9;if(f)B(y+a+H/2,x+b+W/2)=s/1.8;}}}C;}
//int i,j,k,t;D a,b,c,x,y,z,d,r=0;PG;for(t=0;t<20;t++){for(k=0;k<4;k++){a=sin(r+k*PI/2.);b=cos(r+k*PI/2.);c=5+120*sin(t*PI/20.);for(i=-W/2;i<W/2;i++){for(j=-H/2;j<H/2;j++){d=abs(a*i+b*j+c)/sqrt(a*a+b*b);if(d<1.2)B(j+H/2,i+W/2)=0xFF;}}}r+=PI/20.;C;}
//int i,j,k,t;D a,b,c,x,y,z,d,r=0;PG;for(t=0;t<64;t++){for(j=0;j<H;j++){for(i=0;i<W;i++){a=4*sin(i/4.+cos(j/40.)*20*sin(r/2.)+cos(t/2.+j/4.));if(abs(a)>2){B(j,i)=0xFF;}}}r+=PI/16.;C;}
//int i,j,k,l,t;D a,b,c,x,y,z,d,r=0;PG;for(t=0;t<64;t++){for(k=0;k<6;k++){for(j=0;j<H;j++){for(i=0;i<W;i++){x=i-W/2+30*sin(r+k*PI);y=j-H/2+50*cos(r+k*PI/3.);b=abs(sin(r)*40)+40;d=sqrt(x*x+y*y)-b;a=abs(60*sin(r));if(d<0 && d>-a){d+=a;l=d+1;l=rand()%l/2;d+=l;if(d>a)d=a;a=255*cos(d*PI/2./a);B(j,i)=a;}else if(d<0)B(j,i)=255;}}}r+=PI/16.;C;}
//int i,j,k,l,t,c,p,a,x,y,z,d,r=0;char *FNT=(char*)0xFFA6E;PG;for(t=0;t<64;t++){for(i=0;i<5;i++){c="DANK@"[i]+1;for(x=0;x<8;x++){for(y=0;y<8;y++){p=FNT[c*8+y];p&=0x80>>x;if(p){z=34;for(j=0;j<z;j++)for(k=0;k<z;k++)B(y*z+j,x*z+k+i*8*z-t*26+W)=0x7f;}}}}C;}
//D i,j,k,a,b,c,d,tj,r;static Buffer bf;L B(iy,ix)=(ix+iy)%255;for(i=0;i<64;i++){tj=D(rand()%32)-16;for(x=0;x<W;x+=32){for(y=0;y<H;y+=32){for(j=0;j<32;j++){for(k=0;k<32;k++){a=x-W/2;b=y-H/2;r=PI/64.;c=a*cos(r)-b*sin(r);d=a*sin(r)+b*cos(r);a=c-a;b=d-b;bf(y+k+b+tj,x+j+a+tj)=B(y+k+tj,x+j+tj);}}}}L B(iy,ix)=bf(iy,ix);F;}
//I i;D a,r,d,x,y,cx,cy,by=0;PG;U8 *p=P.Get();p[764]=0;p[763]=0;p[762]=255;cx=-60;cy=H/2;for(i=0;i<64;i++){{L B(iy,ix)=0x7f;}a=0;for(r=0;r<PI*120;r+=.1){for(i=0;i<4;i++){x=r*cos(a+i*2);y=r*sin(a+i*2);B(y+cy,x+cx)=0;}a+=.01;}L{x=ix-W/2;y=iy-H/2;d=sqrt(x*x+y*y);if(d<60)B(cy+y,cx+x)=0xFF;}if(cx<W/2)cx+=8;else{r=(rand()%10)+10;L{a=iy+r*sin(ix/20.+i/20.);if(a<by){B(iy,ix)=254;}}by+=4;}C;}
//I i,j,k,a,b,c;D d,x,y=0;for(i=0;i<64;i++){I p[4]={-20,30,12,30};D *z,q[8]={3,90,10,44,3,70,8,40};{L{B(iy,ix)=40;}}for(j=0;j<2;j++){a=p[j*2];b=p[j*2+1];for(k=0;k<2;k++){z=q+k*4;L{x=ix-W/2;y=iy-H/2;d=sqrt(x*x/z[0]+y*y/z[1]);if(d<z[2] && y<0)B(iy+b,ix+a)=z[3];}}}U8 *f=(U8*)0xFFA6E;char *t="LTTI9Z&";a=-200+i*16;if(a>99)a=99;x=a;y=190;j=0;while(t[j]){k=t[j++]-5;for(a=0;a<8;a++)for(b=0;b<8;b++)if(f[k*8+b]&(0x80>>a))B(y+b,x+a+j*8)=15;}C;}
//I i,j,k,a,b,c;D d,x,y=0,s;for(i=0;i<32;i++){U8 *f=(U8*)DOSCHAR;for(j=0;j<40;j++){x=100+20*sin(j/PI*5.+i*PI/16.);y=32*4+H-20-j*32-i*4;s=(H-y)/80.;for(a=0;a<8*s;a++)for(b=0;b<8*s;b++)if(f['Z'*8+(I)(b/s)]&(0x80>>(I)(a/s)))B(y+b,x+a)=15;}C;}
//I i,j,k,m,n,o;D a,b,c,x,y;for(i=0;i<32;i++){{L{x=ix-W/2;y=iy-H/2;a=sqrt(x*x+y*y);if(a<70)B(y+H/2,x+H/2)=44;}}{L{x=ix-W/2;y=iy-H/2;c=1.7;b=.2+.8*sin(i*PI/16);b*=b;a=sqrt(x*x/c+y*y/b);if(a<50 && y>0)B(y+H/2+10,x+H/2)=0;}}for(j=0;j<2;j++){L{x=ix-W/2;y=iy-H/2;a=sqrt(x*x+y*y);if(a<10)B(y+H/2-20,x+H/2-20+40*j)=0;}}j=0;x=y=10;while(o="IBIB"[j]){for(k=0;k<64;k++){m=k%8;n=k/8;if(((U8*)0xFFA6E)[(o-1)*8+n]&(0x80>>m))B(y+n,x+m+j*8)=15;}j++;}C;}
//I j,k,l,m;D i,a,x,y,z,cx,sx,tx,ty,tz;PG;for(i=0;i<32;i++){I p[]={-1,-1,1,-1,-1,1,1,1};D *t[]={&x,&y,&z,&y,&x,&z,&z,&x,&y};for(m=0;m<3;m++){for(j=0;j<12;j++)for(a=0;a<20;a+=.2){k=j%4;l=(j/4)*3;t[l][0]=a-10;t[l+1][0]=10*p[k*2];t[l+2][0]=10*p[k*2+1];cx=cos(i*PI/16+m/7.);sx=sin(i*PI/16+m/7.);tx=x;ty=y*cx+z*sx;tz=y*(-sx)+z*(cx);tz+=30+8*sin(i*PI/16);x=tx*80/tz;y=ty*80/tz;tz=(tz=1400-tz*tz)>255?255:tz<0?0:tz;B(y+H/2,x+W/2)=tz;}}C;}
//I j;D i,a,b,c,x,y,m,rx,rz,z,sz,t[]={30,32,65,67};PG;rx=0;rz=0;sz=20;for(i=0;i<23;i++){for(rx=rz;rx<rz+sz;rx++){z=0;for(j=0;j<4;j+=2){if(rx>=PI*t[j] && rx<=PI*t[j+1])z=1;}if(z){for(a=rx;a<rx+1;a+=.1){b=80*sin(a);for(x=a-1;x<a+1;x++){for(y=0;y<H;y++){c=sqrt(pow(a-x,2)+pow(b+H/2-y,2));if(c<3){B(y,x)=255-c*30;}}}}}else{B(H/2,rx)=0xFF;{L{m=B(iy,ix);m-=2;if(m<0)m=0;B(iy,ix)=m;}}}}rz+=sz;F;}
//I i,j,k,a,b,x,y,w;for(i=0;i<256;i++){P=149+i/10;P=5+i/1.8;P=0;}for(i=0;i<3;i++)P[0][i]=0xFF;for(i=0;i<64;i++){w=0;{L{B(iy,ix)=0;}}for(x=W/4;x<W*.75;x++){if(x<W/2)w+=2;else w-=2;for(j=0;j<w;j++){B(H/2-w/2+j,x)=1+abs(j-w/2)*3.1;}}k=i;if(k>41)k=41;for(a=0;a<2;a++){D b[]={-1,W/4,1,W*.745};w=0;for(x=0;x<W/4-k/1.1;x++){w+=2;for(j=0;j<w;j++){B(H/2-w/2+j+k/2*b[a*2],b[1+a*2]-(x+k)*b[a*2])=x*(3+k/18.)+1;}}}F;}
///*amiga blocks*/ I i;D cz,sz,r,tx,ty,a,b,s,t;t=s=0;r=PI*11/8;for(i=0;i<32;i++){cz=cos(r);sz=sin(r);{L{a=ix;b=iy/4.;tx=a*cz+b*sz;ty=-sz*a+b*cz;if(s)B(H-ty-t*H/4,tx)=15;else B(H-ty-t*H/4,W-tx)=15;}}{L{B(H+iy-t*H/4,ix)=15;}}if(i<25)r+=PI/8;if(r>=PI*17/8){r=PI*11/8;s=!s;t++;}C;}
//green cube I n,i,j;D f,t,d,u,v,w,a,b,c,p,q,r,l,k;for(i=0;i<768;i++)P=i%3==1?i:0;for(n=0;n<16;++n){f=n/32.*3.14;k=sin(PI*n/16.);l=.7;L {t=0;for(i=0;i<8;i++){u=x*l*t;v=y*l*t;w=t*l-8.5;ROT(u,v,f*2*k);ROT(u,w,f*3);r=1+2.*sin(PI*n/8.);p=u<-r?-r:(u>r?r:u);q=v<-1?-1:(v>r?r:v);r=w<-1?-1:(w>1?1:w);a=u-p;b=v-q;c=w-r;p=a*a+b*b+c*c;if(p>2000)break;d=sqrt(p);t+=d;if(d<.03)break;}t=1/(1+t*t+d*100);t*=20;t=t>1?1:t;B[iy][ix]=t*255;}F;}
//cube letters I i,j,k,m,n,o;D t,u,v,w,a,b,c,r,d;PG;for(i=0;i<16;i++){r=PI/16.*i;L{for(m=0;m<8;m++)for(n=0;n<8;n++){o=f8['Q'*8+7-n]&(1<<m);if(!o)continue;t=10;while(t>0){u=x*12.;v=y*12.;w=t;ROT(u,v,0);ROT(u,w,r);u+=m*3.2-12;v+=n*3.2-12;a=u<-1?-1:u>1?1:u;b=v<-1?-1:v>1?1:v;c=w<-1?-1:w>1?1:w;a=a-u;b=b-v;c=c-w;a=a*a+b*b+c*c;d=sqrt(a);if(d<.1){d=255.*t*.12;d+=B(iy,ix);if(d<0)d=0;if(d>255)d=255;B(iy,ix)=d;break;}t-=(d);}}}C;}
//dragon curve I k,l,x,y,dx,dy,t,d,a;struct Q{U8 *s;I i;};vector<void*>z;U8 *s;y=x=W/3;l=k=0;Q *q=new Q;q->s=(U8*)"FX";q->i=20;z.insert(z.begin(),q);dx=1;dy=0;N:do{q=(Q*)z.at(k);d=q->i;s=q->s;while((a=*s++)!=0){a-='F';if(d&&a>0){q->s=s;q->i=d;z.at(k)=q;s=(U8*)(a==18?"X+YF+":"-FX-Y");q=new Q;q->i=--d;q->s=s;z.push_back(q);k++;goto N;}if(a<0){a=a==-27?1:-1;t=a*dy;dy=-a*dx;dx=t;}if(!a){B(y,x)=l++/99;x+=dx;y+=dy;}}z.pop_back();k--;}while(k);F;
//hex tunnel I i,j,k,l,m;D a,b,c,d,e,x,y,z;for(a=0;a<256;a++){P=a/7;P=a/1;P=a/1;}for(m=i=0;i<24;i++){for(k=0;k<64;k++)for(j=0;j<6;j++)for(x=0;x<100;x++){y=88;z=0;a=x-50;b=y;c=z;ROT(a,b,PI/3*j);a-=200;ROT(a,c,PI/32*k-PI/48*i);c=c+500;a=a*500/c;b=b*500/c;e=700-c;e/=1;e=e<0?0:e>255?255:e;if(c>320)B(b+H/2,a+400)=e;}C;}
{//START BLOCK
//MAX length 439
//export_image();

I i,j;
D x,y,a,b,c,d,e,u,v,r;

//for(i=0;i<1;i++)
{
/*
	{
	D p[]={
128.6613213834577,115.30930478216203,
130.2254956418043,90.28251664861641,
129.44340851263098,71.5124255484572,



		

	};
	for(j=0;j<sizeof(p)/sizeof(p[0]);j+=6){
		x=pow(p[j+2],2)+pow(p[j+3],2)-p[j]*p[j]-pow(p[j+1],2);
		y=pow(p[j+4],2)+pow(p[j+5],2)-pow(p[j+2],2)-pow(p[j+3],2);
		a=2*p[j+2]-2*p[j];
		b=2*p[j+3]-2*p[j+1];
		c=2*p[j+4]-2*p[j+2];
		d=2*p[j+5]-2*p[j+3];
		e=1/(a*d-c*b);
		u=d*x-b*y;
		v=-c*x+a*y;
		x=e*u;
		y=e*v;
		r=sqrt(pow(p[j+4]-x,2)+pow(p[j+5]-y,2));
		a=atan2(p[j+1]-y,p[j]-x);
		if(a<0)
			a+=2*PI;
		b=atan2(p[j+3]-y,p[j+2]-x);
		if(b<0)
			b+=2*PI;
		c=atan2(p[j+5]-y,p[j+4]-x);
		if(c<0)
			c+=2*PI;
		d=min(min(b,c),a);
		b=max(max(b,c),a);
		a=d;
		d=b-a;
		//printf("x=%f y=%f r=%f min=%f rot=%f\n",x,y,r,a*180/PI,b*180/PI);
		printf("%.1f,%.1f,%.1f,%.1f,%.1f,\n",x,y,r,a*180/PI,b*180/PI);
	}
	}
*/
	D p[]={
174,160,111,153,286,
96,189,30,-1,135,
188,170,63,163,254,
199,88,35,70,144,
187,144,32,-40,3,
226,128,17,-61,100,
238,109,5,-76,100,
240,98,6,-90,80,
211,81,28,-99,130,
109,185,8,0,360,
83,187,8,0,360,
93,176,4,0,360,
98,196,14,280,343,
69,198,17,293,348,
89,339,171,261,279,
//-80,89,210,-6,5,
	};
	for(j=0;j<15*5;j+=5){
		for(a=p[j+3];a<=p[j+4];a+=.1){
			b=p[j+2];
			c=PI/180;
			x=cos(a*c)*b;
			y=sin(a*c)*b;
			B(H-(p[j+1]+y),p[j]+x)=15;
		}
	}
	/*
	for(;j<4);j+=4)
	{
		x=p[j];
		y=p[j+1];
		for(a=0;a<p[j+3];a++){
			b=p[j+2]*PI/180;
			B(y+sin(b)*a,x+cos(b)*a)=15;
		}
	}
*/
	C;
}

}//END BLOCK
#else
//	PG;D i,j,k,t=0,q=0;for(i=0;i<64;i++){t+=PI/8;for(j=0;j<200;j++){q+=.06;for(k=0;k<200;k++){D x,y,z;y=j*2;x=sin(q+t+k/20)*30+100;z=k+200;if(z>0){x=x*256/z;y=y*256/z;I c;z/=2;if(z>255)c=255;else c=z;B(x,y)=c;}}}C;}

//I f,i,s,x,y;for(f=0;f<64;f++,C){static D t,a,b,w[S*9]={W/2,H/2};w[2]=70+cos(f*PI/15)*20;D *d=w,*n=w+3,*e;for(s=0;s<5;s++)for(e=n;d<e;d+=3){t=d[2];for(a=0;a<2*PI;a+=PI/4,n+=3){b=a+f*PI/30;n[0]=d[0]+cos(b)*t;n[1]=d[1]+sin(b)*t;n[2]=d[2]*(0.05*sin(f*PI/15)+0.25);}B(d[1],d[0])=32+s*3;}}
//for(D f=0;f<2*M_PI;f+=0.1,C)for( D x=-1;x<1;x+=0.02)for(D z=-1;z<1;z+=0.02){ D y=sin(2*f)*sqrt(x*x+z*z),c=cos(f),s=sin(f),d=cos(-7),t=sin(-7),k=c*x-s*z,l=t*s*x+d*y+c*t*z,m=2+d*s*x-t*y+d*c*z;X(l/m,k/m)=15;}
//int i,j,S=999,W=320,H=240,k=0;float D=S,x,y,z,f,e;for(;k++<16;){for(j=1;j++<D;){e=j/D;z=log(2*e);for(i=0;i++<S;){f=2*M_PI*i/S;x=W*e*cos(f)+160;y=-25*z+H*e*sin(f)+60;x=x<0?0:x>(W-1)?W-1:x;y=y<0?0:y>(H-1)?H-1:y;B[(int)y][(int)x]=((j/6+k)&15)*16+((i/8+k)&15);}}F;}
//typedef float f;for(int c=0;c<15;++c){f b=0.1+6.3*c/15.0,s=sin(b);f q=cos(b)*s,r=sin(b)*s;s=cos(b);for(int i=0;i<W*H;++i){int p=i%256;P[p][0]=P[p][1]=P[p][2]=p;f t=0;for (int p=0;p<15;++p){f h=(2*(i%W)/f(W)-1)*t;f j=(1-2*(i/W)/f(H))*t;f k=0.8*t-0.65;f ev=h*q+j*r+k*s;f l=h-q*ev;f m=j-r*ev;f n=k-s*ev;f o=0.35/sqrt(l*l+m*m+n*n);f u=h-l*o;f v=j-m*o;f w=k-n*o;t+=sqrt(u*u+v*v+w*w)-0.1;} B[0][i]=255/(1+t);} F;}
//int i,x,y;for(float f=1;f>0;f-=0.1) {;for(y=0;y<H;y++) for(x=0;x<W;x++) { std::complex<double> c(-0.1011-f+(2*f*x)/W,0.9563-f+(2*f*y)/H),z(0,0); for(i=50;i>0 && std::abs(z) < 2;--i) z=z*z+c; if (i) B[y][x]=i; }F;memset(B,0,S);}
//PG;I i,j,k,s[100*3];for(i=0;i<100;i++){s[i*3]=rand()%H;s[i*3+1]=rand()%W;s[i*3+2]=rand()%255;}for(k=0;k<4;k++){for(i=0;i<100;i++){B(s[i*3],s[i*3+1])=s[i*3+2];I d=s[i*3+2]/10;if(d==0)d=2;s[i*3+1]=(s[i*3+1]+d)%W;}C;}
//I c,i,y,x,j,k,l,s,q;D p[16],*z,t,f,d,u,v,w,o,g,e;for(c=0;c<60;++c){f=c*0.104;z=p;for(i=0;i<4;++i){d=f*2+i*4.18;v=3+sin(d)+sin(f*2)*1.3;w=cos(f)/v;o=sin(f)/v;u=cos(d);v=u*o;u*=w;*z++=u+o;*z++=v-w;*z++=u-o;*z++=v+w;}for(s=0;s<W*H;++s){x=s%W;y=s/W;u=2*x/D(W)-1;v=1-2*y/D(H);B[y][x]=0;t=1;for(q=0;q<18;){k=q/3;i=q%3;l=(i+k)*2;j=(++q%3+k)*2;w=p[j]-p[l];g=u-p[l];o=v-p[++l];e=p[j+1]-p[l];w=o*w-g*e;w*=k&1?-1:1;t*=w>0?w:0;if(i>1){if(t>0)B[y][x]=k+1;t=1;}}}F;}
//#define ct 200000 PG;static I i,j,k,s[ct];D rx=0,ry=0,rz=0,scl=300;for(i=0;i<ct;i++){s[i]=W/2-rand()%W;}for(k=0;k<64;k++){for(i=0;i<ct/3;i+=3){D cx,cy,cz,sx,sy,sz,tx,ty,tz;I x,y,z;x=s[i];y=s[i+1];z=s[i+2];cx=cos(rx);cy=cos(ry);cz=cos(rz);sx=sin(rx);sy=sin(ry);sz=sin(rz);tx=x*cy*cz+y*cy*sz-z*sy;ty=x*(sx*sy*cz-cx*sz)+y*(sx*sy*sz+cx*cz)+z*sx*cy;tz=x*(cx*sy*cz+sx*sz)+y*(cx*sy*sz-sx*cz)+z*cx*cy+1000;if(tz>0)B(tx*scl/tz+H/2,ty*scl/tz+W/2)=(1+cos(tz/W/5*PI))*180;}rz+=PI/32;ry+=PI/32;rx+=PI/32;C;}
//I t,f,i,x,y;for(x=0;x<W;x++){i=0;for(y=0;y<H;y++){if(!i) t=i=15+rand()%32;if(!y) i-=rand()%t;B[y][x]=(i-- *255)/t;}}for(f=0;f<16;f++,F)for(i=0;i<256;i++){P=i+f*16;P=P=0;}
//D a,f,i,x,y,t=0,r=0,d=0;a=i=0;PG;t=56*4*PI/360;for(f=0;f<64;f++){for(x=0;x<H;x++){for(y=0;y<W;y++){D cy,sy,tx,ty,tz,z=10*sin(a)*sin(x/8+r)*cos(y/8);cy=cos(t);sy=sin(t);tx=(x+d)*cy-z*sy;ty=y;tz=(x+d)*sy+z*cy;tz+=50;if(tz!=0){I i,j;i=tx*400/tz+H;j=ty*400/tz+0*W/8;tz=-tz+200;if(tz>255)tz=255;else if(tz<0)tz=0;B(i,j)=tz;}}}r+=12*PI/360;a+=PI/360*25;t+=2*PI/360;C;}
//int x,y,z;uint8_t w,BB[H][W];memset(BB,0,S);for(x=y=0;x<256;x++,y+=4){P=x<64?y:255;P=x>63?x<128?y:255:0;P=x>127?x<192?y:255:0;}for(z=0;z<800;z++){for(x=0;x<W;x++)BB[239][x]=rand()&255;for(y=80;y<239;y++)for(x=0;x<W;x++,w=x==W?0:1){w=(BB[y][x]+BB[y][x+w]+BB[y+1][x]+BB[y+1][x+w]+BB[y+1][x]+BB[y+1][x+w])/6;if(!(y&7))w--;BB[y][x]=w&128?0:w;B[y][x]=((((char*)0xFFA6E)["Lorem ipsum dolor sit amet edipisc elit"[x/8]*8+(y&7)]<<(x&7))&128)?BB[y][x]:0;}if(!(z&3))F;}
//I i,j,k,c,x,y,z;PG;FILE *fi;system("dir >e:\\t.txt");fi=fopen("e:\\t.txt","r");char t[0x10000]={0},*tmp,*l;fread(t,1,sizeof(t),fi);fclose(fi);tmp=t;for(z=0;z<64;z++){i=x=y=0;l=0;while(tmp[i]!=0){if(tmp[i]=='\n'){x=0;y+=8;if(l==0)l=tmp+i+1;}else if(tmp[i]>=' ')for(j=0;j<8;j++){for(k=0;k<8;k++){c=((((char*)DOSCHAR)[(tmp[i]*8)+j]));if(c&(1<<(7-k)))c=0xFF;else c=0;B(y+j,x+k)=c;}}x+=8;i++;}C;if(l)tmp=l;}
PG;for(I c=0;c<64;++c){D k=c/64.0*6.28;for(I y=0;y<H;++y){for(I x=0;x<W;++x){D u=x/D(W)*2-1;D v=1-y/D(H)*2;D p=cos(k);D q=sin(k)*cos(k);D a=u-p;D b=v-q;D l=sqrt(a*a+b*b);a/=l;b/=l;D t = 0.0;for(int i=0;i<16;++i){D m=100.0;D h=p+a*t;D f=q+b*t;for(int j=0;j<3;++j){D g=j/3.0*6.28;D r=h-cos(g)*0.5;D s=f-sin(g)*0.5;D o=sqrt(r*r+s*s)-0.05;m=m<o?m:o;} if(m<0.1){t = 1000.0;break;} m=fabs(m)*0.1;t+=m;if(t>=l){break;} } B[y][x] = 255.0 / (1.0 + t*t);} } F;}
#endif

return 0;
}
/*
 *----->y
 |
 |
 \/
 x
*/
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


test();
/*===============================================*/

frame_limit:
	if(0)
	if(frame_count>0)
	{
		static void *gifimage=0;
		static void *gsdata=0;
		unsigned char *tmp=anim_palette;
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
				int j;
				tmp=anim_palette+(i*768);
				for(j=0;j<256;j++){
					ctable[j*3]=tmp[j*3];
					ctable[j*3+1]=tmp[j*3+1];
					ctable[j*3+2]=tmp[j*3+2];
				}
				fputgif(gsdata,0,0,W,H,buffers+(i*H*W),ctable);
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
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR     lpCmdLine,int       nCmdShow)
{
	open_console();
	main(0,0);
	return 0;
}
