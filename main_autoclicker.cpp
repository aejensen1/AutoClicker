#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <limits>
#include <conio.h>
#include <map>

// Constants for hotkey IDs
constexpr int HOTKEY_START_STOP = 1;
constexpr int HOTKEY_EXIT = 2;
constexpr int HOTKEY_INCREASE_SPEED = 3;
constexpr int HOTKEY_DECREASE_SPEED = 4;
constexpr int HOTKEY_SET_SPEED = 5;
#define MOD_NOREPEAT 0x4000

std::atomic<bool> running(false);
std::atomic<bool> program_running(true);
std::atomic<double> delayTime(0.001);  // Initial delay between clicks (in seconds)
std::atomic<bool> openMenu(false);
std::atomic<bool> pauseThreads(false);
std::atomic<DWORD> clickerButton(MOUSEEVENTF_LEFTDOWN);  // Default to left mouse button

// Map to store key names
std::map<int, std::string> keyNameMap = {
    {VK_LBUTTON, "Left Mouse Button"},
    {VK_RBUTTON, "Right Mouse Button"},
    {VK_MBUTTON, "Middle Mouse Button"},
    {VK_CANCEL, "Control-break processing"},
    {VK_BACK, "Backspace"},
    {VK_TAB, "Tab"},
    {VK_CLEAR, "Clear"},
    {VK_RETURN, "Enter"},
    {VK_SHIFT, "Shift"},
    {VK_CONTROL, "Ctrl"},
    {VK_MENU, "Alt"},
    {VK_PAUSE, "Pause"},
    {VK_CAPITAL, "Caps Lock"},
    {VK_ESCAPE, "Esc"},
    {VK_SPACE, "Spacebar"},
    {VK_PRIOR, "Page Up"},
    {VK_NEXT, "Page Down"},
    {VK_END, "End"},
    {VK_HOME, "Home"},
    {VK_LEFT, "Left Arrow"},
    {VK_UP, "Up Arrow"},
    {VK_RIGHT, "Right Arrow"},
    {VK_DOWN, "Down Arrow"},
    {VK_SELECT, "Select"},
    {VK_PRINT, "Print"},
    {VK_EXECUTE, "Execute"},
    {VK_SNAPSHOT, "Print Screen"},
    {VK_INSERT, "Insert"},
    {VK_DELETE, "Delete"},
    {VK_HELP, "Help"},
    {VK_NUMPAD0, "Numpad 0"},
    {VK_NUMPAD1, "Numpad 1"},
    {VK_NUMPAD2, "Numpad 2"},
    {VK_NUMPAD3, "Numpad 3"},
    {VK_NUMPAD4, "Numpad 4"},
    {VK_NUMPAD5, "Numpad 5"},
    {VK_NUMPAD6, "Numpad 6"},
    {VK_NUMPAD7, "Numpad 7"},
    {VK_NUMPAD8, "Numpad 8"},
    {VK_NUMPAD9, "Numpad 9"},
    {VK_MULTIPLY, "Multiply"},
    {VK_ADD, "Add"},
    {VK_SEPARATOR, "Separator"},
    {VK_SUBTRACT, "Subtract"},
    {VK_DECIMAL, "Decimal"},
    {VK_DIVIDE, "Divide"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F4, "F4"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {VK_F13, "F13"},
    {VK_F14, "F14"},
    {VK_F15, "F15"},
    {VK_F16, "F16"},
    {VK_F17, "F17"},
    {VK_F18, "F18"},
    {VK_F19, "F19"},
    {VK_F20, "F20"},
    {VK_F21, "F21"},
    {VK_F22, "F22"},
    {VK_F23, "F23"},
    {VK_F24, "F24"},
    {VK_NUMLOCK, "Num Lock"},
    {VK_SCROLL, "Scroll Lock"},
    {VK_LSHIFT, "Left Shift"},
    {VK_RSHIFT, "Right Shift"},
    {VK_LCONTROL, "Left Ctrl"},
    {VK_RCONTROL, "Right Ctrl"},
    {VK_LMENU, "Left Alt"},
    {VK_RMENU, "Right Alt"},
    {48, "0"},
    {49, "1"},
    {50, "2"},
    {51, "3"},
    {52, "4"},
    {53, "5"},
    {54, "6"},
    {55, "7"},
    {56, "8"},
    {57, "9"},
    {65, "A"},
    {66, "B"},
    {67, "C"},
    {68, "D"},
    {69, "E"},
    {70, "F"},
    {71, "G"},
    {72, "H"},
    {73, "I"},
    {74, "J"},
    {75, "K"},
    {76, "L"},
    {77, "M"},
    {78, "N"},
    {79, "O"},
    {80, "P"},
    {81, "Q"},
    {82, "R"},
    {83, "S"},
    {84, "T"},
    {85, "U"},
    {86, "V"},
    {87, "W"},
    {88, "X"},
    {89, "Y"},
    {90, "Z"},
    {192, "`"},
    {189, "-"},
    {187, "="},
    {219, "["},
    {221, "]"},
    {220, "\\"},
    {186, ";"},
    {222, "'"},
    {188, ","},
    {190, "."},
    {191, "/"}
};

std::map<std::string, int> registeredKeys = {
    {"clicker", VK_LBUTTON},
    {"toggleClick", 77},
    {"increaseSpeed", VK_ADD},
    {"decreaseSpeed", VK_SUBTRACT},
    {"menu", 76}
};

void click_thread() {
    while (program_running) {
        if (pauseThreads) {
            // Wait until the threads are allowed to run again
            while (pauseThreads) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        if (running) {
            int clicker = registeredKeys["clicker"];
            if (clicker == VK_LBUTTON || clicker == VK_RBUTTON || clicker == VK_MBUTTON) {
                // Handle mouse buttons
                DWORD downEvent = (clicker == VK_LBUTTON) ? MOUSEEVENTF_LEFTDOWN :
                                  (clicker == VK_RBUTTON) ? MOUSEEVENTF_RIGHTDOWN :
                                  MOUSEEVENTF_MIDDLEDOWN;
                DWORD upEvent = (clicker == VK_LBUTTON) ? MOUSEEVENTF_LEFTUP :
                                (clicker == VK_RBUTTON) ? MOUSEEVENTF_RIGHTUP :
                                MOUSEEVENTF_MIDDLEUP;
                mouse_event(downEvent, 0, 0, 0, 0);
                mouse_event(upEvent, 0, 0, 0, 0);
            } else {
                // Handle keyboard keys
                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = clicker;
                input.ki.dwFlags = 0; // Key down
                SendInput(1, &input, sizeof(INPUT));

                input.ki.dwFlags = KEYEVENTF_KEYUP; // Key up
                SendInput(1, &input, sizeof(INPUT));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delayTime * 1000))); // Delay between clicks in milliseconds
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Reduce CPU usage
    }
}

void adjust_speed(bool increase) {
    double current = delayTime.load(); // Load the current value
    double newDelayTime;
    do {
        newDelayTime = current; // Start with the last known value
        if (increase) {
            // Ensure the new delay time is not less than the minimum allowed value
            newDelayTime = (current - 0.01 > 0.00001) ? current - 0.01 : 0.00001;
            std::cout << "Delay time decreased to " << newDelayTime << " seconds\n";
        } else {
            // Increase the delay time
            newDelayTime = current + 0.01;
            std::cout << "Delay time increased to " << newDelayTime << " seconds\n";
        }
    } while (!delayTime.compare_exchange_weak(current, newDelayTime)); // Atomically update the value
}

void set_speed(double newSpeed) {
    delayTime = newSpeed;
    std::cout << "Delay time set to " << newSpeed << " seconds\n";
}

bool register_hotkey(HWND hwnd, int id, UINT fsModifiers, UINT vk) {
    if (RegisterHotKey(hwnd, id, fsModifiers, vk)) {
        //std::cout << "Successfully registered hotkey id " << id << "\n";
        return true;
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
            std::cerr << "Hotkey id " << id << " is already registered by another application.\n";
        } else {
            std::cerr << "Failed to register hotkey id " << id << ". Error: " << error << "\n";
        }
        return false;
    }
}

