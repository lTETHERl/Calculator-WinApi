#include <windows.h>
#include <vector>
#include <string>
#include <sstream>

//Глобальные переменные
HWND hMainWindow = nullptr;
HWND hStatic = nullptr;
std::vector<std::vector<HWND>> buttons(4, std::vector<HWND>(4, nullptr));

//Кисточка
HBRUSH hStaticBackgroundBrush = nullptr;

//Параметры кнопок
const int buttonWeight = 60;
const int buttunHeight = 80;

//Символы для кнопок
std::vector<std::vector<std::wstring>> buttonsSymbols = {
    {L"7", L"8", L"9", L"x"},
    {L"4", L"5", L"6", L"-"},
    {L"1", L"2", L"3", L"+"},
    {L"C", L"0", L"/", L"="}
};

//Переменная для текста и для последующего подсчёта.
std::wstring text = L"0";

//Функция счёта
double calculate(const std::wstring& str) {
    double result = 0;
    double temp = 0;
    wchar_t sign = L'+';
    std::wstringstream ss(str);

    while (ss >> temp) {
        switch (sign) {
        case L'+':
            result += temp;
            break;

        case L'-':
            result -= temp;
            break;

        case L'x':
            result *= temp;
            break;

        case L'/':
            result /= temp;
            break;
        }
        ss >> sign;
    }

    return result;
}


// Функция обработки сообщений встроенного окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Обработка сообщений
    switch (uMsg) {
    case WM_COMMAND:

        //Проверка нажатия кнопок
        for (int i = 0; i < buttons.size(); ++i) {
            for (int j = 0; j < buttons.size(); ++j) {
                if (LOWORD(wParam) == BN_CLICKED && (HWND)lParam == buttons[i][j]) {
                    if (buttonsSymbols[i][j] == L"=") {
                        std::wstring result = std::to_wstring(calculate(text));
                        SetWindowText(hStatic, result.c_str());
                        text = result;
                    }

                    else if (buttonsSymbols[i][j] == L"C") {
                        text = L"0";
                        SetWindowText(hStatic, text.c_str());
                    }

                    else {
                        if (text == L"0") {
                            text = buttonsSymbols[i][j];
                        }

                        else {
                            text += buttonsSymbols[i][j];
                        }

                        SetWindowText(hStatic, text.c_str());
                        break;
                    }
                }
            }
        }
        break;

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdcStatic, RGB(255, 255, 255)); // Установите цвет текста
        SetBkMode(hdcStatic, TRANSPARENT); // Сделайте фон текста прозрачным

        if (hStaticBackgroundBrush == nullptr) {
            hStaticBackgroundBrush = CreateSolidBrush(RGB(3, 173, 252)); // Создайте фоновую кисть
        }

        return reinterpret_cast<LRESULT>(hStaticBackgroundBrush);
    }
    break;

    case WM_GETMINMAXINFO: // Предотвращение изменения размера
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 300; // Минимальная ширина
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500; // Минимальная высота
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 300; // Минимальная ширина
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 500; // Минимальная высота
        return 0;


    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Создание встроенного окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"Калькулятор";
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);
    hMainWindow = CreateWindowEx(
        0, L"Калькулятор", L"Калькулятор",
        WS_OVERLAPPEDWINDOW | WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 350, 550,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );

    //Создание статика для текста
    hStatic = CreateWindowEx(
        0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
        10, 20, 260, 50, hMainWindow, nullptr, hInstance, nullptr
    );

    //Создание шрифта
    HFONT hFont = CreateFont(40, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    if (hFont) {
        SendMessage(hStatic, WM_SETFONT, WPARAM(hFont), TRUE);
    }


    //Создание кнопок
    for (int i = 0; i < buttons.size(); ++i) {
        for (int j = 0; j < buttons.size(); ++j) {
            buttons[i][j] = CreateWindowEx(
                0, L"BUTTON", buttonsSymbols[i][j].c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                buttonWeight*j+20, buttunHeight*i+100, buttonWeight, buttunHeight, hMainWindow, nullptr, hInstance, nullptr
            );
        }
    }

    SetWindowText(hStatic, text.c_str());

    

    ShowWindow(hMainWindow, nCmdShow);

    // Запуск цикла обработки сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
