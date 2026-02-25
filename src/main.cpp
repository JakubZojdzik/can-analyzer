#include <termios.h>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore>
#include "display.hpp"
#include "uart_transporter.hpp"
#include "mock_transporter.hpp"
#include "display_record.hpp"

std::mutex recordsMutex;
std::binary_semaphore refreshDisplay(0);

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

void receiverThread(Transporter &transp, std::vector<DisplayRecord> &records, Display &d) {
    CANMessage msg;
    while(true) {
        transp.receive(msg);
        printf("%lx", msg.identifier);
        recordsMutex.lock();
        handleFrame(records, msg, d);
        recordsMutex.unlock();
        refreshDisplay.release();
    }
}

void drawThread(Display &d) {
    while(true) {
        refreshDisplay.acquire();
        recordsMutex.lock();
        d.refresh();
        recordsMutex.unlock();
    }
}

int main() {
    unsigned long baudRate = B921600;
    UARTTransporter transp("/dev/ttyUSB0", baudRate);

    // MockTransporter transp(50, true);
    std::vector<DisplayRecord> records;
    records.reserve(100);
    Display display(&records);
    std::thread receiver(receiverThread, std::ref(transp), std::ref(records), std::ref(display));
    std::thread drawer(drawThread, std::ref(display));

    while(true) {
        int ch = getchar();
        recordsMutex.lock();
        display.handleInput(ch);
        recordsMutex.unlock();
        refreshDisplay.release();
    }
}
