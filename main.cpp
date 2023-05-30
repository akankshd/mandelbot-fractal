#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Create and register a window - the basic stuff
    char* szClass = "ZorroWindowClass";
    HINSTANCE hi = GetModuleHandle(NULL);
    UnregisterClass(szClass, hi);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hi;
    wcex.hIcon = LoadIcon(hi, (LPCSTR)128);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClass;
    wcex.hIconSm = LoadIcon(hi, (LPCSTR)128);
    RegisterClassEx(&wcex);
    HWND hwnd = CreateWindowEx(0, szClass, "Zorro Mandelbrot Test", 0x96cf0000, 0, 0, 640, 480, NULL, 0, NULL, NULL);

    // Now create a "Reset" and "Quit" menu
    HMENU menu = CreateMenu();
    HMENU hSubMenu = CreateMenu();
    InsertMenu(hSubMenu, 0, MF_BYPOSITION | MF_STRING, 1, "Reset");
    InsertMenu(hSubMenu, 2, MF_BYPOSITION | MF_STRING, 3, "Quit");
    InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "File");

    // Activate window, menu, and message loop
    if (hwnd)
    {
        SetMenu(hwnd, menu);
        ShowWindow(hwnd, SW_SHOW);
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

// Convert value to color
long JetColor(double v)
{
    double d = 25.0;
    v = v / d + 0.5;
    int f = (int)(255 * (v - i));
    int r = 0, g = 0, b = 0;

    switch ((int)v)
    {
    case 0:
        r = 0;
        g = 0;
        b = f;
        break;
    case 1:
        r = 0;
        g = f;
        b = 255;
        break;
    case 2:
        r = f;
        g = 255;
        b = 255 - f;
        break;
    case 3:
        r = 255;
        g = 255 - f;
        b = 0;
        break;
    case 4:
        r = 255 - f;
        g = 0;
        b = 0;
        break;
    }
    return r | (g << 8) | (b << 16);
}

// Draw the Mandelbrot fractal.
void Draw(HDC hdc, long vw, long vh)
{
    long w = (long)vw, h = (long)vh, i, j;
    long detail = 100;

    if (w > 0)
    {
        for (i = 0; i < w; i++)
        {
            for (j = 0; j < h; j++)
            {
                double x = m_x + ((double)i) * m_width / w;
                double y = m_y + ((double)(h - j)) * m_width / w;
                double zx = 0, zy = 0;
                int inset = 1, times = 0;
                while (inset && times < detail)
                {
                    times++;
                    double zxs = zx * zx;
                    double zys = zy * zy;
                    zy = 2 * zx * zy + y;
                    zx = zxs - zys + x;
                    if (zxs + zys >= 4.0)
                        inset = 0;
                }
                if (inset)
                    SetPixel(hdc, i, j, 0);
                else
                    SetPixel(hdc, i, j, JetColor(times));
            }
        }
    }
}

// Windows message loop
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Variables for the Mandelbrot fractal
    double m_x = 0.344142,
           m_y = 0.075094,
           m_width = 0.017813;

    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
    case WM_RBUTTONDOWN: // Reset to full fractal
        m_x = -2.5;
        m_y = -2;
        m_width = 4.0;
        InvalidateRect(hWnd, 0, 0);
        break;
    case WM_LBUTTONDOWN: { // Zoom in at cursor position
        RECT rect;
        GetClientRect(hWnd, &rect);
        long x = ((long)lParam) & 0xffff;
        long y = (((long)lParam) & 0xffff0000) >> 16;
        double zoom = 0.5;
        double a = x;
        a /= rect.right;
        m_x += m_width * (a - 0.5);
        a = y;
        a /= rect.bottom;
        m_y += m_width * (0.5 - a);
        m_width *= zoom;
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (wParam)
        {
        case 1:
            m_x = -2.5;
            m_y = -2;
            m_width = 4.0;
            InvalidateRect(hWnd, 0, 0);
            break;
        case 2:
            break;
        case 3:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        Draw(hdc, rect.right, rect.bottom);
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
        case VK_F12:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
