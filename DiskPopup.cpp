#include "DiskPopup.h"

#include "Screen.h"
#include "colors.h"

#include <iostream>
#include <sstream>
#include <iomanip>

static const SHORT INFO_LINES = 4;

std::wstring getDriveType(const std::wstring& path) {
    switch (GetDriveTypeW(path.c_str())) {
        case DRIVE_NO_ROOT_DIR: return L"Путь не является диском";
        case DRIVE_REMOVABLE: return L"Сменный диск";
        case DRIVE_FIXED: return L"Жесткий диск";
        case DRIVE_REMOTE: return L"Сетевой диск";
        case DRIVE_CDROM: return L"CD-ROM";
        case DRIVE_RAMDISK: return L"RAM-диск";
        default: return L"Неизвестный";
    }
}

std::wstring to_hex(DWORD num) {
    std::wstringstream s;
    s << std::setfill(L'0') << std::setw(2*sizeof(num)) << std::hex;
    s << num;
    return s.str();
}

std::vector<std::wstring> getDriveString() {
    DWORD size = GetLogicalDriveStringsW(0, nullptr);
    std::vector<wchar_t> chars(size);
    GetLogicalDriveStringsW(size, chars.data());

    std::vector<std::wstring> result;

    int start = 0;
    for (int i = start; i < size - 1; ++i) {
        if (chars[i] == L'\0') {
            std::wstring rootName = chars.data() + start;
            std::vector<wchar_t> volumeInfo(MAX_PATH + 1);
            std::vector<wchar_t> fileSysName(MAX_PATH + 1);
            DWORD serialNum;
            DWORD maxCompLen;
            DWORD sysFlags;

            GetVolumeInformationW(rootName.data(), volumeInfo.data(), volumeInfo.size(), &serialNum, &maxCompLen, &sysFlags, fileSysName.data(), fileSysName.size());

            /*
            std::wcout << L"rootName: " << rootName << std::endl;
            std::wcout << L"  volumeInfo: " << volumeInfo.data() << std::endl;
            std::wcout << L"  fileSysName: " << fileSysName.data() << std::endl;
            std::wcout << L"  serialNum: " << serialNum << std::endl;
            std::wcout << L"  maxCompLen: " << maxCompLen << std::endl;
            std::wcout << L"  sysFlags: " << sysFlags << std::endl;
            

            if (sysFlags & FILE_CASE_PRESERVED_NAMES) std::wcout << L"    FILE_CASE_PRESERVED_NAMES" << std::endl;
            if (sysFlags & FILE_CASE_SENSITIVE_SEARCH) std::wcout << L"    FILE_CASE_SENSITIVE_SEARCH" << std::endl;
            if (sysFlags & FILE_FILE_COMPRESSION) std::wcout << L"    FILE_FILE_COMPRESSION" << std::endl;
            if (sysFlags & FILE_NAMED_STREAMS) std::wcout << L"    FILE_NAMED_STREAMS" << std::endl;
            if (sysFlags & FILE_PERSISTENT_ACLS) std::wcout << L"    FILE_PERSISTENT_ACLS" << std::endl;
            if (sysFlags & FILE_READ_ONLY_VOLUME) std::wcout << L"    FILE_READ_ONLY_VOLUME" << std::endl;
            if (sysFlags & FILE_SEQUENTIAL_WRITE_ONCE) std::wcout << L"    FILE_SEQUENTIAL_WRITE_ONCE" << std::endl;
            if (sysFlags & FILE_SUPPORTS_ENCRYPTION) std::wcout << L"    FILE_SUPPORTS_ENCRYPTION" << std::endl;
            if (sysFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES) std::wcout << L"    FILE_SUPPORTS_EXTENDED_ATTRIBUTES" << std::endl;
            if (sysFlags & FILE_SUPPORTS_HARD_LINKS) std::wcout << L"    FILE_SUPPORTS_HARD_LINKS" << std::endl;
            if (sysFlags & FILE_SUPPORTS_OBJECT_IDS) std::wcout << L"    FILE_SUPPORTS_OBJECT_IDS" << std::endl;
            if (sysFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID) std::wcout << L"    FILE_SUPPORTS_OPEN_BY_FILE_ID" << std::endl;
            if (sysFlags & FILE_SUPPORTS_REPARSE_POINTS) std::wcout << L"    FILE_SUPPORTS_REPARSE_POINTS" << std::endl;
            if (sysFlags & FILE_SUPPORTS_SPARSE_FILES) std::wcout << L"    FILE_SUPPORTS_SPARSE_FILES" << std::endl;
            if (sysFlags & FILE_SUPPORTS_TRANSACTIONS) std::wcout << L"    FILE_SUPPORTS_TRANSACTIONS" << std::endl;
            if (sysFlags & FILE_SUPPORTS_USN_JOURNAL) std::wcout << L"    FILE_SUPPORTS_USN_JOURNAL" << std::endl;
            if (sysFlags & FILE_UNICODE_ON_DISK) std::wcout << L"    FILE_UNICODE_ON_DISK" << std::endl;
            if (sysFlags & FILE_VOLUME_IS_COMPRESSED) std::wcout << L"    FILE_VOLUME_IS_COMPRESSED" << std::endl;
            if (sysFlags & FILE_VOLUME_QUOTAS) std::wcout << L"    FILE_VOLUME_QUOTAS" << std::endl;
            //*/

            result.emplace_back(rootName.substr(0, 2) + L" " + getDriveType(rootName));
            start = i + 1;
        }
    }

    return result;
}

