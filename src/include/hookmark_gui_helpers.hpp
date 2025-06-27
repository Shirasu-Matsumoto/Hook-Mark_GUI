#ifndef __HOOKMARK_GUI_HELPERS_HPP__
#define __HOOKMARK_GUI_HELPERS_HPP__

#include <string>
#include <windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <unordered_map>
#include <fstream>
#include <cmath>

namespace hmgui {
    inline D2D1_COLOR_F red_color = D2D1::ColorF(D2D1::ColorF::Red);
    inline D2D1_COLOR_F blue_color = D2D1::ColorF(D2D1::ColorF::Blue);
    inline D2D1_COLOR_F black_color = D2D1::ColorF(D2D1::ColorF::Black);
    inline D2D1_COLOR_F kifu_edge_color = D2D1::ColorF(0x176ff7);
    inline D2D1_COLOR_F kifu_bg_color = D2D1::ColorF(0x689be9, 0.6f);
    inline D2D1_COLOR_F gray_color = D2D1::ColorF(D2D1::ColorF::Gray);
    inline D2D1_COLOR_F yellow_color = D2D1::ColorF(0xeeee11);
    inline D2D1_COLOR_F white_color = D2D1::ColorF(D2D1::ColorF::White);
    inline D2D1_COLOR_F white_smoke_color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke);
    inline D2D1_COLOR_F light_gray_color = D2D1::ColorF(D2D1::ColorF::LightGray);

    inline std::string utf16_to_utf8(const std::wstring &utf16_str) {
        if (utf16_str.empty()) return std::string();

        int utf8_len = WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), nullptr, 0, nullptr, nullptr);
        if (utf8_len <= 0) return std::string();

        std::string utf8_str(utf8_len, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), &utf8_str[0], utf8_len, nullptr, nullptr);
        return utf8_str;
    }

    inline std::wstring utf8_to_utf16(const std::string &utf8_str) {
        if (utf8_str.empty()) return std::wstring();

        int utf16_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), nullptr, 0);
        if (utf16_len <= 0) return std::wstring();

        std::wstring utf16_str(utf16_len, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), &utf16_str[0], utf16_len);
        return utf16_str;
    }

    inline void load_config_single(const std::string &filepath, std::unordered_map<std::string, std::string> &config) {
        std::ifstream file(filepath);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '[') continue;

            size_t pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\"") + 1);

            config[key] = value;
        }
    }

    inline D2D1_RECT_F rect_to_rectf(RECT rect) {
        return D2D1::RectF(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right),
            static_cast<float>(rect.bottom)
        );
    }

    inline RECT rectf_to_rect(D2D1_RECT_F rectf) {
        return {
            static_cast<long>(rectf.left),
            static_cast<long>(rectf.top),
            static_cast<long>(rectf.right),
            static_cast<long>(rectf.bottom)
        };
    }

    inline D2D1_RECT_F cliped_rectf(D2D1_RECT_F rectf) {
        return {
            rectf.left + 1.0f,
            rectf.top + 1.0f,
            rectf.right - 1.0f,
            rectf.bottom - 1.0f
        };
    }

    inline D2D1_RECT_F round_rectf(D2D1_RECT_F rectf) {
        return D2D1::RectF(
            std::round(rectf.left),
            std::round(rectf.top),
            std::round(rectf.right),
            std::round(rectf.bottom)
        );
    }
}

#endif
