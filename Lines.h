#pragma once

#include <vector>
#include <string>
#include <windows.h>

class Screen;
struct Rect;

struct StyledText {
    std::wstring text;
    WORD normalAttr;
    WORD selectedAttr;
};

class Lines {
public:
    Lines(std::vector<std::wstring> lines = {}, SHORT padX = 0, SHORT padY = 0);

    void setLines(std::vector<std::wstring> textLines, WORD normalAttr = 0, WORD selectedAttr = 0);
    void setLines(std::vector<StyledText> newLines);

    int findLine(const std::wstring& text) const;

    int getSelectedIdx() const;
    std::wstring getSelectedText() const;
    void setSelectedIdx(int newIdx);
    bool hasSelection() const;
    void unselect();

    void selectPrev();
    void selectNext();
    void scrollToSelection(int visibleHeight);

    int getScrollOffset() const;
    void setScrollOffset(int newOffset);

    void drawTextOn(Screen& screen, const Rect& rect, bool centered = false);
    void drawOn(Screen& screen, const Rect& rect, WORD backgroundAttr, bool centered = false);

private:
    void _drawOn(Screen& screen, const Rect& rect, bool centered, bool withAttrs);

    std::vector<StyledText> lines;
    SHORT padX;
    SHORT padY;
    int selectedIdx = -1;
    int scrollOffset = 0;
};
