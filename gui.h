#include <windows.h>
#include <commctrl.h>
#include "resource.h"

int CALLBACK  dlg(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int current_frame=0;
	static int frame_delay=10;
	static int timer;
	static int animate=FALSE;
	static int max_delay=150;
	switch(msg){
	case WM_INITDIALOG:
		SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGE,TRUE,MAKELONG(0,63));
		break;
	case WM_TIMER:
		InvalidateRect(hwnd,0,FALSE);
		if(animate){
			current_frame=(current_frame+1)%64;
		}
		break;
	case WM_PAINT:
		{
			HDC hdc;
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
			if(animate){
				char str[80];
				sprintf(str,"frame=%i delay=%i ms",current_frame,frame_delay);
				SetDlgItemText(hwnd,IDC_TEXT,str);
			}
		}
		break;
	case WM_HSCROLL:
		{
			int scroll;
			printf("scroll, %08X %08X\n",wparam,lparam);
			scroll=SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_GETPOS,0,0);
			if(!animate){
				static int last_scroll=0;
				if(GetKeyState(VK_RBUTTON)&0x8000){
					if(scroll>last_scroll)
						scroll+=10;
					else
						scroll-=10;
					SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETPOS,TRUE,scroll);
				}
				current_frame=scroll;
				if(current_frame<0)
					current_frame=0;
				else if(current_frame>63)
					current_frame=63;
				InvalidateRect(hwnd,0,FALSE);
				last_scroll=scroll;
				{
					char str[80];
					sprintf(str,"frame=%i",current_frame);
					SetDlgItemText(hwnd,IDC_TEXT,str);
				}
			}
			else{
				frame_delay=scroll;
				if(frame_delay<10)
					frame_delay=10;
				else if(frame_delay>max_delay)
					frame_delay=max_delay;
				if(timer)
					KillTimer(hwnd,timer);
				timer=SetTimer(hwnd,1337,frame_delay,NULL);
				printf("delay=%i\n",frame_delay);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			break;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wparam)){
		case IDC_ANIMATE:
			if(IsDlgButtonChecked(hwnd,IDC_ANIMATE)){
				char str[40];
				animate=TRUE;
				sprintf(str,"speed=%i ms / frame",frame_delay);
				SetDlgItemText(hwnd,IDC_TEXT,str);
				if(frame_delay<10)
					frame_delay=10;
				else if(frame_delay>max_delay)
					frame_delay=max_delay;
				if(timer)
					KillTimer(hwnd,timer);
				timer=SetTimer(hwnd,1337,frame_delay,NULL);
				SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGE,TRUE,MAKELONG(10,max_delay));
				SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETPOS,TRUE,frame_delay);
			}
			else{
				char str[40];
				animate=FALSE;
				sprintf(str,"frame=%i",current_frame);
				SetDlgItemText(hwnd,IDC_TEXT,str);
				if(timer)
					KillTimer(hwnd,timer);
				timer=0;
				SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGE,TRUE,MAKELONG(0,63));
				SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETPOS,TRUE,current_frame);
			}
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
