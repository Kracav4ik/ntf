#include "MakeFilePopup.h"

#include "Screen.h"
#include "colors.h"
#include "MessagePopup.h"
#include "utils.h"

MakeFilePopup::MakeFilePopup(Screen& screen, SHORT w, SHORT h)\
    : w(w)
    , h(h)
    , newName(screen, this, w - 8)
{
}

void MakeFilePopup::setOnUpdateDirs(std::function<void()> func) {
    updateDirs = std::move(func);
}

void MakeFilePopup::show(const std::wstring& root) {
    visible = true;
    dirRoot = root;
    newName.setText(L"Новый файл");
}

void MakeFilePopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        std::wstring name = newName.getText();
        if (name.empty()) {
            return;
        }
        std::wstring path = dirRoot + L"\\" + name;

        HANDLE created = CreateFileW(path.c_str(), 0, 0, nullptr, CREATE_NEW, 0, nullptr);
        if(created == INVALID_HANDLE_VALUE) {
            MessagePopup::show({L"Ошибка создания файла:", getLastErrorText()});
            return;
        }
        CloseHandle(created);
        updateDirs();
        visible = false;
    });

}

void MakeFilePopup::drawOn(Screen& screen) {
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
    screen.textOut(inner.getLeftTop(), L"Создать файл:");
    newName.drawOn(screen, inner.moved(0, 1).getLeftTop(), FG::BLACK | BG::DARK_CYAN);

    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> OK ", L" <Esc> Отмена "}, FG::BLACK | BG::DARK_CYAN);
}
