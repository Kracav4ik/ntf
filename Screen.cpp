#include "Screen.h"

#include <memory>
#include <cwchar>

namespace {

WORD fixAltCtrl(WORD mask) {
    if ((mask & ANY_ALT_PRESSED) != 0) {
        mask |= ANY_ALT_PRESSED;
    }
    if ((mask & ANY_CTRL_PRESSED) != 0) {
        mask |= ANY_CTRL_PRESSED;
    }
    return mask;
}

DWORD makeKey(WORD virtualKey, WORD modifiers) {
    return virtualKey | (modifiers << 16);
}

}

Screen::Screen(SHORT width, SHORT height)
    : width(width)
    , height(height)
{
    stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    origConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    currConsole = createBuffer(width, height);
    nextConsole = createBuffer(width, height);
    SetConsoleActiveScreenBuffer(currConsole);
}

Screen::~Screen() {
    SetConsoleActiveScreenBuffer(origConsole);
}

void Screen::clear(WORD colorAttr) {
    paintRect({0, 0, width, height}, colorAttr);
}

void Screen::paintRect(const Rect& rect, WORD colorAttr) {
    DWORD _unused;
    COORD origin = rect.getLeftTop();
    for (int i = 0; i < rect.h; ++i) {
        FillConsoleOutputCharacterW(nextConsole, ' ', rect.w, origin, &_unused);
        FillConsoleOutputAttribute(nextConsole, colorAttr, rect.w, origin, &_unused);
        ++origin.Y;
    }
}

void Screen::textOut(COORD pos, const std::wstring& text) {
    textOut(pos, text, text.size());
}

void Screen::textOut(COORD pos, const std::wstring& text, DWORD size) {
    DWORD _unused;
    WriteConsoleOutputCharacterW(nextConsole, text.c_str(), size, pos, &_unused);
}

void Screen::boundedLine(COORD pos, SHORT w, const std::wstring& text, bool centered) {
    if (w < 1) {
        return;
    }
    SHORT textSize = text.size();
    if (textSize > w) {
        static const std::wstring ELLIPSIS = L"...";
        if (ELLIPSIS.size() >= w) {
            textOut(pos, ELLIPSIS, w);
        } else {
            textOut(pos, text, w - ELLIPSIS.size());
            pos.X += w - ELLIPSIS.size();
            textOut(pos, ELLIPSIS);
        }
    } else {
        if (centered) {
            pos.X += (w - textSize)/2;
        }
        textOut(pos, text);
    }
}

void Screen::frame(const Rect& rect, bool fat) {
    static const std::wstring FAT_RECT  = L"═║╔╗╚╝";
    static const std::wstring SLIM_RECT = L"─│┌┐└┘";
    enum {HOR, VERT, LT, RT, LB, RB};

    SHORT w = rect.w;
    SHORT h = rect.h;
    if (w < 2 || h < 2) {
        return;
    }
    COORD lt = rect.getLeftTop();
    COORD rb = {(SHORT)(lt.X + w - 1), (SHORT)(lt.Y + h - 1)};

    DWORD _unused;
    const std::wstring& chars = fat ? FAT_RECT : SLIM_RECT;
    FillConsoleOutputCharacterW(nextConsole, chars[HOR], w, {lt.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, chars[HOR], w, {lt.X, rb.Y}, &_unused);
    for (SHORT i = lt.Y + 1; i < rb.Y; ++i) {
        FillConsoleOutputCharacterW(nextConsole, chars[VERT], 1, {lt.X, i}, &_unused);
        FillConsoleOutputCharacterW(nextConsole, chars[VERT], 1, {rb.X, i}, &_unused);
    }
    FillConsoleOutputCharacterW(nextConsole, chars[LT], 1, {lt.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, chars[RT], 1, {rb.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, chars[LB], 1, {lt.X, rb.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, chars[RB], 1, {rb.X, rb.Y}, &_unused);
}

void Screen::flip() {
    std::swap(currConsole, nextConsole);
    SetConsoleActiveScreenBuffer(currConsole);
}

void Screen::setTitle(const std::wstring& title) {
    SetConsoleTitleW(title.c_str());
}

void Screen::setCursorVisible(bool visible) {
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(origConsole, &info);
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(currConsole, &info);
    SetConsoleCursorInfo(nextConsole, &info);
}

void Screen::processEvent() {
    INPUT_RECORD event;
    DWORD count;
    ReadConsoleInputW(stdinHandle, &event, 1, &count);
    if (count == 1 && event.EventType == KEY_EVENT) {
        const auto& keyEvent = event.Event.KeyEvent;
        if (keyEvent.bKeyDown != TRUE) {
            return;
        }
        WORD keyState = fixAltCtrl(keyEvent.dwControlKeyState & (ANY_ALT_PRESSED | ANY_CTRL_PRESSED | SHIFT_PRESSED));
        DWORD key = makeKey(keyEvent.wVirtualKeyCode, keyState);
        auto it = keyHandlers.find(key);
        if (it != keyHandlers.end()) {
            it->second();
        }
    }
}

void Screen::handleKey(WORD virtualKey, WORD modifiers, std::function<void()> callback) {
    keyHandlers.emplace(makeKey(virtualKey, fixAltCtrl(modifiers)), std::move(callback));
}

HANDLE Screen::createBuffer(SHORT width, SHORT height) {
    HANDLE buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

    CONSOLE_SCREEN_BUFFER_INFO oldInfo;
    GetConsoleScreenBufferInfo(buffer, &oldInfo);

    COORD screenBuf;
    screenBuf.X = std::max(width, oldInfo.dwSize.X);
    screenBuf.Y = std::max(height, oldInfo.dwSize.Y);
    SetConsoleScreenBufferSize(buffer, screenBuf);

    SMALL_RECT screenRect;
    screenRect.Left   = 0;
    screenRect.Top    = 0;
    screenRect.Right  = width - 1;
    screenRect.Bottom = height - 1;
    SetConsoleWindowInfo(buffer, TRUE, &screenRect);

    screenBuf.X = width;
    screenBuf.Y = height;
    SetConsoleScreenBufferSize(buffer, screenBuf);

    return buffer;
}
