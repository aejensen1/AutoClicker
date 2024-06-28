# AutoClicker
Author: Anders Jensen
Date: 6-28-2024
Summary: An autoclicker that can be used for gaming, auto-liking, or task automations.

A while ago, I was playing a video game, and thought to myself: 
  I wish I could click faster, as well as take care of the monotonous tasks of clicking ahead of me while I was away from my PC. 

I created one of my first Python projects, building an autoclicker from scratch that I could turn on and let run as long as I wished.
It worked good enough for my purposes and for a long time it sat collecting dust in a folder.

Recently autoclickers came up in a conversation, and I decided that I could use my multiple years of coding experience to make an
autoclicker bigger and better than the one before.

Some of my goals:
  - Adjustable delay between clicks
  - Change clicker key bind (eg. change the autoclicker from left mouse button to 'q' or any other key)
  - Achieve the greatest speed possible with the hardware readily available to me.

To achieve better speed, I decided to shift away from the python interpreter to c coding. I dislike c code, and it would have taken me
a long time to code out everything, so I leveraged CoPilot, Bing AI, and ChatGPT. Using these tools did not make the coding problem
easy, but helped me focus on the bigger issues, and also required hours of troubleshooting.

One thing that was important to me was to be able to use hotkeys to start/stop the clicker while not focused in the terminal window.
I discovered that c was not working to make what are called "global hotkeys" using Windows API, and moved on to use C++, which worked.

As a Windows user, something important was that in order to compile the C or C++ code, I needed MinGW installed. The problem was that
mingw32 did not have enough capabilities with POSIX threading, which meant I had to find a way to get mingw64 working on my computer.

After some research, I discovered MSYS2, which among other features allows for installation of mingw64. Some important notes on setting
up mingw64 if you need to compile a .cpp file:

  - Go here to get the MSYS2 installation manager: https://www.msys2.org/
  - I dont reccommend typing what they say to install mingw64 on Windows: "pacman -S mingw-w64-ucrt-x86_64-gcc"
    - Instead install gcc using: "pacman -S mingw-w64-x86_64-gcc" and set environment variable path as C:\msys64\mingw64\bin.
    - Basically, these both install mingw, but in different folders. With the environment variable set, I was able to use g++ in Powershell.
  - Use this command to compile the cpp file. The static option requests static linking of dlls which may work better than dynamic for this
    - indivdual file: "g++ -std=c++11 -static -o my_program.exe my_program.cpp"

As far as user interface goes, you can access the hotkeys in any window. I tried to make the hotkeys uncommon keys needed for games and
aplications. You can change them in the code if needed. Also, the program may have issues if you have the hotkeys being used by another
applications, which could cause conflicts in registering. Some basic settings like delay can be modified in the command line.
Just type "l" to open the command option and "help" to see available commands.

This was a fun project overall, and got me more comfortable with C++. Enjoy!
