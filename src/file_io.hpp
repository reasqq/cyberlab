#pragma once
#include <windows.h>
#include <filesystem>
#include "../labs/common.hpp"

inline std::string readTextFile(const std::filesystem::path& path) {
    std::error_code error;
    require(std::filesystem::is_regular_file(path, error), "Выберите обычный текстовый файл.");
    std::ifstream file(path, std::ios::binary);
    require(bool(file), "Не удалось открыть файл.");
    std::string content(262145, '\0');
    file.read(content.data(), static_cast<std::streamsize>(content.size()));
    content.resize(static_cast<size_t>(file.gcount()));
    require(!file.bad(), "Ошибка чтения файла.");
    require(content.size() <= 262144, "Файл больше 256 КиБ.");
    if (content.compare(0, 3, "\xEF\xBB\xBF") == 0) content.erase(0, 3);
    require(content.find('\0') == std::string::npos, "Файл содержит нулевые байты. Выберите текст UTF-8.");
    require(content.empty() || MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, content.data(), static_cast<int>(content.size()), nullptr, 0) > 0,
            "Ожидается файл в кодировке UTF-8.");
    return content;
}

inline void writeNewFile(const std::filesystem::path& path, const std::string& text) {
    std::wstring name = path.filename().wstring();
    require(!name.empty() && name.back() != L'.' && name.back() != L' ' && name.find_first_of(L"<>:\"|?*") == std::wstring::npos,
            "Недопустимое имя файла.");
    std::wstring base = name.substr(0, name.find(L'.'));
    for (wchar_t& c : base) if (c >= L'a' && c <= L'z') c -= L'a' - L'A';
    bool device = base == L"CON" || base == L"PRN" || base == L"AUX" || base == L"NUL";
    device = device || (base.size() == 4 && (base.substr(0, 3) == L"COM" || base.substr(0, 3) == L"LPT") && base[3] >= L'1' && base[3] <= L'9');
    require(!device, "Это зарезервированное Windows имя. Выберите другое.");
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    require(file != INVALID_HANDLE_VALUE, "Не удалось создать файл. Выберите новое имя: существующие файлы защищены от перезаписи.");
    if (GetFileType(file) != FILE_TYPE_DISK) { CloseHandle(file); throw std::runtime_error("Нужен обычный файл на диске."); }
    DWORD written = 0;
    bool success = WriteFile(file, text.data(), static_cast<DWORD>(text.size()), &written, nullptr) && written == text.size();
    bool closed = CloseHandle(file) != 0;
    if (!success || !closed) DeleteFileW(path.c_str());
    require(success && closed, "Не удалось полностью записать файл.");
}
