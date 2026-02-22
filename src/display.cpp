#include <ncurses.h>
#include <cstdlib>
#include "display.hpp"

Display::Display(std::vector<DisplayRecord> *records) {
    initscr();
    timeout(0);
    raw();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, height_, width_);
    records_ = records;
    char buf[32];
    timeDeltaSize_ = snprintf(buf, sizeof(buf), "%lu", timeDeltaMax_);
    visibleRecords_ = height_ - 3; // two for header, one for footer
}

Display::~Display() {
    endwin();
}

void Display::drawHeader() {
    attron(A_REVERSE);
    mvprintw(0, width_/2 - size(welcomeMessage)/2, "%s", welcomeMessage.c_str());
    attroff(A_REVERSE);
    mvprintw(1, 0, "%s", headerString.c_str());
}

void Display::drawList() {
    unsigned int currY = 2;
    while (currY < height_ - 1 && scrollOffset_ + currY - 2 < records_->size()) {
        DisplayRecord curr = (*records_)[scrollOffset_ + currY - 2];
        unsigned long tDelta = std::min(timeDeltaMax_, curr.timeDelta);
        if (currY - 2 == selectedRow_) {
            attron(A_REVERSE);
        }
        mvprintw(
            currY,
            0,
            "  %*lu  |   %08lX   |  %d  | ",
            timeDeltaSize_,
            tDelta,
            curr.msg.identifier,
            curr.msg.isRtr & 1
        );
        for (int i = 0; i < curr.msg.dlc; i++) {
            printw("%02X ", curr.msg.data[i]);
        }

        if (currY - 2 == selectedRow_) {
            attroff(A_REVERSE);
        }
        currY++;
    }
}

void Display::drawFooter() {
    mvprintw(height_-1, 0, "Row: %u  |  Scroll: %u", selectedRow_, scrollOffset_);
}


void Display::refresh() {
    clear();
    getmaxyx(stdscr, height_, width_);

    drawHeader();
    drawList();
    drawFooter();

    ::refresh();
}

void Display::handleInput(int ch) {
    switch (ch) {
        case 113: // q
            std::exit(0);
            break;

        case KEY_UP:
        case 107: // k
            if (selectedRow_ == 0 && scrollOffset_ > 0)
                scrollOffset_--;
            else if (selectedRow_ != 0)
                selectedRow_--;
            break;

        case KEY_DOWN:
        case 106: // j
            if (selectedRow_ + scrollOffset_ >= records_->size() - 1)
                break;

            if (selectedRow_ < visibleRecords_ - 1)
                selectedRow_++;
            else 
                scrollOffset_++;
            break;

        case 103: // g
            selectedRow_ = 0;
            scrollOffset_ = 0;
            break;

        case 71: // G
            if (records_->size() == 0) break;
            selectedRow_ = std::min(visibleRecords_-1, static_cast<unsigned int>(records_->size()-1));
            scrollOffset_ = records_->size() - 1 - selectedRow_;
            break;

        case 99: // c
            records_->clear();
            selectedRow_ = 0;
            scrollOffset_ = 0;
            break;
    }
}

void Display::newRecordInform(unsigned int position) {
    if (position <= selectedRow_ + scrollOffset_) {
        if (records_->size() > visibleRecords_)
            scrollOffset_++;
        else
            selectedRow_++;
    }
}
