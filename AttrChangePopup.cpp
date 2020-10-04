#include "AttrChangePopup.h"

#include "Screen.h"
#include "colors.h"

static const DWORD ATTRS[] = {
        FILE_ATTRIBUTE_READONLY,
        FILE_ATTRIBUTE_ARCHIVE,
        FILE_ATTRIBUTE_HIDDEN,
        FILE_ATTRIBUTE_SYSTEM,
};
static const std::wstring NAMES[] = {
        L"Только для чтения",
        L"Архивный",
        L"Скрытый",
        L"Системный",
};

std::wstring getTimeStr(const FILETIME& time) {
    SYSTEMTIME ts;
    FileTimeToSystemTime(&time, &ts);
    SYSTEMTIME tl;
    SystemTimeToTzSpecificLocalTime(nullptr, &ts, &tl);
    std::vector<wchar_t> buf(100);
    std::swprintf(buf.data(), buf.size(), L"%02d.%02d.%04d %02d:%02d:%02d.%03d", tl.wDay, tl.wMonth, tl.wYear, tl.wHour, tl.wMinute, tl.wSecond, tl.wMilliseconds);
    return buf.data();
}

AttrChangePopup::AttrChangePopup(SHORT w, SHORT h)
    : w(w), h(h)
{
}

void AttrChangePopup::show(const std::wstring& fileRoot, const std::wstring& fileName) {
    visible = true;
    root = fileRoot;
    name = fileName;
    std::wstring path = root + L"\\" + name;

    DWORD attr = GetFileAttributesW(path.c_str());
    badAttr = attr == INVALID_FILE_ATTRIBUTES;
    if (badAttr) {
        oldAttr = 0;
    } else {
        oldAttr = attr;
    }
    newAttr = oldAttr;

    badTime = true;
    HANDLE file = CreateFileW(path.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        badTime = GetFileTime(file, &creationTime, &accessTime, &modifyTime) == 0;
        CloseHandle(file);
    }

    updateAttr();
}

void AttrChangePopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        // TODO change attributes here
        visible = false;
    });
    screen.handleKey(this, VK_SPACE, 0, [this]() {
        int selected = attrText.getSelectedIdx();
        if (!badAttr && selected < 4) {
            newAttr ^= ATTRS[selected];
            updateAttr();
        }
    });
    screen.handleKey(this, VK_UP, 0, [this]() {
        attrText.selectPrev();
    });
    screen.handleKey(this, VK_DOWN, 0, [this]() {
        attrText.selectNext();
    });
}

void AttrChangePopup::drawOn(Screen& screen) {
    if (!visible) {
        return;
    }
    auto center = screen.center();
    Rect rect{(SHORT)(center.X - w/2), (SHORT)(center.Y - h/2), w, h};
    Rect shadow(rect.moved(2, 1));
    screen.paintRect(shadow, FG::DARK_GREY | BG::BLACK, false);
    screen.paintRect(rect, FG::BLACK | BG::GREY);

    Rect frameRect = rect.withPadding(2, 1);
    screen.frame(frameRect);

    Rect inner = frameRect.withPadding(2, 2);
    std::wstring type = (oldAttr & FILE_ATTRIBUTE_DIRECTORY) ? L"папки" : L"файла";
    screen.textOut(inner.getLeftTop(), L"Атрибуты " + type + L" " + name + L":");
    attrText.drawOn(screen, inner.moved(0, 1));

    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> OK ", L" <Esc> Отмена "}, FG::BLACK | BG::DARK_CYAN);
}

void AttrChangePopup::updateAttr() {
    std::vector<std::wstring> lines;
    if (badAttr) {
        lines.push_back(L"  - ошибка при получении атрибутов -");
        lines.push_back(L"  - ошибка при получении атрибутов -");
        lines.push_back(L"  - ошибка при получении атрибутов -");
        lines.push_back(L"  - ошибка при получении атрибутов -");
    } else {
        for (int i = 0; i < 4; ++i) {
            std::wstring line;
            if ((newAttr & ATTRS[i]) != (oldAttr & ATTRS[i])) {
                line += L" *";
            } else {
                line += L"  ";
            }
            if (newAttr & ATTRS[i]) {
                line += L"[x] ";
            } else {
                line += L"[ ] ";
            }
            line += NAMES[i];
            lines.push_back(std::move(line));
        }
    }
    if (badTime) {
        lines.push_back(L"Дата создания:     - не удалось получить -");
        lines.push_back(L"Дата доступа:      - не удалось получить -");
        lines.push_back(L"Дата изменения:    - не удалось получить -");
    } else {
        lines.push_back(L"Дата создания:     " + getTimeStr(creationTime));
        lines.push_back(L"Дата доступа:      " + getTimeStr(accessTime));
        lines.push_back(L"Дата изменения:    " + getTimeStr(modifyTime));
    }
    attrText.setLines(styledText(std::move(lines), FG::BLACK | BG::GREY, FG::BLACK | BG::DARK_CYAN));
    if (!attrText.hasSelection()) {
        attrText.setSelectedIdx(0);
    }
    if (badAttr || badTime) {
        attrText.unselect();
    }
}
