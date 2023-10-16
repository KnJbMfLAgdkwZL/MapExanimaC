// MapExanimaC.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "MapExanimaC.h"
#include <thread>
#include "printDebug.h"
#include <locale>
#include "LoadPNG.h"
#include <windowsx.h>
#include <d2d1.h>
#include <filesystem>
#include <math.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

#define MAX_LOADSTRING 100

// Globale Variablen:
int monitor_width;
int monitor_height;

MONITORINFO info;

HINSTANCE hInst;                                // Aktuelle Instanz
WCHAR szTitle[MAX_LOADSTRING];                  // Titelleistentext
WCHAR szWindowClass[MAX_LOADSTRING];           // Der Klassenname des Hauptfensters.
HWND hWnd;                                     // Der Handle des Hauptfensters

int winSizeWidth=300, winSizeHeight=300;
int windowPos[] = { 0,0 };
// Globale APP Exanima Variable
BOOL b_readMemory = TRUE;
DWORD mapState = CORNERMAP;
BYTE mapLVL = 0;
float x_pos, y_pos;
float Map_rec[] = { 0, 0 };
float PosFig_rec[] = { 140, 100 };
int tmpmpos[] = { 0, 0 };
float map_size[] = { 300, 300 };
float x_maxSize = 300.f;
float scale = 1.0f;
int rotateAngel = 0;

//Globale Variable für Direct2D

ID2D1Bitmap* bmp_Map;
ID2D1Bitmap* bmp_PosFig;
IWICImagingFactory* wicFactory = NULL;
ID2D1Factory* D2DFactory = NULL;

ID2D1HwndRenderTarget* renderTarget = NULL;

HWND mainWindowH;


// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                ReadMemoryOfExanima();
thread*             ptr_t_ReadMemoryOfExanima;
void                HideWindowBorders(HWND);
ID2D1Bitmap*        lbmpfromFile(const wchar_t*);
void                UpdateWindowProp();

LRESULT CALLBACK keyboard_hook(const int ,const WPARAM ,const LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //CREATE DIRECT2D Factory
    HRESULT CoInitializeExHR = CoInitializeEx(NULL, 0);

    HRESULT CoCreateInstanceHR = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );

    HRESULT D2D1CreateFactoryHR = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);
    
   

    // Globale Zeichenfolgen initialisieren
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPEXANIMAC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);   



    // Anwendungsinitialisierung ausführen:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // TODO: Hier Code einfügen.    
    thread t_ReadMemoryOfExanima(ReadMemoryOfExanima);
    ptr_t_ReadMemoryOfExanima = &t_ReadMemoryOfExanima;

    //SetHOOK
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, hInstance, 0);
    wchar_t buf[256];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
    
    
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPEXANIMAC));
   
    MSG msg;
    
    
    // Hauptnachrichtenschleife:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
;   ptr_t_ReadMemoryOfExanima->join();
    UnhookWindowsHookEx(hHook);
    return (int) msg.wParam;
}



//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_MAPEXANIMAC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL));
    
   

    return RegisterClassExW(&wcex);
}

