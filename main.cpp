#include "Lines.h"
#include "Screen.h"

#include "colors.h"

int main() {
    Screen s(80, 25);
    s.setTitle(L"Not too far");
    s.setCursorVisible(false);


    Rect r1{0, 0, 40, 23};
    Rect r2{40, 0, 40, 23};
    Lines left({L"Осторожно слева!"});
    Lines right({L"Осторожно справа!"});
    Lines bottom({L"Хочется глубокомысленную мысль, но ее нет :("});
    left.setSelectedIdx(0);
//    Rect r1{10, 2, 20, 5};
//    Lines l1({L"Осторожно!"});

//    Lines l2({L"Очень длинный текст"});
//
//    Lines l3({L"smol"});
//
//    Rect r4{13, 15, 14, 3};
//    Lines l4({L"pad pad pad"}, 4);
//
//    Rect r5{40, 2, 20, 8};
//    Lines l5({L"Много линий", L"Есть как очень длинные", L"Так и", L"Короткие", L"А также", L"Пара", L"За экраном"}, 4, 2);
//    l5.setSelectedIdx(0);
//
//    Rect r6{40, 12, 24, 10};
//    Lines l6({L"Много линий", L"Есть как очень длинные", L"Так и", L"Короткие"}, 2);
//    l6.setSelectedIdx(0);

    auto repaint0 = [&]() {
        s.clear(FG::GREY | BG::BLACK);

//        l1.drawOn(s, r1, FG::WHITE | BG::RED, 0, true);
//        s.frame(r1);
//
//        l2.drawOn(s, {11, 9, 18, 1}, FG::WHITE | BG::BLUE);
//
//        l3.drawOn(s, {19, 12, 2, 1}, FG::GREEN | BG::DARK_GREEN);
//
//        l4.drawOn(s, r4, FG::YELLOW | BG::DARK_CYAN, 0, true);
//
//        l5.drawOn(s, r5, FG::YELLOW | BG::DARK_YELLOW, FG::CYAN | BG::DARK_CYAN);
//        s.frame(r5, true);
//        s.frame(r5.withPadding(1, 1), false);
//
//        l6.drawOn(s, r6, FG::RED | BG::DARK_MAGENTA, FG::CYAN | BG::DARK_CYAN, true);
//        s.frame(r6.withPadding(1, 1), false);

        s.flip();
    };
    auto repaint = [&]() {
        s.clear(FG::GREY | BG::BLACK);

        s.paintRect(r1, FG::CYAN | BG::DARK_BLUE);
        left.drawOn(s, r1.withPadding(1, 1), FG::WHITE | BG::DARK_BLUE, FG::BLACK | BG::DARK_CYAN);
        s.frame(r1);

        s.paintRect(r2, FG::CYAN | BG::DARK_BLUE);
        right.drawOn(s, r2.withPadding(1, 1), FG::WHITE | BG::DARK_BLUE, FG::BLACK | BG::DARK_CYAN);
        s.frame(r2);

        bottom.drawTextOn(s, {0, 23, 80, 1});

        s.flip();
    };

    bool running = true;
    s.handleKey(VK_F10, 0, [&]() {
        running = false;
    });
    s.handleKey(VK_TAB, 0, [&]() {
        left.hasSelection() ? left.unselect() : left.setSelectedIdx(0);
        right.hasSelection() ? right.unselect() : right.setSelectedIdx(0);
    });
    s.handleKey(' ', 0, [&]() {
//        l5.setSelectedIdx(0);
//        l5.scrollToSelection(4);
//        l6.setSelectedIdx(0);
    });
    s.handleKey(VK_UP, 0, [&]() {
//        l5.selectPrev();
//        l5.scrollToSelection(4);
//        l6.selectPrev();
    });
    s.handleKey(VK_DOWN, 0, [&]() {
//        l5.selectNext();
//        l5.scrollToSelection(4);
//        l6.selectNext();
    });

    repaint();
    while (running) {
        s.processEvent();
        repaint();
    }
}
