#pragma once
#include "display_record.hpp"
#include <vector>
#include <string>

class Display {
public:
    Display(std::vector<DisplayRecord> *records);
    ~Display();
    void handleInput(int ch);
    void redraw();
    void changeInform(RecordChange change);

private:
    std::vector<DisplayRecord> *records_;
    unsigned int scrollOffset_ = 0;
    unsigned int selectedRow_ = 0;
    unsigned int height_;
    unsigned int width_;
    unsigned int visibleRecords_;
    unsigned long timeDeltaMax_ = 99999999LU;
    int timeDeltaSize_;
    std::string welcomeMessage = "---== Can bus analyzer ==---";
    std::string headerString = "  identifier  | RTR | body";

    void drawRecords(unsigned int startInd, unsigned int endInd);
    void drawHeader();
    void drawFooter();
};
