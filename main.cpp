#include <windows.h>
#include "resource.h"

#include "gifhead.h"
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
			int i;
			frame=buffers+(current_frame*W*H);
			for(i=0;i<W*H;i++){
				unsigned char r,g,b;
				unsigned char *tmp=P[frame[i]];
				r=tmp[0];
				g=tmp[1];
				b=tmp[2];
				buffer[i*3]=r;
				buffer[i*3+1]=g;
				buffer[i*3+2]=b;
			}
			

			hdc=BeginPaint(hwnd,&ps);
			memset(&bmi,0,sizeof(BITMAPINFO));
			bmi.bmiHeader.biBitCount=24;
			bmi.bmiHeader.biWidth=H;
			bmi.bmiHeader.biHeight=W;
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biSize=sizeof(bmi);
			SetDIBitsToDevice(hdc,0,30,H,W,0,0,0,W,buffer,&bmi,DIB_RGB_COLORS);
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
			delta=lastx-x;
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
int main()
{
	PG;
	I i,j,k,s[100*3];
	for(i=0;i<100;i++){
		s[i*3]=rand()%H;
		s[i*3+1]=rand()%W;
		s[i*3+2]=rand()%255;
	}
	for(k=0;k<64;k++){
		for(i=0;i<100;i++){
			B(s[i*3],s[i*3+1])=s[i*3+1];
			I d=s[i*3+2]/10;
			if(d==0)
				d=2;
			s[i*3+1]=(s[i*3+1]+d)%H;
		}
		F;
	}




	DialogBox(0,(LPSTR)IDD_DIALOG1,0,dlg);

	return 0;
}