#include "Lines.h"

#include "Screen.h"

Lines::Lines(std::vector<std::wstring> lines, SHORT padX, SHORT padY)
    : lines(std::move(lines))
    , padX(padX)
    , padY(padY)
{
}

const std::vector<std::wstring>& Lines::getLines() const {
    return lines;
}

void Lines::setLines(std::vector<std::wstring> newLines) {
    lines = std::move(newLines);
    setSelectedIdx(getSelectedIdx());
    setScrollOffset(getScrollOffset());
}

int Lines::getSelectedIdx() const {
    return selectedIdx;
}

std::wstring Lines::getSelectedText() const {
    if (!hasSelection()) {
        return L"";
    }
    return lines[selectedIdx];
}

void Lines::setSelectedIdx(int newIdx) {
    selectedIdx = std::max(-1, std::min(newIdx, (int)lines.size()));
    if (hasSelection() && selectedIdx < scrollOffset) {
        scrollOffset = selectedIdx;
    }
}

bool Lines::hasSelection() const {
    return selectedIdx != -1;
}

void Lines::unselect() {
    selectedIdx = -1;
}

void Lines::selectPrev() {
    if (!hasSelection()) {
        return;
    }
    if (selectedIdx > 0) {
        --selectedIdx;
    }
}

void Lines::selectNext() {
    if (!hasSelection()) {
        return;
    }
    if (selectedIdx < lines.size() - 1) {
        ++selectedIdx;
    }
}

void Lines::scrollToSelection(int visibleHeight) {
    if (selectedIdx < scrollOffset) {
        scrollOffset = selectedIdx;
    }
    if (visibleHeight != -1 && scrollOffset < selectedIdx - visibleHeight + 1) {
        scrollOffset = selectedIdx - visibleHeight + 1;
    }
}

int Lines::getScrollOffset() const {
    return scrollOffset;
}

void Lines::setScrollOffset(int newOffset) {
    scrollOffset = std::max(0, std::min(newOffset, (int)lines.size() - 1));
}

void Lines::drawTextOn(Screen& screen, const Rect& rect, bool centered) {
    COORD origin = rect.getLeftTop();
    int linesCount = lines.size() - scrollOffset;
    if (centered && linesCount < rect.h) {
        origin.Y += (rect.h - linesCount)/2;
    }
    for (int idx = scrollOffset; idx < lines.size(); ++idx) {
        if (origin.Y >= rect.y + rect.h) {
            break;
        }
        screen.boundedLine(origin, rect.w, lines[idx], centered);
        ++origin.Y;
    }
}

void Lines::drawOn(Screen& screen, const Rect& rect, WORD colorAttr, WORD selectionAttr, bool centered) {
    screen.paintRect(rect, colorAttr);
    if (hasSelection()) {
        int selectionOffset = selectedIdx - scrollOffset;
        if (selectionOffset >= 0 && selectionOffset < rect.h - 2*padY) {
            SHORT selectionY = rect.y + selectionOffset + padY;
            if (centered) {
                int linesCount = lines.size() - scrollOffset;
                if (linesCount < rect.h) {
                    selectionY += (rect.h - linesCount)/2;
                }
            }
            Rect selectionRect{rect.x, selectionY, rect.w, 1};
            screen.paintRect(selectionRect.withPadX(padX), selectionAttr);
        }
    }
    drawTextOn(screen, rect.withPadding(padX, padY), centered);
}
