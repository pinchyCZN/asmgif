#include "gifhead.h"
#include "vgapal.h"
#include <setjmp.h>

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
*/

/*===============================================*/

int test()
{
#if 0
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
*/
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
D a,f,i,x,y,t=0,r=0,d=0;a=i=0;PG;t=56*4*PI/360;for(f=0;f<64;f++){for(x=0;x<H;x++){for(y=0;y<W;y++){D cy,sy,tx,ty,tz,z=10*sin(a)*sin(x/8+r)*cos(y/8);cy=cos(t);sy=sin(t);tx=(x+d)*cy-z*sy;ty=y;tz=(x+d)*sy+z*cy;tz+=50;if(tz!=0){I i,j;i=tx*400/tz+H;j=ty*400/tz+0*W/8;tz=-tz+200;if(tz>255)tz=255;else if(tz<0)tz=0;B(i,j)=tz;}}}r+=12*PI/360;a+=PI/360*25;t+=2*PI/360;C;}

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
