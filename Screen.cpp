#include "Screen.h"

#include "Popup.h"

#include <memory>
#include <cwchar>

namespace {

enum {HOR, VERT};
enum {LT, RT, LB, RB};
enum {LHOR, RHOR, TVERT, BVERT};
struct LineStyles {
    std::wstring lines;
    std::wstring corners;
    std::wstring joinToFat;
    std::wstring joinToSlim;
};
const LineStyles FAT = {
    L"═║",
    L"╔╗╚╝",
    L"╠╣╦╩",
    L"╞╡╥╨"
};
const LineStyles SLIM = {
    L"─│",
    L"┌┐└┘",
    L"╟╢╤╧",
    L"├┤┬┴"
};

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

void Screen::paintRect(const Rect& rect, WORD colorAttr, bool clearChars) {
    DWORD _unused;
    COORD origin = rect.getLeftTop();
    for (int i = 0; i < rect.h; ++i) {
        if (clearChars) {
            FillConsoleOutputCharacterW(nextConsole, ' ', rect.w, origin, &_unused);
        }
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
    SHORT w = rect.w;
    SHORT h = rect.h;
    if (w < 2 || h < 2) {
        return;
    }
    COORD lt = rect.getLeftTop();
    COORD rb = {(SHORT)(lt.X + w - 1), (SHORT)(lt.Y + h - 1)};

    DWORD _unused;
    const std::wstring& lines = fat ? FAT.lines : SLIM.lines;
    const std::wstring& corners = fat ? FAT.corners : SLIM.corners;
    FillConsoleOutputCharacterW(nextConsole, lines[HOR], w, {lt.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, lines[HOR], w, {lt.X, rb.Y}, &_unused);
    for (SHORT i = lt.Y + 1; i < rb.Y; ++i) {
        FillConsoleOutputCharacterW(nextConsole, lines[VERT], 1, {lt.X, i}, &_unused);
        FillConsoleOutputCharacterW(nextConsole, lines[VERT], 1, {rb.X, i}, &_unused);
    }
    FillConsoleOutputCharacterW(nextConsole, corners[LT], 1, {lt.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, corners[RT], 1, {rb.X, lt.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, corners[LB], 1, {lt.X, rb.Y}, &_unused);
    FillConsoleOutputCharacterW(nextConsole, corners[RB], 1, {rb.X, rb.Y}, &_unused);
}

void Screen::separator(const Rect& rect, bool fatLine, bool fatEnds) {
    const auto& style = fatLine ? FAT : SLIM;
    const std::wstring& lines = style.lines;
    const std::wstring& joins = fatEnds ? style.joinToFat : style.joinToSlim;
    bool isHor = rect.h == 1;

    int count = isHor ? rect.w : rect.h;
    SHORT dx = isHor ? 1 : 0;
    SHORT dy = isHor ? 0 : 1;
    COORD pos = rect.getLeftTop();
    int first = isHor ? LHOR : TVERT;
    int mid = isHor ? HOR : VERT;
    int last = isHor ? RHOR : BVERT;

    DWORD _unused;
    FillConsoleOutputCharacterW(nextConsole, joins[first], 1, {pos.X, pos.Y}, &_unused);
    pos.X += dx;
    pos.Y += dy;
    for (int i = 1; i < count - 1; ++i) {
        FillConsoleOutputCharacterW(nextConsole, lines[mid], 1, {pos.X, pos.Y}, &_unused);
        pos.X += dx;
        pos.Y += dy;
    }
    FillConsoleOutputCharacterW(nextConsole, joins[last], 1, {pos.X, pos.Y}, &_unused);
}

void Screen::labelsFill(const Rect& rect, const std::vector<std::wstring>& labelsList, WORD colorAttr) {
    labels(rect, labelsList, colorAttr, -1);
}

void Screen::labels(const Rect& rect, const std::vector<std::wstring>& labelsList, WORD colorAttr, int separator) {
    int labelsWidthSum = 0;
    for (const auto& label : labelsList) {
        labelsWidthSum += label.size();
    }
    SHORT y = rect.y;
    SHORT x = rect.x;
    int separatorsSum;
    if (separator == -1) {
        separatorsSum = std::max((int)labelsList.size() - 1, rect.w - labelsWidthSum);
    } else {
        separatorsSum = separator * (labelsList.size() - 1);
        x += (rect.w - separatorsSum - labelsWidthSum)/2;
    }
    int curLabelsWidth = 0;
    for (int i = 0; i < labelsList.size(); ++i) {
        SHORT labelX = x;
        if (i > 0) {
            labelX += curLabelsWidth + i*separatorsSum/(labelsList.size() - 1);
        }
        const auto& label = labelsList[i];
        paintRect({labelX, y, (SHORT)label.size(), 1}, colorAttr, false);
        textOut({labelX, y}, label);
        curLabelsWidth += label.size();
    }
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

SHORT Screen::w() const {
    return width;
}

SHORT Screen::h() const {
    return height;
}

COORD Screen::center() const {
    return {(SHORT)(width / 2), (SHORT)(height / 2)};
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
        auto globalIt = priorityHandlers.find(key);
        if (globalIt != priorityHandlers.end()) {
            globalIt->second();
            return;
        }
        for (Popup* owner : ownersOrder) {
            if (!owner->isPopupVisible()) {
                continue;
            }
            auto& handlers = handlersByPopup[owner];
            auto it = handlers.find(key);
            if (it != handlers.end()) {
                it->second();
            }
            return;
        }
        globalIt = globalHandlers.find(key);
        if (globalIt != globalHandlers.end()) {
            globalIt->second();
            return;
        }
    }
}

void Screen::appendOwner(Popup* owner) {
    ownersOrder.push_back(owner);
}

void Screen::handlePriorityKey(WORD virtualKey, WORD modifiers, std::function<void()> callback) {
    priorityHandlers.emplace(makeKey(virtualKey, fixAltCtrl(modifiers)), std::move(callback));
}

void Screen::handleKey(WORD virtualKey, WORD modifiers, std::function<void()> callback) {
    globalHandlers.emplace(makeKey(virtualKey, fixAltCtrl(modifiers)), std::move(callback));
}

void Screen::handleKey(Popup* owner, WORD virtualKey, WORD modifiers, std::function<void()> callback) {
    handlersByPopup[owner].emplace(makeKey(virtualKey, fixAltCtrl(modifiers)), std::move(callback));
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
