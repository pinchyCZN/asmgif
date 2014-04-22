#include <windows.h>
#include "resource.h"

#include "gifhead.h"
#include "vgapal.h"
int frame_count=0;
unsigned char buffers[64*W*H];
Palette P;
Buffer B;



void SaveFrame(bool clear)
{
	unsigned char *b=B.Get();
	memcpy(buffers+(frame_count*W*H),b,W*H);
	if(clear)
		memset(b,0,W*H);
	frame_count++;
	if(frame_count>=64)
		frame_count=63;
}



int CALLBACK  dlg(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int current_frame=0;
	static int timer;
	static int pause=TRUE;
	switch(msg){
	case WM_INITDIALOG:
		timer=SetTimer(hwnd,1337,20,NULL);
		break;
	case WM_TIMER:
		InvalidateRect(hwnd,0,FALSE);
		if(!pause){
			current_frame=(current_frame+1)%64;

		}
		break;
	case WM_PAINT:
		{
			HDC hdc,hdcMem;
			PAINTSTRUCT ps;
			BITMAPINFO bmi;
			unsigned char *frame;
			char buffer[W*H*3];
			char flip[W*H*3];
			int i,j;
			frame=buffers+(current_frame*W*H);
			for(i=0;i<W*H;i++){
				unsigned char r,g,b;
				unsigned char *tmp=P[frame[i]];
				r=tmp[0];
				g=tmp[1];
				b=tmp[2];
				flip[i*3]=b;
				flip[i*3+1]=g;
				flip[i*3+2]=r;
			}
			for(i=0;i<H;i++){
				for(j=0;j<W*3;j++){
					buffer[(H-i-1)*W*3+j]=flip[i*W*3+j];
				}

			}			

			hdc=BeginPaint(hwnd,&ps);
			memset(&bmi,0,sizeof(BITMAPINFO));
			bmi.bmiHeader.biBitCount=24;
			bmi.bmiHeader.biWidth=W;
			bmi.bmiHeader.biHeight=H;
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biSize=sizeof(bmi);
			SetDIBitsToDevice(hdc,0,30,W,H,0,0,0,H,buffer,&bmi,DIB_RGB_COLORS);
			EndPaint(hwnd,&ps);
			char str[80];
			sprintf(str,"%i",current_frame);
			SetDlgItemText(hwnd,IDC_FRAME,str);
		}
		break;
	case WM_MOUSEMOVE:
		{
			static lastx=0;
			int x=LOWORD(lparam);
			int delta;
			//printf("key=%08X %08X\n",wparam,lparam);
			if(!pause)
				break;
			delta=x-lastx;
			if(delta>2)
				delta=2;
			else if(delta<-2)
				delta=-2;
			current_frame+=delta;
			if(current_frame<0)
				current_frame=0;
			else if(current_frame>=64)
				current_frame=63;
			lastx=x;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wparam)){
		case IDC_PAUSE:
			pause=!pause;
			break;
		case IDC_NEXT:
			current_frame=(current_frame+1)%64;
			break;
		case IDOK:
			EndDialog(hwnd,0);
			break;
		case IDCANCEL:
			EndDialog(hwnd,0);
			break;
		}
		break;
	}
	return 0;
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





	DialogBox(0,(LPSTR)IDD_DIALOG1,0,dlg);

	return 0;
}