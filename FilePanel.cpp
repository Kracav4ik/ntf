#include "FilePanel.h"
#include "colors.h"

std::vector<std::wstring> getListDirs(const std::wstring& path) {
    std::wstring mask = path + L"\\*";
    WIN32_FIND_DATAW findData;
    HANDLE find = FindFirstFileW(mask.c_str(), &findData);

    std::vector<std::wstring> dirs;

    do {
        std::wstring name = findData.cFileName;
        if (name == L".") {
            continue;
        }
        dirs.push_back(std::move(name));
    } while (FindNextFileW(find, &findData) != 0);

    FindClose(find);

    return dirs;
}

FilePanel::FilePanel(Rect rect, std::wstring path): rect(rect), path(std::move(path)) {
    updateLines();
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
    if (lines.hasSelection() && lines.getSelectedText() == L"..") {
        path = path.substr(0, path.rfind(L'\\'));
        updateLines();
    }
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

void FilePanel::updateLines() {
    lines.setLines(getListDirs(path));
}
