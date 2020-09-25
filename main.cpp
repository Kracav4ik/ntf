#include <iostream>
#include "Lines.h"
#include "Screen.h"

#include "colors.h"
#include "FilePanel.h"

std::wstring getFullPath(const std::wstring& path) {
    DWORD size = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
    std::wstring result(size - 1, L' ');
    GetFullPathNameW(path.c_str(), size, result.data(), nullptr);
    return result;
}

int main() {
    Screen s(80, 25);
    s.setTitle(L"Not too far");
    s.setCursorVisible(false);

    std::wstring appDir = getFullPath(L".");

    bool running = true;
    FilePanel leftPanel({0, 0, 40, 23}, appDir);
    FilePanel rightPanel({40, 0, 40, 23}, appDir);
    Lines bottom;

    auto repaint = [&]() {
        s.clear(FG::GREY | BG::BLACK);

        leftPanel.drawOn(s);
        rightPanel.drawOn(s);
        bottom.drawOn(s, {0, 23, 80, 1});

        s.flip();
    };

    auto getCurrentPanel = [&]() -> FilePanel& {
        return rightPanel.hasSelection() ? rightPanel : leftPanel;
    };
    auto updateBottom = [&]() {
        bottom.setLines(styledText({ getCurrentPanel().getPath() }, FG::GREY | BG::BLACK));
    };

    s.handleKey(VK_F10, 0, [&]() {
        running = false;
    });
    s.handleKey(VK_TAB, 0, [&]() {
        leftPanel.hasSelection() ? leftPanel.unselect() : leftPanel.select();
        rightPanel.hasSelection() ? rightPanel.unselect() : rightPanel.select();
        updateBottom();
    });
    s.handleKey(VK_UP, 0, [&]() {
        getCurrentPanel().selectPrev();
    });
    s.handleKey(VK_DOWN, 0, [&]() {
        getCurrentPanel().selectNext();
    });
    s.handleKey(VK_HOME, 0, [&]() {
        getCurrentPanel().selectFirst();
    });
    s.handleKey(VK_END, 0, [&]() {
        getCurrentPanel().selectLast();
    });

    s.handleKey(VK_RETURN, 0, [&]() {
        getCurrentPanel().enter();
        updateBottom();
    });

    rightPanel.unselect();
    updateBottom();
    repaint();
    while (running) {
        s.processEvent();
        repaint();
    }
}
