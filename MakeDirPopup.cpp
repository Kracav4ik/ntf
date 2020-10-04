#include "MakeDirPopup.h"

#include "Screen.h"
#include "colors.h"
#include "MessagePopup.h"

MakeDirPopup::MakeDirPopup(Screen& screen, SHORT w, SHORT h)
    : w(w)
    , h(h)
    , newName(screen, this, w - 8)
{
}

void MakeDirPopup::setOnUpdateDirs(std::function<void()> func) {
    updateDirs = std::move(func);
}

void MakeDirPopup::show(const std::wstring& root) {
    visible = true;
    dirRoot = root;
    newName.setText(L"Новая папка");
}

void MakeDirPopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        // TODO make dir here
        std::wstring name = newName.getText();
        if (name.empty()) {
            return;
        }
        std::wstring path = dirRoot + L"\\" + name;

        if(CreateDirectoryW(path.c_str(), nullptr) == 0) {
            DWORD lastError = GetLastError();
            switch(lastError) {
                case ERROR_ALREADY_EXISTS:
                    MessagePopup::show({L"Папка уже существует"});
                    return;
                case ERROR_PATH_NOT_FOUND:
                    MessagePopup::show({L"Путь не найден"});
                    return;
                default:
                    MessagePopup::show({L"Ошибка " + std::to_wstring(lastError)});
                    return;
            }
        }
        updateDirs();
        visible = false;
    });
}

void MakeDirPopup::drawOn(Screen& screen) {
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
    screen.textOut(inner.getLeftTop(), L"Создать папку:");
    newName.drawOn(screen, inner.moved(0, 1).getLeftTop(), FG::BLACK | BG::DARK_CYAN);

    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> OK ", L" <Esc> Отмена "}, FG::BLACK | BG::DARK_CYAN);
}