bool unregister_hotkey(HWND hwnd, int id) {
    if (UnregisterHotKey(hwnd, id)) {
        //std::cout << "Successfully unregistered hotkey id " << id << "\n";
        return true;
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_HOTKEY_NOT_REGISTERED) {
            std::cerr << "Hotkey id " << id << " is not registered.\n";
        } else {
            std::cerr << "Failed to unregister hotkey id " << id << ". Error: " << error << "\n";
        }
        return false;
    }
}

void register_all_hotkeys() {
    register_hotkey(NULL, HOTKEY_START_STOP, MOD_NOREPEAT, registeredKeys["toggleClick"]); // 'M' default key
    register_hotkey(NULL, HOTKEY_EXIT, MOD_NOREPEAT, 0x4B); // 'K' default key
    register_hotkey(NULL, HOTKEY_INCREASE_SPEED, MOD_NOREPEAT, VK_OEM_PLUS); // '+' default key
    register_hotkey(NULL, HOTKEY_DECREASE_SPEED, MOD_NOREPEAT, VK_OEM_MINUS); // '-' default key
    register_hotkey(NULL, HOTKEY_SET_SPEED, MOD_NOREPEAT, registeredKeys["menu"]); // 'L' default key
}

void unregister_all_hotkeys() {
    unregister_hotkey(NULL, HOTKEY_START_STOP);
    unregister_hotkey(NULL, HOTKEY_EXIT);
    unregister_hotkey(NULL, HOTKEY_INCREASE_SPEED);
    unregister_hotkey(NULL, HOTKEY_DECREASE_SPEED);
    unregister_hotkey(NULL, HOTKEY_SET_SPEED);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN) {
            if (p->vkCode == registeredKeys["toggleClick"]) {
                running = !running;
                std::cout << (running ? "Clicking started." : "Clicking stopped.") << std::endl;
                return 1;
            } else if (p->vkCode == registeredKeys["increaseSpeed"]) {
                adjust_speed(true);
                return 1;
            } else if (p->vkCode == registeredKeys["decreaseSpeed"]) {
                adjust_speed(false);
                return 1;
            } else if (p->vkCode == registeredKeys["menu"]) {
                openMenu = true;
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void change_clicker() {
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    pauseThreads = true;
    int newClicker = -1; // Variable to store the new clicker button

    std::cout << "Current clicker: " << keyNameMap[registeredKeys["clicker"]] << std::endl;
    std::cout << "Press any key or mouse button to set a new clicker.\n";

    while (true) {
        // Check keyboard keys (Scan codes for keys)
        for (int i = 8; i <= 255; ++i) {
            if (i == VK_RETURN) {
                continue; // Skip Enter key
            }
            if (GetAsyncKeyState(i) & 0x8000) {
                newClicker = i; // Update the new clicker button
                break;
            }
        }

        // Check mouse buttons (VK_LBUTTON = 1, VK_RBUTTON = 2, VK_MBUTTON = 4)
        for (int btn = VK_LBUTTON; btn <= VK_MBUTTON; ++btn) {
            if (GetAsyncKeyState(btn) & 0x8000) {
                newClicker = btn; // Update the new clicker button
                break;
            }
        }

        if (newClicker != -1) {
            if (registeredKeys.count("clicker") && newClicker == registeredKeys["clicker"]) {
                std::cerr << "Error: Key is already being used as clicker." << std::endl;
                FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
                break; // Exit the loop after setting a new hotkey
            } else if (registeredKeys.count("toggleClick") && newClicker == registeredKeys["toggleClick"] ||
                registeredKeys.count("increaseSpeed") && newClicker == registeredKeys["increaseSpeed"] ||
                registeredKeys.count("decreaseSpeed") && newClicker == registeredKeys["decreaseSpeed"] ||
                registeredKeys.count("menu") && newClicker == registeredKeys["menu"]) {
                std::cerr << "Error: Key is already in use for another function." << std::endl;
                newClicker = -1; // Reset newClicker to continue checking for valid input
            } else {
                std::cout << "New clicker: " << keyNameMap[newClicker] << std::endl;
                registeredKeys["clicker"] = newClicker; // Set the new clicker button
                clickerButton = newClicker; // Set the new clicker button
                FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
                break; // Exit the loop after setting a new hotkey
            }
        }

        Sleep(50); // Optional: Add a small delay to reduce CPU usage
    }

    pauseThreads = false;
}


int main() {
    // Register global hotkeys
    register_all_hotkeys();

    // Start the click thread
    std::thread clicker(click_thread);

    std::cout << "Press 'm' to start/stop clicking, '+' to increase speed, '-' to decrease speed, 'k' to exit, 'l' to open menu.\n";

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            switch (msg.wParam) {
                case HOTKEY_START_STOP:
                    running = !running;
                    std::cout << (running ? "Clicking started\n" : "Clicking stopped\n");
                    break;
                case HOTKEY_EXIT:
                    program_running = false;
                    running = false;
                    PostQuitMessage(0);
                    break;
                case HOTKEY_INCREASE_SPEED:
                    adjust_speed(true);
                    break;
                case HOTKEY_DECREASE_SPEED:
                    adjust_speed(false);
                    break;
                case HOTKEY_SET_SPEED:
                    // Pause the click thread and open the menu
                    pauseThreads = true;
                    unregister_all_hotkeys();

                    // Menu logic
                    openMenu = true;
                    std::string command;
                    std::cout << "Type a command. Type 'help' for help or 'exit' to close the menu.\n";
                    while (openMenu) {
                        std::getline(std::cin, command);
                        if (command == "exit") {
                            openMenu = false;
                        } else if (command == "help") {
                            std::cout << "Available commands:\n";
                            std::cout << "  set delay {number} - Set the delay between clicks (in seconds)\n";
                            std::cout << "    Note: It is reccomended to have a delay of at least 0.001 seconds\n";
                            std::cout << "  change clicker - Change the mouse button used for clicking\n";
                            std::cout << "  help - Show this help message\n";
                            std::cout << "  exit - Close the menu and resume hotkey functionality\n";
                        } else if (command.rfind("set delay", 0) == 0) {
                            try {
                                double newDelay = std::stod(command.substr(10));
                                delayTime = newDelay;
                                std::cout << "Delay set to " << newDelay << " seconds\n";
                            } catch (const std::exception& e) {
                                std::cout << "Error: Invalid delay value. Please enter a number.\n";
                            }
                        } else if (command == "change clicker") {
                            change_clicker();
                        } else {
                            std::cout << "Error: Invalid command. Type 'help' for a list of commands.\n";
                        }
                    }

                    // Resume the click thread and re-register hotkeys
                    pauseThreads = false;
                    register_all_hotkeys();
                    break;
            }
        }
    }

    // Unregister global hotkeys before exiting
    unregister_all_hotkeys();

    // Join the clicker thread before exiting
    if (clicker.joinable()) {
        clicker.join();
    }

    return 0;
}
