#include <ncurses.h>
#include <cstdlib>
#include <cstdio>
#include "display.hpp"

Display::Display(std::vector<DisplayRecord> *records) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, height_, width_);
    records_ = records;
    char buf[32];
    timeDeltaSize_ = snprintf(buf, sizeof(buf), "%lu", timeDeltaMax_);
    visibleRecords_ = height_ - 3; // two for header, one for footer
    drawHeader();
    drawFooter();
    refresh();
}

Display::~Display() {
    endwin();
}

void Display::drawHeader() {
    attron(A_REVERSE);
    attron(A_BOLD);
    mvprintw(0, width_/2 - size(welcomeMessage)/2, "%s", welcomeMessage.c_str());
    attroff(A_REVERSE);
    attroff(A_BOLD);
    mvprintw(1, 0, "%s", headerString.c_str());
}


void Display::drawFooter() {
    mvprintw(height_-1, 0, "Row: %u  |  Scroll: %u", selectedRow_, scrollOffset_);
}


void Display::drawRecords(unsigned int startInd, unsigned int endInd) {
    unsigned int i = std::max(startInd, scrollOffset_);
    unsigned int currY = i - scrollOffset_;
    while (i <= endInd && currY < height_ - 3 && scrollOffset_ + currY < records_->size()) {
        DisplayRecord curr = (*records_)[i];
        move(currY + 2, 0);
        clrtoeol();
        if (currY == selectedRow_) {
            attron(A_REVERSE);
        }
        printw(
            "   %08lX   |  %d  | ",
            curr.msg.identifier,
            curr.msg.isRtr & 1
        );
        for (int i = 0; i < curr.msg.dlc; i++) {
            printw("%02X ", curr.msg.data[i]);
        }

        if (currY == selectedRow_) {
            attroff(A_REVERSE);
        }
        currY++;
        i++;
    }
    refresh();
}

void Display::changeInform(RecordChange change) {
    if (change.isNew && change.position <= selectedRow_ + scrollOffset_) {
        if (records_->size() > visibleRecords_)
            scrollOffset_++;
        else
            selectedRow_++;
    }

    if (change.position < scrollOffset_ || change.position >= scrollOffset_ + visibleRecords_)
        return;

    if (change.isNew) {
        drawRecords(change.position, records_->size());
    } else {
        drawRecords(change.position, change.position);
    }
}

void Display::redraw() {
    drawRecords(0, records_->size());
}

void Display::handleInput(int ch) {
    switch (ch) {
        case 113: // q
            endwin();
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

