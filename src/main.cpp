#include <termios.h>
#include <vector>
#include "mock_transporter.hpp"
#include "display_record.hpp"
#include "display.hpp"

void handleFrame(std::vector<DisplayRecord> &records, CANMessage &msg, Display &d) {
    auto now = std::chrono::steady_clock::now();

    if (records.empty()) {
        records.push_back({msg, now, 0, 0xff});
        return;
    }

    int start = 0, end = static_cast<int>(records.size());
    while (start + 1 < end) {
        int mid = (start + end) / 2;
        if (records[mid].msg.identifier <= msg.identifier)
            start = mid;
        else
            end = mid;
    }

    if (records[start].msg.identifier == msg.identifier) {
        auto prev = records[start].timestamp;
        records[start].timestamp = now;
        records[start].timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
        records[start].msg = msg;
    } else {
        DisplayRecord newRecord{msg, now, 0, 0xff};
        if (records[start].msg.identifier < msg.identifier)
            start++;
        auto insertPos = records.begin() + start;
        records.insert(insertPos, newRecord);
        d.newRecordInform(start);
    }
}

int main() {
    // unsigned long baudRate = B921600;
    MockTransporter transp(50, true);
    CANMessage msg;
    std::vector<DisplayRecord> records;
    records.reserve(100);
    Display display(&records);

    while(true) {
        transp.receive(msg);
        handleFrame(records, msg, display);
        int ch = getch();
        display.handleInput(ch);
        display.refresh();
    }
}
