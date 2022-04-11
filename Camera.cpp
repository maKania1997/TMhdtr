#include "INIReaderV2.h"
#include <Windows.h>
#include <iostream>

using namespace std;

void camera_window_position(int& camera_window_pos_x, int& camera_window_pos_y) {

    INIReader reader(".\\TMheadtrack_config_temporary.ini");

    int camera_window_pos = reader.GetInteger("main", "camera_window_pos", 200);

    switch (camera_window_pos) {
    case 1:
        cout << "Left upper" << "\n";
        camera_window_pos_x = 0;
        camera_window_pos_y = 0;
        break;
    case 2:
        cout << "Right upper" << "\n";
        camera_window_pos_x = GetSystemMetrics(0) - 180;
        camera_window_pos_y = 0;
        break;
    case 3:
        cout << "Left lower" << "\n";
        camera_window_pos_x = 0;
        camera_window_pos_y = GetSystemMetrics(1) - 250;
        break;
    case 4:
        cout << "Left lower" << "\n";
        camera_window_pos_x = GetSystemMetrics(0) - 180;
        camera_window_pos_y = GetSystemMetrics(1) - 250;
        break;
    }
}

void camera_InFront(LPCSTR window_name, int& camera_window_pos_x, int& camera_window_pos_y) {
    HWND handle;
    handle = FindWindowA(NULL, window_name);
    if (handle) {
        //cout << endl << "Setting up associated console window ON TOP !";
        SetWindowPos(
            handle, // window handle
            HWND_TOPMOST, // "handle to the window to precede
                          // the positioned window in the Z order
                          // OR one of the following:"
                          // HWND_BOTTOM or HWND_NOTOPMOST or HWND_TOP or HWND_TOPMOST
            camera_window_pos_x, camera_window_pos_y, // X, Y position of the window (in client coordinates)
            180, 250, // cx, cy => width & height of the window in pixels
            //SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW // The window sizing and positioning flags.
            SWP_DRAWFRAME | SWP_SHOWWINDOW // The window sizing and positioning flags.
        );
        // OPTIONAL ! - SET WINDOW'S "SHOW STATE"
        //ShowWindow(
        //    handle, // window handle
        //    /*SW_DENORMAL*/
        //    SW_NORMAL // how the window is to be shown
        //              // SW_NORMAL => "Activates and displays a window.
        //              // If the window is minimized or maximized,
        //              // the system restores it to its original size and position.
        //              // An application should specify this flag
        //              // when displaying the window for the first time."
        //);
        //cout << endl << "Done.";
    }
    else {
        cout << endl << "There is no console window associated with this app :(";
    }
}