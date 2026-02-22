#pragma once
#include "display_record.hpp"
#include <vector>
#include <string>

class Display {
public:
    Display(std::vector<DisplayRecord> *records);
    ~Display();
    void refresh();
    void handleInput(int ch);

private:
    std::vector<DisplayRecord> *records_;
    unsigned int scrollOffset_ = 0;
    unsigned int selectedRow_ = 0;
    unsigned int height_ = 0;
    unsigned int width_ = 0;

    unsigned long timeDeltaMax_ = 99999999LU;
    int timeDeltaSize_;

    void drawHeader();
    void drawList();
    void drawFooter();
    std::string welcomeMessage = "---== Can bus analyzer ==---";
    std::string headerString = "  t delta   |  identifier  | RTR | body";
};
