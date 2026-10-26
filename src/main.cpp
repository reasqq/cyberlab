#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <wrl.h>
#include <filesystem>
#include "WebView2.h"
#include "../labs/common.hpp"
#include "../labs/lab11.cpp"
#include "../labs/lab12.cpp"
#include "../labs/lab21.cpp"
#include "../labs/lab22.cpp"
#include "../labs/lab31.cpp"
#include "../labs/lab32.cpp"
#include "../labs/lab41.cpp"
#include "../labs/lab42.cpp"
#include "../labs/lab51.cpp"
#include "../labs/lab52.cpp"
#include "../labs/lab61.cpp"
#include "../labs/lab62.cpp"
#include "file_io.hpp"
#include "ui.hpp"

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;
HWND windowHandle;
ComPtr<ICoreWebView2Controller> controller;
ComPtr<ICoreWebView2> webview;
std::string selectedText;
bool canSave = false;
bool initialNavigation = true;

std::wstring wide(const std::string& value) {
    if (value.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0);
    require(size > 0, "Ожидается текст в кодировке UTF-8.");
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), size);
    return result;
}

std::string utf8(const std::wstring& value) {
    if (value.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    require(size > 0, "Некорректный Unicode-текст.");
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), size, nullptr, nullptr);
    return result;
}

void post(const std::string& kind, const std::string& body) {
    if (webview) webview->PostWebMessageAsString(wide(kind + "\n" + body).c_str());
}

void resize() {
    if (controller) {
        RECT bounds;
        GetClientRect(windowHandle, &bounds);
        controller->put_Bounds(bounds);
    }
}

void openFile() {
    wchar_t path[32768] = {};
    OPENFILENAMEW dialog = { sizeof(dialog) };
    dialog.hwndOwner = windowHandle;
    dialog.lpstrFilter = L"Текст UTF-8 (*.txt)\0*.txt\0Все файлы\0*.*\0";
    dialog.lpstrFile = path;
    dialog.nMaxFile = 32768;
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&dialog)) return;
    std::string content = readTextFile(path);
    canSave = false;
    post("file", content);
}

void saveFile() {
    require(canSave, "Сначала выполните обработку файла.");
    wchar_t path[32768] = L"filtered.txt";
    OPENFILENAMEW dialog = { sizeof(dialog) };
    dialog.hwndOwner = windowHandle;
    dialog.lpstrTitle = L"Сохранить в новый файл UTF-8";
    dialog.lpstrFilter = L"Текст UTF-8 (*.txt)\0*.txt\0";
    dialog.lpstrDefExt = L"txt";
    dialog.lpstrFile = path;
    dialog.nMaxFile = 32768;
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
    if (!GetSaveFileNameW(&dialog)) return;
    writeNewFile(path, selectedText);
    post("notice", "Отобранные строки сохранены в UTF-8.");
}

