#ifndef USEWX

#include <windows.h>
#include <commctrl.h>
#include <fcntl.h>
#include <io.h>
#include "resource.h"

void open_console()
{
	char title[MAX_PATH]={0};
	HWND hcon;
	FILE *hf;
	static BYTE consolecreated=FALSE;
	static int hcrt=0;

	if(consolecreated==TRUE)
	{
		GetConsoleTitle(title,sizeof(title));
		if(title[0]!=0){
			hcon=FindWindow(NULL,title);
			ShowWindow(hcon,SW_SHOW);
		}
		hcon=(HWND)GetStdHandle(STD_INPUT_HANDLE);
		FlushConsoleInputBuffer(hcon);
		return;
	}
	AllocConsole();
	hcrt=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);

	fflush(stdin);
	hf=_fdopen(hcrt,"w");
	*stdout=*hf;
	setvbuf(stdout,NULL,_IONBF,0);
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		ShowWindow(hcon,SW_SHOW);
		SetForegroundWindow(hcon);
	}
	consolecreated=TRUE;
}

int display_vga_pal(HWND hwnd,int mx,int my)
{
	extern unsigned char vgapal[];
	HDC hdc;
	PAINTSTRUCT ps;
	BITMAPINFO bmi;
	int i,j,k,x,y;
	int yoffset=30;
	int hover=0;
	int inside=FALSE;
#define	_size 16
#define _height (_size*16)
#define _width (_size*16)
	static char buffer[_size*_size*256*3];
	{
		int c;
		char str[80]={0};
		x=mx/_size;
		y=(my-yoffset)/_size;
		i=x+y*_size;
		hover=c=(vgapal[i*3]<<16)|(vgapal[i*3+1]<<8)|(vgapal[i*3+2]);
		if(my>=yoffset && my<=yoffset+_size*16 && mx<=_size*16){
			sprintf(str,"index=%i c=%06X %i,%i",i,c,mx,my);
			inside=TRUE;
		}
		else
			str[0]=0;
		SetDlgItemText(hwnd,IDC_TEXT,str);
	}
	x=y=0;
	for(i=0;i<256;i++){
		for(j=0;j<_size;j++){
			for(k=0;k<_size;k++){
				buffer[x*3+y*_width*3+j*3+k*_width*3]=vgapal[i*3+2];
				buffer[x*3+y*_width*3+j*3+k*_width*3+1]=vgapal[i*3+1];
				buffer[x*3+y*_width*3+j*3+k*_width*3+2]=vgapal[i*3+0];
			}
		}
		x+=_size;
		if(x>=_width){
			x=0;
			y+=_size;
		}
		if(y>=_height)
			break;
	}
	hdc=BeginPaint(hwnd,&ps);
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader.biBitCount=24;
	bmi.bmiHeader.biWidth=_width;
	bmi.bmiHeader.biHeight=-_height;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biSize=sizeof(bmi);
	SetDIBitsToDevice(hdc,0,yoffset,_width,_height,0,0,0,_height,buffer,&bmi,DIB_RGB_COLORS);

	if(!(mx<_width && my<=_height+yoffset && my>=yoffset)){
		hover=GetSysColor(COLOR_BTNFACE);
		hover=((hover&0xFF)<<16)|(hover&0xFF00)|((hover>>16)&0xFF);
	}
	{
		int w=_width/2,h=_height/2;
		for(i=0;i<w*h;i++){
			buffer[i*3]=hover;
			buffer[i*3+1]=hover>>8;
			buffer[i*3+2]=hover>>16;
		}
		bmi.bmiHeader.biWidth=w;
		bmi.bmiHeader.biHeight=h;
		SetDIBitsToDevice(hdc,_width,yoffset,w,h,0,0,0,h,buffer,&bmi,DIB_RGB_COLORS);
	}
	EndPaint(hwnd,&ps);
	return 0;
}
struct CTRL_POS{
	int id;
	int x,y;
};
struct CTRL_POS ctrls[]={
	{IDC_ANIMATE,0,0},
	{IDC_SLIDER,0,0},
	{IDC_TEXT,0,0},
	{IDC_VGA,0,0}
};
int init_ctrl_pos(HWND hwnd)
{
	int i;
	for(i=0;i<sizeof(ctrls)/sizeof(CTRL_POS);i++){
		RECT rect={0};
		HWND hctrl=GetDlgItem(hwnd,ctrls[i].id);
		GetClientRect(hctrl,&rect);
		MapWindowPoints(hctrl,hwnd,(POINT*)&rect,2);
		ctrls[i].x=rect.left;
		ctrls[i].y=rect.top;
	}
	return 0;
}
int move_ctrls(HWND hwnd,int dx,int dy)
{
	int i;
	for(i=0;i<sizeof(ctrls)/sizeof(CTRL_POS);i++){
		RECT rect={0};
		HWND hctrl=GetDlgItem(hwnd,ctrls[i].id);
		int x,y;
		x=ctrls[i].x+dx;
		y=ctrls[i].y+dy;
		SetWindowPos(hctrl,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	return 0;
}
int get_delta_rect(RECT *a,RECT *b,int *dw,int *dh)
{
	*dw=(b->right-b->left)-(a->right-a->left);
	*dh=(b->bottom-b->top)-(a->bottom-a->top);
	return 1;
}
int CALLBACK  dlg(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int current_frame=0;
	static int frame_delay=100; //66;
	static int timer;
	static int animate=FALSE;
	static int max_delay=150;
	static int show_vga_pal=FALSE;
	static int mx=0,my=0;
	static RECT orig_rect={0};
#define Y_OFFSET 30
	switch(msg){
	case WM_INITDIALOG:
		SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGE,TRUE,MAKELONG(0,63));
		SendDlgItemMessage(hwnd,IDC_ANIMATE,BM_CLICK,0,0);
		GetWindowRect(hwnd,&orig_rect);
		init_ctrl_pos(hwnd);
		{
			char title[200];
			HANDLE hConWnd;
			RECT rect={0};
			GetWindowRect(hwnd,&rect);
			GetConsoleTitle(title,sizeof(title));
			hConWnd=FindWindow(NULL,title);
			SetWindowPos((HWND)hConWnd,0,0,rect.bottom,800,800,SWP_NOZORDER);
		}
		break;
	case WM_TIMER:
		if(animate){
			InvalidateRect(hwnd,0,FALSE);
			current_frame=(current_frame+1)%64;
			if(current_frame>=frame_count)
				current_frame=0;
		}
		break;
	case WM_SIZE:
		{
			RECT rect={0};
			int dw=0,dh=0;
			GetWindowRect(hwnd,&rect);
			get_delta_rect(&orig_rect,&rect,&dw,&dh);
			move_ctrls(hwnd,dw,dh);
			InvalidateRect(hwnd,NULL,TRUE);
		}
		break;
	case WM_SIZING:
		{
			RECT *rect=(RECT*)lparam;
			int w,h,nw,nh;
			if(0==rect)
				break;
			w=orig_rect.right-orig_rect.left;
			h=orig_rect.bottom-orig_rect.top;
			nw=rect->right-rect->left;
			nh=rect->bottom-rect->top;
			switch(wparam){
			case WMSZ_BOTTOM:
				if(nh<h)
					rect->bottom=rect->top+h;
				break;
			case WMSZ_BOTTOMLEFT:
				if(nh<h)
					rect->bottom=rect->top+h;
				if(nw<w)
					rect->left=rect->right-w;
				break;
			case WMSZ_BOTTOMRIGHT:
				if(nh<h)
					rect->bottom=rect->top+h;
				if(nw<w)
					rect->right=rect->left+w;
				break;
			case WMSZ_LEFT:
				if(nw<w)
					rect->left=rect->right-w;
				break;
			case WMSZ_RIGHT:
				if(nw<w)
					rect->right=rect->left+w;
				break;
			case WMSZ_TOP:
				if(nh<h)
					rect->top=rect->bottom-h;
				break;
			case WMSZ_TOPLEFT:
				if(nh<h)
					rect->top=rect->bottom-h;
				if(nw<w)
					rect->left=rect->right-w;
				break;
			case WMSZ_TOPRIGHT:
				if(nh<h)
					rect->top=rect->bottom-h;
				if(nw<w)
					rect->right=rect->left+w;
				break;

			}
		}
		break;
	case WM_PAINT:
		{
			if(show_vga_pal){
				display_vga_pal(hwnd,mx,my);
			}
			else{
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
					unsigned char *tmp=anim_palette+(768*current_frame)+(frame[i]*3);
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
				//SetDIBitsToDevice(hdc,0,Y_OFFSET,W,H,0,0,0,H,buffer,&bmi,DIB_RGB_COLORS);
				{
					RECT rect;
					int dw=0,dh=0;
					GetWindowRect(hwnd,&rect);
					get_delta_rect(&orig_rect,&rect,&dw,&dh);
					StretchDIBits(hdc,0,Y_OFFSET,W+dw,H+dh,0,0,W,H,buffer,&bmi,DIB_RGB_COLORS,SRCCOPY);

				}

				EndPaint(hwnd,&ps);
				if(animate){
					char str[80];
					sprintf(str,"frame=%i delay=%i ms",current_frame,frame_delay);
					SetDlgItemText(hwnd,IDC_TEXT,str);
				}
			}
		}
		break;
	case WM_HSCROLL:
		{
			int scroll;
			scroll=SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_GETPOS,0,0);
			if(animate){
				frame_delay=scroll;
				if(frame_delay<10)
					frame_delay=10;
				else if(frame_delay>max_delay)
					frame_delay=max_delay;
				if(timer)
					KillTimer(hwnd,timer);
				timer=SetTimer(hwnd,1337,frame_delay,NULL);
			}
			else{
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
		}
		break;
	case WM_LBUTTONDBLCLK:
		if(IsZoomed(hwnd))
			ShowWindow(hwnd,SW_RESTORE);
		else
			ShowWindow(hwnd,SW_MAXIMIZE);
		break;
	case WM_MOUSEMOVE:
		{
			mx=LOWORD(lparam);
			my=HIWORD(lparam);
			if(show_vga_pal){
				InvalidateRect(hwnd,0,0);
			}else if((!animate) && mx<=W && my>=Y_OFFSET && my<=(Y_OFFSET+H)){
				char str[80];
				sprintf(str,"x=%03i y=%03i",mx,my-Y_OFFSET);
				SetDlgItemText(hwnd,IDC_TEXT,str);
			}
			break;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wparam)){
		case IDC_VGA:
			CheckDlgButton(hwnd,IDC_ANIMATE,BST_UNCHECKED);
			PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(IDC_ANIMATE,BN_UNPUSHED),0);
			show_vga_pal=FALSE;
			if(IsDlgButtonChecked(hwnd,IDC_VGA))
				show_vga_pal=TRUE;
			InvalidateRect(hwnd,0,TRUE);
			break;
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
			PostMessage(GetDlgItem(hwnd,IDC_ANIMATE),BM_CLICK,0,0);
			//EndDialog(hwnd,0);
			break;
		case IDCANCEL:
			EndDialog(hwnd,0);
			break;
		}
		break;
	}
	return 0;
}
#endif // USEWX

