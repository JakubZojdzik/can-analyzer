#include <termios.h>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include "display.hpp"
#include "uart_transporter.hpp"
#include "mock_transporter.hpp"
#include "display_record.hpp"

std::mutex recordsMutex;

void handleFrame(std::vector<DisplayRecord> &records, CANMessage &msg, Display &d) {
    auto now = std::chrono::steady_clock::now();
    if (records.empty()) {
        records.push_back({msg, now, 0, 0xff});
        d.changeInform(RecordChange(0, true));
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
        if (records[start].msg.dlc == msg.dlc && !std::memcmp(records[start].msg.data, msg.data, msg.dlc)) {
            return;
        }
        records[start].msg = msg;
        d.changeInform(RecordChange(start, false));
    } else {
        DisplayRecord newRecord{msg, now, 0, 0xff};
        if (records[start].msg.identifier < msg.identifier)
            start++;
        auto insertPos = records.begin() + start;
        records.insert(insertPos, newRecord);
        d.changeInform(RecordChange(start, true));
    }
}

void recordFrame(CANMessage msg, FILE *f) {
    double seconds = msg.timestamp / 1'000'000.0;

    std::fprintf(f,
        "%.6f; %08X; %d; ",
        seconds,
        msg.identifier,
        msg.isRtr & 1
    );

    for (int i = 0; i < msg.dlc; i++) {
        std::fprintf(f, "%02X ", msg.data[i]);
    }
    std::fprintf(f, "\n");
    std::fflush(f);
}


void receiverThread(Transporter &transp, std::vector<DisplayRecord> &records, Display &d, FILE *recording) {
    CANMessage msg;
    while(true) {
        transp.receive(msg);
        recordsMutex.lock();
        handleFrame(records, msg, d);
        if (recording)
            recordFrame(msg, recording);
        recordsMutex.unlock();
    }
}

int main(int argc, char *argv[]) {
    FILE *recording = NULL;
    if (argc >= 2) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            printf(
                "CAN Analyzer\n"
                "-r | --record <path> saves all frame recordings into file located in <path>\n"
                "Nawigation:\n"
                "  UP    - k, up arrow\n"
                "  DOWN  - j, down arrow\n"
                "  BEGIN - g\n"
                "  END   - G\n"
                "Other:\n"
                "  clear - c\n"
                "  copy  - y\n"
                "  exit  - q\n"
            );
            return 0;
        } else if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "--record")) {
            if (argc != 3) {
                printf("Invalid --recording flag usage\n");
            }
            recording = std::fopen(argv[2], "w");
            if (!recording) {
                std::fprintf(stderr, "Failed to open %s: %s\n", argv[2], std::strerror(errno));
                return 1;
            }
        } else {
            printf("Invalid cli option %s\n", argv[1]);
            return 0;
        }
    }

    unsigned long baudRate = B921600;
    // UARTTransporter transp("/dev/ttyUSB1", baudRate);
    MockTransporter transp(50, true);

    std::vector<DisplayRecord> records;
    records.reserve(100);
    Display display(&records);
    std::thread receiver(receiverThread, std::ref(transp), std::ref(records), std::ref(display), recording);

    while(true) {
        int ch = getchar();
        recordsMutex.lock();
        if (display.handleInput(ch)) {
            if (recording)
                fclose(recording);
            std::exit(0);
        }
        display.redraw();
        recordsMutex.unlock();
    }
}