//
//   FUNKTION: InitInstance(HINSTANCE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTARE:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

   hWnd = CreateWindowExW(WS_EX_TOPMOST,
       szWindowClass,
       szTitle,
       WS_POPUP,
       CW_USEDEFAULT, CW_USEDEFAULT,
       winSizeWidth, winSizeHeight,
       NULL,
       NULL,
       hInstance,
       NULL
   );
   
   
   if (!hWnd)
   {
      return FALSE;
   }
   mainWindowH = hWnd;

   // Create a D2D render target properties
   D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
   D2D1_SIZE_U size = D2D1::SizeU(winSizeWidth, winSizeHeight);
   D2DFactory->CreateHwndRenderTarget(
       renderTargetProperties,
       D2D1::HwndRenderTargetProperties(hWnd, size),
       &renderTarget
   );
   

   //lade die Map und die posfigure
   bmp_Map = lbmpfromFile(getMap(mapLVL));
   //if (bmp_Map) {
   //    map_size[0] = bmp_Map->GetPixelSize().width;
   //    map_size[1] = bmp_Map->GetPixelSize().height;
   //}
   bmp_PosFig = lbmpfromFile(L"assets\\PosFig.png");

   
  

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SetWindowPos(hWnd, HWND_TOPMOST, 0 , 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  
   return TRUE;
}

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK: Verarbeitet Meldungen für das Hauptfenster.
//
//  WM_PAINT    - Darstellen des Hauptfensters
//  WM_DESTROY  - Ausgeben einer Beendenmeldung und zurückkehren
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
       
    //OutputIntToHex(message);
    switch (message)
    {
        /* UPDATE UI FROM THE INFORMATION INPUT OF THE GAME EXANIMA*/
    case IDM_MY_MSG_UPDATE_UI:
        if (mapLVL > 1) {
            if (mapState & FULLTRANSMAP) {
                Map_rec[0] = (x_pos * scale + monitor_width/2);
                Map_rec[1] = (y_pos * scale + monitor_height/2);
                PosFig_rec[0] = monitor_width / 2 - 15;
                PosFig_rec[1] = monitor_height / 2 - 40;
            }
            
            else if(mapState & CORNERMAP) {
                Map_rec[0] = (x_pos * scale + 145);
                Map_rec[1] = (y_pos * scale + 140);
            }            
                
            else if (mapState & FULLMAP) {
                if (scale > 1) {
                    PosFig_rec[0] = ((-x_pos - 5) * scale + Map_rec[0]) + 4 * scale;
                    PosFig_rec[1] = ((-y_pos - 40) * scale + Map_rec[1]) + 19 * scale;
                }
                else {
                    PosFig_rec[0] = ((-x_pos - 5) * scale + Map_rec[0]) - 2 / scale;
                    PosFig_rec[1] = ((-y_pos - 40) * scale + Map_rec[1]) - 7 / scale;
                }
            }
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_CREATE:
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, 90, LWA_ALPHA);
        
        break;
    case WM_LBUTTONDOWN:
        //for dragging not only by the title, but also by any part of the window 
        tmpmpos[0] = GET_X_LPARAM(lParam);
        tmpmpos[1] = GET_Y_LPARAM(lParam);
        if (mapState & (CORNERMAP | HIDEMAP)) {            
            ReleaseCapture();
            SendMessage(hWnd, 0xA1, 2, 0);
        }
        else {


        }
        break;
    case WM_MOUSEMOVE:
        if ((mapState & FULLMAP) && wParam == 0x0001) {
            
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            xPos = xPos - tmpmpos[0];
            yPos = yPos - tmpmpos[1];
            //if ((-map_size[0]+1500) < (xPos + Map_rec[0]) && (xPos + Map_rec[0]) < 1500)
                Map_rec[0] = xPos + Map_rec[0];
            //if ((-map_size[1]+1500) < (yPos + Map_rec[1]) && (yPos + Map_rec[1]) < 1500)
                Map_rec[1] = yPos + Map_rec[1];            
            tmpmpos[0] = GET_X_LPARAM(lParam);
            tmpmpos[1] = GET_Y_LPARAM(lParam);           
        }
        break;
    case WM_LBUTTONDBLCLK:       
        if (mapState&CORNERMAP) {
            RECT rec;
            GetWindowRect(hWnd, &rec);
            windowPos[0] = rec.left;
            windowPos[1] = rec.top;
           
        }
        mapState = mapState ^ CORNERMAP;
        mapState = mapState ^ FULLMAP;
        UpdateWindowProp();
        break;
    case WM_RBUTTONUP:
        if (mapState&CORNERMAP) 
            mapState = HIDEMAP;                    
        else 
            mapState = CORNERMAP;        
        UpdateWindowProp();        
        break;
    case WM_MOUSEWHEEL:
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ScreenToClient(hWnd, &pt);

        //rein ZOOMEN
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0 && map_size[0] <= x_maxSize+3000) {      
            scale *= 1.1f;
            map_size[0] *= 1.1f;
            map_size[1] *= 1.1f;
            Map_rec[0] = (int)(Map_rec[0] - ((-Map_rec[0] + pt.x) * 0.1));
            Map_rec[1] = (int)(Map_rec[1] - ((-Map_rec[1] + pt.y) * 0.1));
        }            
        else {
            if (GET_WHEEL_DELTA_WPARAM(wParam) < 0 && 600 < map_size[0]) {
                scale /= 1.1f;
                map_size[0] /= 1.1f;
                map_size[1] /= 1.1f;
                Map_rec[0] = (int)(Map_rec[0] + ((-Map_rec[0] + pt.x)/11 ));
                Map_rec[1] = (int)(Map_rec[1] + ((-Map_rec[1] + pt.y)/11 ));
            }                
        } 
        break;
    
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(hWnd, &ps);
        // Draws an image and scales it to the current window size
        if (renderTarget) {
            renderTarget->BeginDraw();
            renderTarget->Clear();

            
            D2D1_POINT_2F imageCenter = D2D1::Point2F(
                PosFig_rec[0]+10,
                PosFig_rec[1]+45
            );
            if (mapState& (CORNERMAP|FULLTRANSMAP))
            renderTarget->SetTransform(
                D2D1::Matrix3x2F::Rotation(-rotateAngel, imageCenter)
            );
            

            if(bmp_Map)    renderTarget->DrawBitmap(bmp_Map, D2D1::RectF(Map_rec[0], Map_rec[1], Map_rec[0]+map_size[0], Map_rec[1]+map_size[1]));
            renderTarget->SetTransform(
                D2D1::Matrix3x2F::Rotation(0, imageCenter)
            );
            if(bmp_PosFig) renderTarget->DrawBitmap(bmp_PosFig, D2D1::RectF(PosFig_rec[0], PosFig_rec[1], PosFig_rec[0] + 20, PosFig_rec[1] + 51));
            
            renderTarget->EndDraw();
        }
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;   
    case WM_ERASEBKGND:
        return 1;
    case WM_CHAR:
        if (wParam == VK_ESCAPE) {
            b_readMemory = FALSE;
            DestroyWindow(hWnd);
       }
        
        break;         
     default:        
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ReadMemoryOfExanima() {
   
    debug_w("Thread started!");
    HWND hWindow = FindWindowEx(0,0, L"Exanima",0);
    DWORD processID;
    
    if (hWindow)
    {


        GetWindowThreadProcessId(hWindow, &processID);
        HANDLE hProcHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
                
        //change fullscreen on resolution value
        HMONITOR monitor = MonitorFromWindow(hWindow, MONITOR_DEFAULTTONEAREST);
        
        info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(monitor, &info);

        monitor_width = info.rcMonitor.right - info.rcMonitor.left;
        monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

        if (isToFullscreen()) {
            HideWindowBorders(hWindow);            
            SetWindowPos(hWindow, HWND_NOTOPMOST, info.rcMonitor.left, 0, monitor_width, monitor_height, SWP_NOSENDCHANGING);
            SetWindowPos(mainWindowH, HWND_TOPMOST, info.rcMonitor.left, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            SendMessage(hWindow, WM_EXITSIZEMOVE, 0, 0);
        }        

        char chBufferMapLVL = 0;

        while (b_readMemory) {
            this_thread::sleep_for(chrono::milliseconds(20));
                       
            if (ReadProcessMemory(hProcHandle, ADDR_MAP_LVL, &chBufferMapLVL, sizeof(chBufferMapLVL), 0) == 0)                 
                exit(0);
           
            if (chBufferMapLVL > 1)
            {
               
                if (chBufferMapLVL != mapLVL) {
                    //CHANGE LVL
                    mapLVL = chBufferMapLVL;  
                    bmp_Map = lbmpfromFile(getMap(mapLVL));
                    map_size[0] = bmp_Map->GetPixelSize().width * scale;
                    map_size[1] = bmp_Map->GetPixelSize().height * scale;
                    x_maxSize = map_size[0];
                    //scale = 1.f;
                }

                float flBufferX = 0.f;
                float flBufferY = 0.f;



                ReadProcessMemory(hProcHandle, ADDR_X_POS, &flBufferX, sizeof(flBufferX), 0);
                ReadProcessMemory(hProcHandle, ADDR_Y_POS, &flBufferY, sizeof(flBufferY), 0);


                x_pos = static_cast<float>(flBufferX);
                y_pos = static_cast<float>(flBufferY);
               
                x_pos = (x_pos + getMapOffsetX(mapLVL)) * -getMapScale(mapLVL);
                y_pos = (y_pos + getMapOffsetY(mapLVL)) * -getMapScale(mapLVL);
                

                /* ROTATE THE MAP ON CAMERA ROTATION 
                             N  E  W  S
                   rotatey-> 1  0  0 -1
                   rotatex-> 0  1 -1  0
                   values in exanima
                */
                ReadProcessMemory(hProcHandle, ADDR_X_ROTATE, &flBufferX, sizeof(flBufferX), 0);
                ReadProcessMemory(hProcHandle, ADDR_Y_ROTATE, &flBufferY, sizeof(flBufferY), 0);

                rotateAngel = atan2(flBufferX, flBufferY) * 180 / M_PI;
                if (rotateAngel < 0) rotateAngel += 360;

                

                PostMessage(mainWindowH, IDM_MY_MSG_UPDATE_UI, 0, 0);
            }
            else {
                PostMessage(mainWindowH, IDM_MY_MSG_UPDATE_UI, 0, 0);
            }
           
            
        }        
    }
    else {      
        
        MessageBoxW(NULL, L"Pls start Exanima [in window-mode]", L"Exanima not running!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        exit(0);
        debug_w("Exanima not started");
    }  
}

void HideWindowBorders(HWND hWnd)
{
    int style = GetWindowLong(hWnd, GWL_STYLE); //gets current style
    SetWindowLong(hWnd, GWL_STYLE, (style & ~(WS_CAPTION | WS_SIZEBOX))); //removes caption and the sizebox from current style
}

ID2D1Bitmap* lbmpfromFile(const wchar_t* file) {
    ID2D1Bitmap* bmp = NULL;
    IWICBitmapDecoder* wicDecoder = NULL;    
    IWICFormatConverter* wicConverter = NULL;
    IWICBitmapFrameDecode* wicFrame = NULL;

    wicFactory->CreateDecoderFromFilename(file, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
   
    wicDecoder->GetFrame(0, &wicFrame);
    
    wicFactory->CreateFormatConverter(&wicConverter);
    wicConverter->Initialize(
        wicFrame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.0,
        WICBitmapPaletteTypeCustom);
    
    if(renderTarget)
    renderTarget->CreateBitmapFromWicBitmap(wicConverter,NULL,&bmp);

    if(wicDecoder) wicDecoder->Release();
    if(wicConverter) wicConverter->Release();
    if(wicFrame) wicFrame->Release();

    return bmp;
}

void UpdateWindowProp() {
    debug_w(mapState);
    LONG cur_style = GetWindowLong(hWnd, GWL_EXSTYLE);
    if (mapState&FULLTRANSMAP) {
        winSizeHeight = monitor_height, winSizeWidth = monitor_width;
        debug_w(monitor_width);
        SetWindowPos(mainWindowH, NULL, info.rcMonitor.left, 0, winSizeWidth, winSizeHeight, SWP_NOZORDER | SWP_NOACTIVATE);

        
        SetWindowLong(hWnd, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetLayeredWindowAttributes(hWnd, 0, 40, LWA_ALPHA);
    }
    else {
        SetWindowLong(hWnd, GWL_EXSTYLE, (GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED) & ~WS_EX_TRANSPARENT);
        SetLayeredWindowAttributes(hWnd, 0, 150, LWA_ALPHA);
        if (mapState&HIDEMAP) {
            winSizeHeight = 25, winSizeWidth = 300;
            SetWindowPos(mainWindowH, NULL, windowPos[0], windowPos[1], winSizeWidth, winSizeHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            Map_rec[0] = 0;
            Map_rec[1] = 0;
        }
        else
            if (mapState&CORNERMAP) {
                winSizeHeight = 300, winSizeWidth = 300;
                SetWindowPos(mainWindowH, NULL, windowPos[0], windowPos[1], winSizeWidth, winSizeHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                Map_rec[0] = 0;
                Map_rec[1] = 0;
                PosFig_rec[0] = 140;
                PosFig_rec[1] = 100;
            }
            else {
                winSizeHeight = monitor_height, winSizeWidth = monitor_width;
                SetWindowPos(mainWindowH, NULL, 0, 0, winSizeWidth, winSizeHeight, SWP_NOZORDER | SWP_NOACTIVATE);

                Map_rec[0] = (x_pos - windowPos[0] + winSizeWidth / 2) * scale;
                Map_rec[1] = (y_pos - windowPos[1] + winSizeHeight / 2) * scale;
            }
    }

    
    renderTarget->Resize(D2D1::SizeU(winSizeWidth, winSizeHeight));
}

LRESULT CALLBACK keyboard_hook(int code, WPARAM wParam, LPARAM lParam) {
    
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* s = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (GetPrivateProfileInt(
            L"AppSettings",
            L"quickSave",
            NULL,
            L".\\assets\\config.ini")) {

            if (s->vkCode == VK_F5) {
                int result = MessageBox(NULL, L"Quick Backup? (Pls go into the Menu or pause Ingame!) ", L"Confirm", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);

                if (result == IDYES) {
                    string directoryExanima;
                    if (getPathExanima().length() > 1)
                        directoryExanima = getPathExanima();
                    else {
                        char* pValue;
                        size_t len;
                        _dupenv_s(&pValue, &len, "APPDATA");
                        if (pValue) {
                            directoryExanima = pValue;
                            directoryExanima += +"\\Exanima";
                        }
                    }
                    string directoryExanimaBackup = directoryExanima + "\\backUP";
                    std::filesystem::create_directories(directoryExanimaBackup);
                    for (const auto& entry : filesystem::directory_iterator(directoryExanima)) {
                        string fileString = entry.path().string();
                        if (fileString.find(".rsg") < fileString.length()) {
                            string filebackup = directoryExanimaBackup + "\\" + entry.path().filename().string();
                            debug_w(filebackup);
                            filesystem::copy_file(fileString, filebackup,
                                filesystem::copy_options::overwrite_existing);
                        }
                    }
                    MessageBoxW(NULL, L"Backup is created. Please remember to Backup in the Menu or pause Ingame.", L"Exanima Backup!", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
                }
                
            }
            if (s->vkCode == VK_F6) {
                string directoryExanima;
                if (getPathExanima().length() < 2) {
                    char* pValue;
                    size_t len;
                    _dupenv_s(&pValue, &len, "APPDATA");
                    if (pValue) {
                        directoryExanima = pValue;
                        directoryExanima += +"\\Exanima";
                    }
                }
                else
                    directoryExanima = getPathExanima();
                string directoryExanimaBackup = directoryExanima + "\\backUP";
                for (const auto& entry : filesystem::directory_iterator(directoryExanimaBackup)) {
                    string fileStringBackup = entry.path().string();
                    if (fileStringBackup.find(".rsg") < fileStringBackup.length()) {
                        string fileString = directoryExanima + "\\" + entry.path().filename().string();
                        debug_w(fileString);
                        filesystem::copy_file(fileStringBackup, fileString,
                            filesystem::copy_options::overwrite_existing);
                    }
                }
                MessageBoxW(NULL, L"Backup is loaded.", L"Exanima Backup!", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            }
        }
        if (s->vkCode == 0x4D)
        {
            mapState ^= FULLTRANSMAP;

            UpdateWindowProp();

        }    
    } 
    return CallNextHookEx(0, code, wParam, lParam);
}