#ifdef USEWX
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/rawbmp.h>

#define BUTTON_QUIT 1
#define BUTTON_ANIMATE 2
#define TIMER_ANI 1000

class MyApp : public wxApp
{
    DECLARE_EVENT_TABLE()
public:
	int current_frame;
	int frame_delay;
	int animate;
	int max_delay;

	wxDialog *dialog;
	wxButton *button_animate;
	wxButton *button_quit;
	wxSlider *slider;
	wxStaticText *info;
	wxBitmap *bitmap;
	wxImage *image;
	wxPaintDC *dc;
    wxBoxSizer* sizer1;
    wxTimer *timer;
    virtual bool OnInit();
    void OnClose(wxCloseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSlide(wxScrollEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAnimate(wxCommandEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);

};
IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE( MyApp, wxApp )
    EVT_PAINT( MyApp::OnPaint )
    EVT_CLOSE(MyApp::OnClose)
    EVT_SCROLL_CHANGED(MyApp::OnSlide)
    EVT_SCROLL_THUMBTRACK(MyApp::OnSlide)
    EVT_BUTTON(BUTTON_QUIT,MyApp::OnQuit)
    EVT_BUTTON(BUTTON_ANIMATE,MyApp::OnAnimate)
    EVT_KEY_DOWN(MyApp::OnKey)
    EVT_TIMER(TIMER_ANI,MyApp::OnTimer)
END_EVENT_TABLE()

int fill_bitmap(wxBitmap *bitmap,unsigned char *buffer)
{
    int x,y;
    int w,h;
    w=bitmap->GetWidth();
    h=bitmap->GetHeight();

   wxAlphaPixelData bmdata(*bitmap);
   bmdata.UseAlpha();
   wxAlphaPixelData::Iterator dst(bmdata);

   for(int y=0; y<h; y++) {
      dst.MoveTo(bmdata, 0, y);
      for(int x=0; x<w; x++) {
         // wxBitmap contains rgb values pre-multiplied with alpha
         unsigned char a=0xFF;
         unsigned char r,g,b;
         r=buffer[x*3+(y*3*W)];
         g=buffer[x*3+(y*3*W)+1];
         b=buffer[x*3+(y*3*W)+2];
         dst.Red()=r;
         dst.Green()=g;
         dst.Blue()=b;
         dst.Alpha()=a;
         dst++;
      }
   }
   return TRUE;
}
void MyApp::OnTimer(wxTimerEvent& event)
{
    if(animate){
        char str[80];
        current_frame=(current_frame+1)%64;
        if(current_frame>=frame_count)
			current_frame=0;
        sprintf(str,"frame=%i delay=%i ms",current_frame,frame_delay);
        info->SetLabel(str);
        dialog->Refresh(0);
    }
}
void MyApp::OnKey(wxKeyEvent& event)
{
    if(event.GetKeyCode()==WXK_ESCAPE)
        exit(0);
}
void MyApp::OnAnimate(wxCommandEvent& event)
{
    animate=!animate;
	if(animate){
		if(timer)
			timer->Start(frame_delay);
		slider->SetRange(0,max_delay);
		slider->SetValue(frame_delay);
	}
	else{
		if(timer)
			timer->Stop();
		slider->SetRange(0,63);
		slider->SetValue(current_frame);
	}
}
void MyApp::OnQuit(wxCommandEvent& event)
{
    exit(0);
}
void MyApp::OnSlide(wxScrollEvent& event)
{
    int scroll=event.GetPosition();
    if(animate){
        frame_delay=scroll;
        if(frame_delay<10)
            frame_delay=10;
        else if(frame_delay>max_delay)
            frame_delay=max_delay;
        timer->Start(frame_delay);
        printf("delay=%i\n",frame_delay);
    }
    else{
        static int last_scroll=0;
        current_frame=scroll;
        if(current_frame<0)
            current_frame=0;
        else if(current_frame>63)
            current_frame=63;
        dialog->Refresh(0);
        last_scroll=scroll;
        {
            char str[80];
            sprintf(str,"frame=%i",current_frame);
            info->SetLabel(str);
        }
    }
}
void MyApp::OnPaint(wxPaintEvent& event)
{
   wxPaintDC dc(dialog);
   if(bitmap){
       if(bitmap->Ok())
       {
           {
			unsigned char *frame,buffer[W*H*3];
			int i;
			frame=buffers+(current_frame*W*H);
			for(i=0;i<W*H;i++){
				unsigned char r,g,b;
				unsigned char *tmp=P[frame[i]];
				r=tmp[0];
				g=tmp[1];
				b=tmp[2];
				buffer[i*3]=b;
				buffer[i*3+1]=g;
				buffer[i*3+2]=r;
			}
            fill_bitmap(bitmap,buffer);
           }
          dc.DrawBitmap(*bitmap, 0, 30);
       }
   }
   event.Skip();
}
void MyApp::OnClose(wxCloseEvent& event)
{
    exit(0);
}
bool MyApp::OnInit()
{
    dialog = new wxDialog((wxFrame *)NULL, wxID_ANY, _T("#asm gif animation tool"),
                                 wxDefaultPosition, wxSize(500, 400),
                                 wxDEFAULT_FRAME_STYLE);

    sizer1 = new wxBoxSizer(wxBOTH);

	button_animate= new wxButton(dialog,BUTTON_ANIMATE,"Animate");
	button_quit=new wxButton(dialog,BUTTON_QUIT,"quit");
	slider = new wxSlider(dialog,wxID_ANY,0,0,63);
	info = new wxStaticText(dialog,wxID_ANY,wxT("info"));
	timer = new wxTimer(dialog,TIMER_ANI);
	bitmap=new wxBitmap(320, 240, 32);

    current_frame=0;
	animate=FALSE;
	frame_delay=66;
	max_delay=150;

    slider->SetValue(current_frame);



	sizer1->Add(slider);
	sizer1->Add(button_animate);
	sizer1->Add(button_quit);
	sizer1->Add(info);

    dialog->SetSizer(sizer1);
    dialog->Layout();

    dialog->Show(true);

    SetTopWindow(dialog);
    return true;
}


#endif // USEWX
