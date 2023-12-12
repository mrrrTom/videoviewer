// videoviewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <atlstr.h>
#include <vfw.h>
#include <assert.h>
#include <csignal>


using namespace std;


void ShowFrame(LPBYTE pDIB)
{
    assert(pDIB != nullptr);
    
    //get the BitmapInfoHeader
    BITMAPINFOHEADER bih;
    RtlMoveMemory(&bih.biSize, pDIB, sizeof(BITMAPINFOHEADER));

    //now get the bitmap bits
    if (bih.biSizeImage < 1)
    {
        return;
    }

    //BYTE* p = pDIB + sizeof(BITMAPINFOHEADER);
    //for (int i{ 0 }; i < bih.biSizeImage; ++p)
    //{
    //    if (p != nullptr) {
    //        byte val = *p;
    //        int oi{ 0 };
    //    }
    //}

    RGBQUAD* Bits = new RGBQUAD[bih.biSizeImage];
    RtlMoveMemory(Bits, pDIB + sizeof(BITMAPINFOHEADER), bih.biSizeImage);
    int x{ 0 };
    int y{ 0 };

    CONSOLE_SCREEN_BUFFER_INFOEX info;
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfoEx(hConsole, &info);
    info.ColorTable[0] = RGB(0, 0, 0);
    RGBQUAD* p = Bits;
    for (int i{ 0 }; i < bih.biSizeImage;i++ ) {
        
        try {
           
            RGBQUAD val = *p;
            p++;
            //if (val.rgbRed != '\0' || val.rgbGreen != '\0' || val.rgbBlue != '\0') {
            //    int oi{ 0 };
            //}

            COORD coord;
            coord.X = x;
            coord.Y = y;
            
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

            info.ColorTable[3] = RGB(val.rgbRed, val.rgbGreen, val.rgbBlue);
            info.ColorTable[15] = RGB(25, 25, 25);
            SetConsoleScreenBufferInfoEx(hConsole, &info);
            SetConsoleTextAttribute(hConsole, 3);
            
            if (bih.biWidth > x) 
            {
                cout << "*";
                x++;
            }
            else {
                x = 0;
                y++;
            }
        }
        catch(exception e){

        }
    }

    delete[] Bits;
}

int main()
{
    AVIFileInit();

    PAVIFILE avi;

    LPCWSTR str = L"D:\\Downloads\\ss.avi";

    int res = AVIFileOpen(&avi, str, OF_READ, NULL);


    if (res != AVIERR_OK)
    {
        //an error occures
        if (avi != NULL)
            AVIFileRelease(avi);

        return FALSE;
    }

    AVIFILEINFO avi_info;
    AVIFileInfo(avi, &avi_info, sizeof(AVIFILEINFO));

    //CString szFileInfo;
    /*szFileInfo.Format("Dimention: %dx%d\n"
        "Length: %d frames\n"
        "Max bytes per second: %d\n"
        "Samples per second: %d\n"
        "Streams: %d\n"
        "File Type: %d", avi_info.dwWidth,
        avi_info.dwHeight,
        avi_info.dwLength,
        avi_info.dwMaxBytesPerSec,
        (DWORD)(avi_info.dwRate / avi_info.dwScale),
        avi_info.dwStreams,
        avi_info.szFileType);*/

    //AfxMessageBox(szFileInfo, MB_ICONINFORMATION | MB_OK);

    PAVISTREAM pStream;
    res = AVIFileGetStream(avi, &pStream, streamtypeVIDEO /*video stream*/,
        0 /*first stream*/);

    if (res != AVIERR_OK)
    {
        if (pStream != NULL)
            AVIStreamRelease(pStream);

        AVIFileExit();
        return FALSE;
    }

    //do some task with the stream
    int iNumFrames;
    int iFirstFrame;

    iFirstFrame = AVIStreamStart(pStream);
    if (iFirstFrame == -1)
    {
        //Error getteing the frame inside the stream

        if (pStream != NULL)
            AVIStreamRelease(pStream);

        AVIFileExit();
        return FALSE;
    }

    iNumFrames = AVIStreamLength(pStream);
    if (iNumFrames == -1)
    {
        //Error getteing the number of frames inside the stream

        if (pStream != NULL)
            AVIStreamRelease(pStream);

        AVIFileExit();
        return FALSE;
    }

    ////getting bitmap from frame
    BITMAPINFOHEADER bih;
    ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
    bih.biPlanes = 1;
    bih.biXPelsPerMeter = 500;
    bih.biYPelsPerMeter = 300;


    //bih.biBitCount = 24;    //24 bit per pixel
    //bih.biClrImportant = 0;
    //bih.biClrUsed = 0;
    //bih.biCompression = BI_RGB;
    //bih.biPlanes = 1;
    //bih.biSize = 40;
    //bih.biXPelsPerMeter = 500;
    //bih.biYPelsPerMeter = 300;
    //calculate total size of RGBQUAD scanlines (DWORD aligned)
    //bih.biSizeImage = (((bih.biWidth * 3) + 3) & 0xFFFC) * bih.biHeight;
    //bih.biWidth = 400;
    //bih.biHeight = 100;

    PGETFRAME pFrame;
    pFrame = AVIStreamGetFrameOpen(pStream, &bih); /*(BITMAPINFOHEADER*) AVIGETFRAMEF_BESTDISPLAYFMT*/ /*&bih*/
    //RGBQUAD* buf = new RGBQUAD[30000];
   // HRESULT res123 = pFrame->SetFormat(NULL, NULL, 0, 0, 200, 100);

    //Get the first frame
    int index = 0;
    for (int i = iFirstFrame; i < iNumFrames; i++)
    {
        index = i - iFirstFrame;

        auto pDIB = (BYTE*)AVIStreamGetFrame(pFrame, index);

        if (pDIB != nullptr)
        {
            ShowFrame(pDIB);
        }
        //CreateFromPackedDIBPointer(pDIB, index);
    }

    AVIStreamGetFrameClose(pFrame);

    //close the stream after finishing the task
    if (pStream != NULL)
        AVIStreamRelease(pStream);

    AVIFileExit();
}
