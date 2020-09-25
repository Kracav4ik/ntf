#include <iostream>

#include "FilePanel.h"
#include "colors.h"

struct ListDirsResults {
    bool isRoot;
    std::vector<std::wstring> dirs;
    std::vector<std::wstring> files;
};

ListDirsResults getListDirs(const std::wstring& path) {
    bool isRoot = true;
    std::vector<std::wstring> dirs;
    std::vector<std::wstring> files;

    std::wstring mask = path + L"\\*";
    WIN32_FIND_DATAW findData;
    HANDLE find = FindFirstFileW(mask.c_str(), &findData);

    do {
        std::wstring name = findData.cFileName;
        if (name == L".") {
            continue;
        }
        if (name == L"..") {
            isRoot = false;
            continue;
        }
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            dirs.push_back(std::move(name));
        } else {
            files.push_back(std::move(name));
        }
    } while (FindNextFileW(find, &findData) != 0);

    FindClose(find);
    std::sort(files.begin(), files.end());
    std::sort(dirs.begin(), dirs.end());

    return {isRoot, dirs, files};
}

FilePanel::FilePanel(Rect rect, std::wstring path): rect(rect), path(std::move(path)) {
    updateLines();
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
    if (lines.hasSelection() && lines.getSelectedIdx() < dirsCount) {
        if (lines.getSelectedText() == L"..") {
            path = path.substr(0, path.rfind(L'\\'));
        } else {
            path = path + L'\\' + lines.getSelectedText();
        }
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
    auto listDirs = getListDirs(path);
    std::vector<std::wstring> newLines;

    if (!listDirs.isRoot) {
        newLines.emplace_back(L"..");
    }

    for (auto&& dir : listDirs.dirs) {
        newLines.push_back(std::move(dir));
    }

    dirsCount = newLines.size();

    for (auto&& file : listDirs.files) {
        newLines.push_back(std::move(file));
    }

    lines.setLines(newLines);
    lines.setSelectedIdx(0);
}
