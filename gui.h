#ifndef USEWX

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

int CALLBACK  dlg(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int current_frame=0;
	static int frame_delay=66;
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
			if(current_frame>=frame_count)
				current_frame=0;
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
			if(animate){
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
