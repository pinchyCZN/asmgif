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

#define USEWX
#ifdef USEWX
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/statline.h>

class GUIDialog : public wxDialog
{
    DECLARE_EVENT_TABLE()
    private:

        // Private event handlers
        void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
        void _wxFB_OnAbout( wxCommandEvent& event ){ OnAbout( event ); }
        void _wxFB_OnQuit( wxCommandEvent& event ){ OnQuit( event ); }
        void OnPaint( wxPaintEvent& event );


    protected:
        enum
        {
            idBtnAbout = 1000,
            idBtnQuit,
        };

        wxStaticText* m_staticText1;
        wxButton* BtnAbout;
        wxStaticLine* m_staticline1;
        wxButton* BtnQuit;

        // Virtual event handlers, overide them in your derived class
        virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
        virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
        virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }



    public:
        wxBitmap bmp;
        GUIDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("wxWidgets Application Template"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );

};


class wxtestApp : public wxApp
{
    public:
        virtual bool OnInit();
};

class wxtestDialog: public GUIDialog
{
    public:
        wxtestDialog(wxDialog *dlg);
        ~wxtestDialog();
    private:
        virtual void OnClose(wxCloseEvent& event);
        virtual void OnQuit(wxCommandEvent& event);
        virtual void OnAbout(wxCommandEvent& event);
        virtual void OnPaint(wxCommandEvent& event);
};

IMPLEMENT_APP(wxtestApp);


void GUIDialog::OnPaint(wxPaintEvent &event)
{


}

bool wxtestApp::OnInit()
{

    wxtestDialog* dlg = new wxtestDialog(0L);
    dlg->SetIcon(wxICON(aaaa)); // To Set App Icon
    dlg->Show();
    return true;
}



wxtestDialog::wxtestDialog(wxDialog *dlg)
    : GUIDialog(dlg)
{
}

wxtestDialog::~wxtestDialog()
{
}

void wxtestDialog::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void wxtestDialog::OnQuit(wxCommandEvent &event)
{
    Destroy();
}
void wxtestDialog::OnPaint(wxCommandEvent &event)
{
    int i;
    i=i;
}

void wxtestDialog::OnAbout(wxCommandEvent &event)
{
    wxString msg(wxVERSION_STRING);
    wxMessageBox(msg, _("Welcome to..."));
}

BEGIN_EVENT_TABLE( GUIDialog, wxDialog )
    EVT_CLOSE( GUIDialog::_wxFB_OnClose )
    EVT_BUTTON( idBtnAbout, GUIDialog::_wxFB_OnAbout )
    EVT_BUTTON( idBtnQuit, GUIDialog::_wxFB_OnQuit )
    EVT_PAINT(GUIDialog::OnPaint)
END_EVENT_TABLE()

GUIDialog::GUIDialog( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxHORIZONTAL );

    m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("frame"), wxDefaultPosition, wxDefaultSize, 0 );
//    m_staticText1->SetFont( wxFont( 20, 74, 90, 90, false, wxT("Arial") ) );

    bSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );

//    wxBoxSizer* bSizer2;
//    bSizer2 = new wxBoxSizer( wxVERTICAL );

    BtnAbout = new wxButton( this, idBtnAbout, wxT("&Animate"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer1->Add( BtnAbout, 0, wxALL, 5 );

    m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    bSizer1->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );

    BtnQuit = new wxButton( this, idBtnQuit, wxT("&Quit"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer1->Add( BtnQuit, 0, wxALL, 5 );

//    bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

    this->SetSizer( bSizer1 );
    this->Layout();
    bSizer1->Fit( this );
    BYTE *pFinal = (BYTE*) malloc(640*480 *3 );
    wxImage pWxImg = wxImage(640,480,pFinal, TRUE);

    bmp = wxBitmap(pWxImg.Scale(640,480));
//    dc.DrawBitmap(bmp, 0,0,false);
}


#endif // USEWX
