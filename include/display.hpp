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
    void newRecordInform(unsigned int position);

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
    std::string headerString = "  t delta   |  identifier  | RTR | body";

    void drawHeader();
    void drawList();
    void drawFooter();
};