DiskPopup::DiskPopup(SHORT xLeft, SHORT xRight, SHORT y, SHORT w, SHORT h)
    : xLeft(xLeft)
    , xRight(xRight)
    , y(y)
    , w(w)
    , h(h)
{
    GetLogicalDrives();
    diskList.setLines(styledText(getDriveString(), FG::WHITE | BG::DARK_CYAN, FG::WHITE | BG::BLACK));
    diskList.setSelectedIdx(0);

    updateDiskInfo();
}

void DiskPopup::updateDiskInfo() {
    std::wstring rootName = selectedDisk() + L"\\";  // needed for FS name

    std::vector<wchar_t> volumeInfo(MAX_PATH + 1);
    std::vector<wchar_t> fileSysName(MAX_PATH + 1);
    DWORD serialNum;
    DWORD maxCompLen;
    DWORD sysFlags;

    GetVolumeInformationW(rootName.data(), volumeInfo.data(), volumeInfo.size(), &serialNum, &maxCompLen, &sysFlags, fileSysName.data(), fileSysName.size());

    std::wstring volumeStr = volumeInfo.data();
    if (volumeStr.empty()) {
        volumeStr = getDriveType(rootName) + L" " + rootName.substr(0, 2);
    }
    std::wstring fileSysStr = fileSysName.data();

    diskInfo.setLines(styledText({
        L"Название тома: " + volumeStr,
        L"Файловая система: " + fileSysStr,
        L"Серийный номер: " + to_hex(serialNum),
        L"Флаги файловой системы: " + to_hex(sysFlags),
    }, FG::WHITE | BG::DARK_CYAN, FG::WHITE | BG::BLACK));
}

bool DiskPopup::isLeftPopup() const {
    return isLeft;
}

std::wstring DiskPopup::selectedDisk() const {
    return diskList.getSelectedText().substr(0, 2);
}

void DiskPopup::registerKeys(Screen& screen) {
    screen.tryHandleKey(VK_F1, ANY_ALT_PRESSED, [this]() {
        if (isVisible) {
            return EventState::Unhandled;
        }
        show(true);
        return EventState::Handled;
    });
    screen.tryHandleKey(VK_F2, ANY_ALT_PRESSED, [this]() {
        if (isVisible) {
            return EventState::Unhandled;
        }
        show(false);
        return EventState::Handled;
    });
    screen.tryHandleKey(VK_UP, 0, [this]() {
        if (!isVisible) {
            return EventState::Unhandled;
        }
        diskList.selectPrev();
        diskList.scrollToSelection(diskListRect().h);
        updateDiskInfo();
        return EventState::Handled;
    });
    screen.tryHandleKey(VK_DOWN, 0, [this]() {
        if (!isVisible) {
            return EventState::Unhandled;
        }
        diskList.selectNext();
        diskList.scrollToSelection(diskListRect().h);
        updateDiskInfo();
        return EventState::Handled;
    });
    screen.tryHandleKey(VK_ESCAPE, 0, [this]() {
        if (!isVisible) {
            return EventState::Unhandled;
        }
        hide();
        return EventState::Handled;
    });
    screen.tryHandleKey(VK_RETURN, 0, [this]() {
        if (!isVisible) {
            return EventState::Unhandled;
        }
        if (selectDisk) {
            selectDisk();
            hide();
        }
        return EventState::Handled;
    });
}

void DiskPopup::setOnSelectFunc(std::function<void()> func) {
    selectDisk = std::move(func);
}

void DiskPopup::drawOn(Screen& screen) {
    if (!isVisible) {
        return;
    }
    Rect rect = popupRect();
    Rect shadow(rect.moved(2, 1));
    screen.paintRect(shadow, FG::DARK_GREY | BG::BLACK, false);
    screen.paintRect(rect, FG::WHITE | BG::DARK_CYAN);

    screen.frame(frameRect());

    diskList.drawOn(screen, diskListRect());
    screen.separator(diskInfoRect().moved(0, -1).withPadX(-1).withH(1));
    diskInfo.drawOn(screen, diskInfoRect());
}

Rect DiskPopup::popupRect() const {
    SHORT x = isLeft ? xLeft : xRight;
    return {x, y, w, h};
}

Rect DiskPopup::frameRect() const {
    return popupRect().withPadding(2, 1);
}

Rect DiskPopup::innerRect() const {
    return frameRect().withPadding(1, 1);
}

Rect DiskPopup::diskListRect() const {
    Rect inner = innerRect();
    return inner.withH(inner.h - INFO_LINES - 1);
}

Rect DiskPopup::diskInfoRect() const {
    Rect listRect = diskListRect();
    return listRect.moved(0, listRect.h + 1).withH(INFO_LINES);
}

void DiskPopup::show(bool left) {
    isLeft = left;
    isVisible = true;
}

void DiskPopup::hide() {
    isVisible = false;
}
