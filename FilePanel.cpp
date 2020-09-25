#include "FilePanel.h"
#include "colors.h"

FilePanel::FilePanel(Rect rect, std::wstring path): rect(rect), path(std::move(path)) {
    lines.setLines({L"Тревога!", L"Тревога!", L"Волк", L"украл зайчат"});
    lines.setSelectedIdx(0);
    lastSelectedIdx = 0;
}

void FilePanel::selectPrev() {
    lines.selectPrev();
    lines.scrollToSelection(rect.h - 2);
}

void FilePanel::selectNext() {
    lines.selectNext();
    lines.scrollToSelection(rect.h - 2);
}

void FilePanel::enter() {
//    TODO: DO SOMETHING
}

void FilePanel::drawOn(Screen& s) {
    s.paintRect(rect, FG::CYAN | BG::DARK_BLUE);
    lines.drawOn(s, rect.withPadding(1, 1), FG::WHITE | BG::DARK_BLUE, FG::BLACK | BG::DARK_CYAN);
    s.frame(rect);
}

const std::wstring& FilePanel::getPath() const {
    return path;
}

bool FilePanel::hasSelection() {
    return lines.hasSelection();
}

void FilePanel::select() {
    lines.setSelectedIdx(lastSelectedIdx);
}

void FilePanel::unselect() {
    lastSelectedIdx = lines.getSelectedIdx();
    lines.unselect();
}
