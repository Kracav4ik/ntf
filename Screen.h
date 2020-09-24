#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#define ANY_ALT_PRESSED (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)
#define ANY_CTRL_PRESSED (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)

struct Rect {
    SHORT x;
    SHORT y;
    SHORT w;
    SHORT h;

    COORD getLeftTop() const { return {x, y}; }

    Rect withPadX(SHORT padX) const {
        Rect copy = *this;
        copy.x += padX;
        copy.w -= 2 * padX;
        return copy;
    }
    Rect withPadY(SHORT padY) const {
        Rect copy = *this;
        copy.y += padY;
        copy.h -= 2 * padY;
        return copy;
    }
    Rect withPadding(SHORT padX, SHORT padY) const {
        Rect copy = *this;
        copy.x += padX;
        copy.y += padY;
        copy.w -= 2 * padX;
        copy.h -= 2 * padY;
        return copy;
    }
};

class Screen {
public:
    Screen(SHORT width, SHORT height);
    ~Screen();

    void clear(WORD colorAttr);
    void paintRect(const Rect& rect, WORD colorAttr);
    void textOut(COORD pos, const std::wstring& text);
    void textOut(COORD pos, const std::wstring& text, DWORD size);
    void boundedLine(COORD pos, SHORT w, const std::wstring& text, bool centered = false);
    void frame(const Rect& rect, bool fat = true);
    void flip();

    void setTitle(const std::wstring& title);
    void setCursorVisible(bool visible);

    void processEvent();
    void handleKey(WORD virtualKey, WORD modifiers, std::function<void()> callback);

private:
    static HANDLE createBuffer(SHORT width, SHORT height);

    SHORT width;
    SHORT height;
    HANDLE stdinHandle;
    HANDLE origConsole;
    HANDLE currConsole;
    HANDLE nextConsole;

    std::unordered_map<DWORD, std::function<void()>> keyHandlers;
};
