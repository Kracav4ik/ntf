#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include <map>

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
    Rect moved(SHORT dx, SHORT dy) const {
        Rect copy = *this;
        copy.x += dx;
        copy.y += dy;
        return copy;
    }
    Rect withW(SHORT newW) const {
        Rect copy = *this;
        copy.w = newW;
        return copy;
    }
    Rect withH(SHORT newH) const {
        Rect copy = *this;
        copy.h = newH;
        return copy;
    }
};

enum class EventState {
    Handled,
    Unhandled
};

class Screen {
public:
    Screen(SHORT width, SHORT height);
    ~Screen();

    void clear(WORD colorAttr);
    void paintRect(const Rect& rect, WORD colorAttr, bool clearChars = true);
    void textOut(COORD pos, const std::wstring& text);
    void textOut(COORD pos, const std::wstring& text, DWORD size);
    void boundedLine(COORD pos, SHORT w, const std::wstring& text, bool centered = false);
    void frame(const Rect& rect, bool fat = true);
    void separator(const Rect& rect, bool fatLine = false, bool fatEnds = true);
    void flip();

    void setTitle(const std::wstring& title);
    void setCursorVisible(bool visible);

    SHORT w() const;
    SHORT h() const;
    COORD center() const;

    void processEvent();
    void handleKey(WORD virtualKey, WORD modifiers, std::function<void()> callback);
    void tryHandleKey(WORD virtualKey, WORD modifiers, std::function<EventState()> callback);

private:
    static HANDLE createBuffer(SHORT width, SHORT height);

    SHORT width;
    SHORT height;
    HANDLE stdinHandle;
    HANDLE origConsole;
    HANDLE currConsole;
    HANDLE nextConsole;

    std::multimap<DWORD, std::function<EventState()>> keyHandlers;
};
