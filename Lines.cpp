#include "Lines.h"

#include "Screen.h"

std::vector<StyledText> styledText(std::vector<std::wstring> textLines, WORD normalAttr, WORD selectedAttr) {
    std::vector<StyledText> result;
    for (auto&& line : textLines) {
        result.push_back({std::move(line), normalAttr, selectedAttr});
    }
    return result;
}

Lines::Lines(std::vector<StyledText> lines) {
    setLines(std::move(lines));
}

void Lines::setLines(std::vector<StyledText> newLines) {
    lines = std::move(newLines);
    setSelectedIdx(getSelectedIdx());
    setScrollOffset(getScrollOffset());
}

int Lines::findLine(const std::wstring& text) const {
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].text == text) {
            return i;
        }
    }
    return -1;
}

int Lines::getSelectedIdx() const {
    return selectedIdx;
}

std::wstring Lines::getSelectedText() const {
    if (!hasSelection()) {
        return L"";
    }
    return lines[selectedIdx].text;
}

void Lines::setSelectedIdx(int newIdx) {
    selectedIdx = std::max(-1, std::min(newIdx, (int)lines.size() - 1));
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
    _drawOn(screen, rect, centered, false);
}

void Lines::drawOn(Screen& screen, const Rect& rect, bool centered) {
    _drawOn(screen, rect, centered, true);
}

void Lines::_drawOn(Screen& screen, const Rect& rect, bool centered, bool withAttrs) {
    COORD origin = rect.getLeftTop();
    int linesCount = lines.size() - scrollOffset;
    if (centered && linesCount < rect.h) {
        origin.Y += (rect.h - linesCount)/2;
    }
    for (int idx = scrollOffset; idx < lines.size(); ++idx) {
        if (origin.Y >= rect.y + rect.h) {
            break;
        }
        if (withAttrs) {
            Rect line{origin.X, origin.Y, rect.w, 1};
            WORD attr = idx == selectedIdx ? lines[idx].selectedAttr : lines[idx].normalAttr;
            screen.paintRect(line, attr);
        }
        screen.boundedLine(origin, rect.w, lines[idx].text, centered);
        ++origin.Y;
    }
}