void copyText(const std::string& content) {
    std::wstring text = wide(content);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
    require(memory != nullptr, "Недостаточно памяти.");
    void* destination = GlobalLock(memory);
    if (!destination) { GlobalFree(memory); throw std::runtime_error("Не удалось подготовить буфер обмена."); }
    std::memcpy(destination, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
    GlobalUnlock(memory);
    if (!OpenClipboard(windowHandle)) { GlobalFree(memory); throw std::runtime_error("Буфер обмена занят. Повторите попытку."); }
    EmptyClipboard();
    HANDLE result = SetClipboardData(CF_UNICODETEXT, memory);
    CloseClipboard();
    if (!result) GlobalFree(memory);
    require(result != nullptr, "Не удалось скопировать код.");
    post("notice", "Полный C++-код скопирован.");
}

void execute(const std::string& command, const std::string& body) {
    require(body.size() <= 270000, "Слишком большой ввод.");
    require(body.find('\0') == std::string::npos, "Нулевой символ запрещён.");
    if (command == "open") { PostMessageW(windowHandle, WM_APP + 1, 0, 0); return; }
    if (command == "save") { PostMessageW(windowHandle, WM_APP + 2, 0, 0); return; }
    if (command == "copy") { copyText(body); return; }
    canSave = false;
    std::istringstream input(body);
    std::ostringstream output;
    if (command == "run11") lab11(input, output);
    else if (command == "run12") lab12(input, output);
    else if (command == "run21") lab21(input, output);
    else if (command == "run22") lab22(input, output);
    else if (command == "run31") lab31(input, output);
    else if (command == "run32") lab32(input, output);
    else if (command == "run41") lab41(input, output);
    else if (command == "run42") lab42(input, output);
    else if (command == "run51") lab51(input, output);
    else if (command == "run52") lab52(input, output);
    else if (command == "run61") lab61(input, output);
    else if (command == "run62") {
        std::string search = line(input, 2048);
        selectedText = filterText(input, output, search);
        canSave = true;
    } else throw std::runtime_error("Неизвестное задание.");
    post("ok", output.str());
}

void startupError() {
    MessageBoxW(windowHandle, L"Не удалось запустить интерфейс. Установите Microsoft Edge WebView2 Runtime с официального сайта Microsoft и повторите запуск. Приложение не требует Python или компилятора.", L"CyberLab — запуск интерфейса", MB_ICONERROR);
    DestroyWindow(windowHandle);
}

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_SIZE) { resize(); return 0; }
    if (message == WM_GETMINMAXINFO) {
        auto limits = reinterpret_cast<MINMAXINFO*>(lParam);
        limits->ptMinTrackSize = { 900, 640 };
        return 0;
    }
    if (message == WM_APP + 1 || message == WM_APP + 2) {
        try { if (message == WM_APP + 1) openFile(); else saveFile(); }
        catch (const std::exception& error) { post("notice", error.what()); }
        return 0;
    }
    if (message == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 1;
    WNDCLASSW windowClass = {};
    windowClass.hInstance = instance;
    windowClass.lpfnWndProc = windowProcedure;
    windowClass.lpszClassName = L"CyberLabWindow";
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(101));
    windowClass.hbrBackground = CreateSolidBrush(RGB(11, 16, 24));
    RegisterClassW(&windowClass);
    windowHandle = CreateWindowExW(0, windowClass.lpszClassName, L"CyberLab — Сагадатов Динияр · ИФБС-11", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1320, 900, nullptr, nullptr, instance, nullptr);
    if (!windowHandle) { CoUninitialize(); return 1; }
    ShowWindow(windowHandle, show);
    PWSTR localFolder = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &localFolder))) { startupError(); CoUninitialize(); return 1; }
    std::wstring cache = std::wstring(localFolder) + L"\\CyberLab\\WebView2";
    CoTaskMemFree(localFolder);
    HRESULT started = CreateCoreWebView2EnvironmentWithOptions(nullptr, cache.c_str(), nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([](HRESULT status, ICoreWebView2Environment* environment) -> HRESULT {
            if (FAILED(status) || !environment) { startupError(); return S_OK; }
            HRESULT created = environment->CreateCoreWebView2Controller(windowHandle,
                Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([](HRESULT result, ICoreWebView2Controller* viewController) -> HRESULT {
                    if (FAILED(result) || !viewController) { startupError(); return S_OK; }
                    controller = viewController;
                    controller->get_CoreWebView2(&webview);
                    ComPtr<ICoreWebView2Settings> settings;
                    webview->get_Settings(&settings);
                    settings->put_AreDefaultContextMenusEnabled(FALSE);
                    settings->put_AreDevToolsEnabled(FALSE);
                    settings->put_IsStatusBarEnabled(FALSE);
                    settings->put_AreDefaultScriptDialogsEnabled(FALSE);
                    EventRegistrationToken token;
                    webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>([](ICoreWebView2*, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                        if (initialNavigation) { initialNavigation = false; return S_OK; }
                        LPWSTR uri = nullptr; args->get_Uri(&uri);
                        if (!uri || std::wstring(uri).rfind(L"about:blank", 0) != 0) args->put_Cancel(TRUE);
                        CoTaskMemFree(uri); return S_OK;
                    }).Get(), &token);
                    webview->add_NewWindowRequested(Callback<ICoreWebView2NewWindowRequestedEventHandler>([](ICoreWebView2*, ICoreWebView2NewWindowRequestedEventArgs* args) -> HRESULT {
                        args->put_Handled(TRUE); return S_OK;
                    }).Get(), &token);
                    webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>([](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                        LPWSTR raw = nullptr;
                        if (FAILED(args->TryGetWebMessageAsString(&raw))) return S_OK;
                        std::wstring received(raw);
                        CoTaskMemFree(raw);
                        try {
                            require(received.size() <= 270100, "Слишком большой запрос.");
                            std::string message = utf8(received);
                            size_t split = message.find('\n');
                            require(split != std::string::npos, "Неверный формат запроса.");
                            execute(message.substr(0, split), message.substr(split + 1));
                        } catch (const std::exception& error) { canSave = false; post("error", error.what()); }
                        return S_OK;
                    }).Get(), &token);
                    resize();
                    if (FAILED(webview->NavigateToString(wide(HTML).c_str()))) startupError();
                    return S_OK;
                }).Get());
            if (FAILED(created)) startupError();
            return S_OK;
        }).Get());
    if (FAILED(started)) startupError();
    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) { TranslateMessage(&message); DispatchMessageW(&message); }
    if (controller) controller->Close();
    webview.Reset(); controller.Reset();
    DeleteObject(windowClass.hbrBackground);
    CoUninitialize();
    return 0;
}
