// MapExanimaC.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "MapExanimaC.h"
#include "string"
#include <thread>
#include "printDebug.h"
#include <locale>
#include "LoadPNG.h"
#include <gdiplus.h>
#include <windowsx.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;
using namespace Gdiplus;
#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE hInst;                                // Aktuelle Instanz
WCHAR szTitle[MAX_LOADSTRING];                  // Titelleistentext
WCHAR szWindowClass[MAX_LOADSTRING];           // Der Klassenname des Hauptfensters.
HWND hWnd;                                     // Der Handle des Hauptfensters
// Globale APP Exanima Variable
BOOL b_readMemory = TRUE;
BOOL isCornerMap = TRUE;
BYTE mapLVL = 0;
float x_pos, y_pos;
int Map_rec[] = { 0, 0 };
int tmpmpos[] = { 0, 0 };
int map_size[] = { 300, 300 };
int x_maxSize = 300;
float scale = 1.0f;
Image* Image_map;
Image* Image_posFig;


HWND mainWindowH;




// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                ReadMemoryOfExanima();
thread*             ptr_t_ReadMemoryOfExanima;
void                HideWindowBorders(HWND);

IStream* CreateStreamOnResource(LPCTSTR , LPCTSTR );




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //Startup GDIPLUS
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
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
    GdiplusShutdown(gdiplusToken);
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
       300, 300,
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
  
   //Image_map.FromFile(getMap(mapLVL));

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
    PAINTSTRUCT ps;
    HDC hdc;
   
    //OutputIntToHex(message);
    switch (message)
    {
        /* UPDATE UI FROM THE INFORMATION INPUT OF THE GAME EXANIMA*/
    case IDM_MY_MSG_UPDATE_UI:

        if (isCornerMap) {
            if (mapLVL > 1) {                
                Map_rec[0] = x_pos - 20;
                Map_rec[1] = y_pos - 51;
            }
        }
        else {
           
        }
        


        break;
    case WM_CREATE:
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, (255 * 80) / 100, LWA_ALPHA);

        Image_map = Bitmap::FromFile(getMap(mapLVL));
        Image_posFig = Bitmap::FromFile(L"assets\\posFig.png");

        break;
    case WM_LBUTTONDOWN:
        //for dragging not only by the title, but also by any part of the window 
        tmpmpos[0] = GET_X_LPARAM(lParam);
        tmpmpos[1] = GET_Y_LPARAM(lParam);
        if (isCornerMap) {
            ReleaseCapture();
            SendMessage(hWnd, 0xA1, 2, 0);
        }
        else {


        }
        break;
    case WM_MOUSEMOVE:
        if (!isCornerMap && wParam == 0x0001) {

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            xPos = xPos - tmpmpos[0];
            yPos = yPos - tmpmpos[1];
            if ((-map_size[0]+500) < (xPos + Map_rec[0]) && (xPos + Map_rec[0]) < 500)
                Map_rec[0] = xPos + Map_rec[0];
            if ((-map_size[1]+500) < (yPos + Map_rec[1]) && (yPos + Map_rec[1]) < 500)
                Map_rec[1] = yPos + Map_rec[1];            
            tmpmpos[0] = GET_X_LPARAM(lParam);
            tmpmpos[1] = GET_Y_LPARAM(lParam);   
        }
        break;
    case WM_LBUTTONDBLCLK:       
        isCornerMap = !isCornerMap;
        if (isCornerMap) {
            SetWindowPos(mainWindowH, NULL, 0, 0, 300, 300, SWP_NOZORDER | SWP_NOACTIVATE);            
            Map_rec[0] = 0;
            Map_rec[1] = 0;
        }
        else {
            SetWindowPos(mainWindowH, NULL, 400, 0, 900, 900, SWP_NOZORDER | SWP_NOACTIVATE);   
            Map_rec[0] = (int)(map_size[0] / -2);
            Map_rec[1] = (int)(map_size[1] / -2);
        }
        break;
    
    case WM_MOUSEWHEEL:
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ScreenToClient(hWnd, &pt);

        //rein ZOOMEN
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0 && map_size[0] < x_maxSize-100) {

            //(-Map_rec[0] + pt.x)

            map_size[0] *= 1.4;
            map_size[1] *= 1.4;
            Map_rec[0] = (int)(Map_rec[0] - ((-Map_rec[0] + pt.x) * 0.4));
            Map_rec[1] = (int)(Map_rec[1] - ((-Map_rec[1] + pt.y) * 0.4));

        }
            
        else {
            if (GET_WHEEL_DELTA_WPARAM(wParam) < 0 && 1000 < map_size[0]) {
                map_size[0] /= 1.4;
                map_size[1] /= 1.4;
                Map_rec[0] = (int)(Map_rec[0] + ((-Map_rec[0] + pt.x) * 0.286));
                Map_rec[1] = (int)(Map_rec[1] + ((-Map_rec[1] + pt.y) * 0.286));
            }
                
        }
        
        debug_w((float)GET_WHEEL_DELTA_WPARAM(wParam));
       

        //debug_w((float)pt.x-450);
        //debug_w((float)pt.y-450);
     
        break;
    
    case WM_PAINT:
    {
       hdc = BeginPaint(hWnd, &ps);


       HDC hdcMem = CreateCompatibleDC(hdc);
       HBITMAP hbmMem = CreateCompatibleBitmap(hdc, map_size[0], map_size[1]);
        SelectObject(hdcMem, hbmMem);

        Gdiplus::Graphics graphics(hdcMem);
        Rect destinationRect(0, 0, 900, 900);
        graphics.DrawImage(Image_map, destinationRect, Map_rec[0], Map_rec[1], map_size[0], map_size[1], UnitPixel);
        graphics.DrawImage(Image_posFig, (int)(Map_rec[0] + 150 - x_pos), (int)(Map_rec[1] + 150 - y_pos), 21, 50);

        BitBlt(hdc, 0, 0, map_size[0], map_size[1], hdcMem, 0, 0, SRCCOPY);

        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;   
    case WM_ERASEBKGND:
        return 1;
    case WM_RBUTTONUP:
       
        b_readMemory = FALSE;        
        DestroyWindow(hWnd);
        break;         
     default:        
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ReadMemoryOfExanima() {
   
    debug_w("Thread started!");
    HWND hWindow = FindWindow(NULL, L"Exanima");
    DWORD processID;
    
    if (hWindow)
    {
        GetWindowThreadProcessId(hWindow, &processID);
        HANDLE hProcHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);

        HideWindowBorders(hWindow);
        //TODO change fullscreen on resolution value
        SetWindowPos(hWindow, HWND_NOTOPMOST, 0, 0, 1920, 1080, SWP_NOSENDCHANGING);
        SendMessage(hWindow, WM_EXITSIZEMOVE, 0, 0);

       

        char chBufferMapLVL = 0;

        while (b_readMemory) {
            this_thread::sleep_for(chrono::milliseconds(20));
            
            
            ReadProcessMemory(hProcHandle, ADDR_MAP_LVL, &chBufferMapLVL, sizeof(chBufferMapLVL), 0);

            if (chBufferMapLVL > 1)
            {
               
                if (chBufferMapLVL != mapLVL) {
                    //CHANGE LVL
                    mapLVL = chBufferMapLVL;     
                }

                float flBufferX = 0.f;
                float flBufferY = 0.f;


                ReadProcessMemory(hProcHandle, ADDR_X_POS, &flBufferX, sizeof(flBufferX), 0);
                ReadProcessMemory(hProcHandle, ADDR_Y_POS, &flBufferY, sizeof(flBufferY), 0);


                x_pos = static_cast<float>(flBufferX);
                y_pos = static_cast<float>(flBufferY);
               
                x_pos = (x_pos + getMapOffsetX(mapLVL)) * -getMapScale(mapLVL);
                y_pos = (y_pos + getMapOffsetY(mapLVL)) * -getMapScale(mapLVL);
                


                //*msg = L"(" + to_wstring(x_pos) + L"/" + to_wstring(y_pos) + L" - MapID:[" + to_wstring(chBufferMapLVL) + L"]";
                PostMessage(mainWindowH, IDM_MY_MSG_UPDATE_UI, 0, 0);

            }
            else {
                PostMessage(mainWindowH, IDM_MY_MSG_UPDATE_UI, 0, 0);
            }
           
            
        }        
    }
    else {        
        float i = 0;
        mapLVL = 1;
        while (i < 10000 && b_readMemory) {

            if (true) {
                if (i == 30) {
                    mapLVL = 2;
                    Image_map = Image::FromFile(getMap(mapLVL));
                    map_size[0] = Image_map->GetWidth();                     
                    x_maxSize = map_size[0];
                    map_size[1] = Image_map->GetHeight();
                    debug_w((float)map_size[1]);
                }
           
                if (i == 900) {
                    mapLVL = 7;     
                    Image_map = Image::FromFile(getMap(mapLVL));
                    map_size[0] = Image_map->GetWidth();
                    x_maxSize = map_size[0];
                    map_size[1] = Image_map->GetHeight();
                }
                if (i == 3900) {
                    mapLVL = 4;   
                    Image_map = Image::FromFile(getMap(mapLVL));
                    map_size[0] = Image_map->GetWidth();
                    x_maxSize = map_size[0];
                    map_size[1] = Image_map->GetHeight();
                }
            }
            
            x_pos = -i*3;
            y_pos = -i*3;  
            PostMessage(mainWindowH, IDM_MY_MSG_UPDATE_UI, 0, 0);
            InvalidateRect(hWnd, NULL, FALSE);            
            this_thread::sleep_for(chrono::milliseconds(50));
           i++;
            
        }
        debug_w("Exanima not started");
    }  
}

void HideWindowBorders(HWND hWnd)
{
    int style = GetWindowLong(hWnd, GWL_STYLE); //gets current style
    SetWindowLong(hWnd, GWL_STYLE, (style & ~(WS_CAPTION | WS_SIZEBOX))); //removes caption and the sizebox from current style
}



