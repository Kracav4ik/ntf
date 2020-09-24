#pragma once

#include <vector>
#include <string>
#include <windows.h>

class Screen;
struct Rect;

class Lines {
public:
    Lines(std::vector<std::wstring> lines = {}, SHORT padX = 0, SHORT padY = 0);

    const std::vector<std::wstring>& getLines() const;
    void setLines(std::vector<std::wstring> newLines);

    int getSelectedIdx() const;
    void setSelectedIdx(int newIdx);
    bool hasSelection() const;
    void unselect();

    void selectPrev();
    void selectNext();
    void scrollToSelection(int visibleHeight);

    int getScrollOffset() const;
    void setScrollOffset(int newOffset);

    void drawTextOn(Screen& screen, const Rect& rect, bool centered = false);
    void drawOn(Screen& screen, const Rect& rect, WORD colorAttr, WORD selectionAttr = 0, bool centered = false);

private:
    std::vector<std::wstring> lines;
    SHORT padX;
    SHORT padY;
    int selectedIdx = -1;
    int scrollOffset = 0;
};
