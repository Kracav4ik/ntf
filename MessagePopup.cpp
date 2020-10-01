#include "MessagePopup.h"

#include "Screen.h"
#include "colors.h"

static const SHORT OK_WIDTH = 8;

void MessagePopup::show(std::vector<std::wstring> text) {
    MessagePopup& popup = get();
    popup.maxWidth = 0;
    for (const auto& line : text) {
        popup.maxWidth = std::max(popup.maxWidth, (int)line.size());
    }
    popup.linesCount = text.size();
    popup.lines.setLines(styledText(std::move(text), FG::WHITE | BG::DARK_RED));
    popup.isVisible = true;
}

void MessagePopup::registerKeys(Screen& screen) {
    // TODO: make modal
    screen.tryHandleKey(VK_RETURN, 0, []() {
        MessagePopup& popup = get();
        if (!popup.isVisible) {
            return EventState::Unhandled;
        }
        popup.isVisible = false;
        return EventState::Handled;
    });
}

void MessagePopup::drawOn(Screen& screen) {
    MessagePopup& popup = get();
    if (!popup.isVisible) {
        return;
    }
    SHORT w = std::max(OK_WIDTH, (SHORT) popup.maxWidth);
    SHORT h = popup.linesCount + 2;
    auto center = screen.center();

    Rect inner{center.X, center.Y, w, h};
    Rect button = inner.moved(-OK_WIDTH/2, h - h/2 - 1).withW(OK_WIDTH).withH(1);
    inner = inner.moved(-w/2, -h/2);
    Rect frame = inner.withPadding(-2, -1);
    Rect outer = frame.withPadding(-2, -1);
    Rect shadow(outer.moved(2, 1));

    screen.paintRect(shadow, FG::DARK_GREY | BG::BLACK, false);
    screen.paintRect(outer, FG::GREY | BG::DARK_RED);
    screen.frame(frame);

    popup.lines.drawOn(screen, inner.withH(inner.h - 2), true);
    screen.separator(inner.moved(0, inner.h - 2).withPadX(-2).withH(1));

    screen.paintRect(button, FG::BLACK | BG::GREY);
    screen.textOut({(SHORT)(center.X - 1), button.y}, L"OK");
}

MessagePopup& MessagePopup::get() {
    static MessagePopup popup;
    return popup;
}
