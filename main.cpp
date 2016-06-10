#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "log.hpp"
#include "remote.hpp"
#include "hack.hpp"

using namespace std;

bool shouldGlow = true;

int main() {
    if (getuid() != 0) {
        cout << "You should run this as root." << endl;
        return 0;
    }

    cout << "s0beit linux hack version 1.3" << endl;

    log::init();
    log::put("Hack loaded...");

	Display* dpy = XOpenDisplay(0);
	Window root = DefaultRootWindow(dpy);
	XEvent ev;

	int keycode = XKeysymToKeycode(dpy, XK_X);
	unsigned int modifiers = ControlMask | ShiftMask;

	XGrabKey(dpy, keycode, modifiers, root, false,
				GrabModeAsync, GrabModeAsync);
	XSelectInput(dpy, root, KeyPressMask);
	
    remote::Handle csgo;

    while (true) {
        if (remote::FindProcessByName("csgo_linux64", &csgo)) {
            break;
        }

        usleep(500);
    }

    cout << "CSGO Process Located [" << csgo.GetPath() << "][" << csgo.GetPid() << "]" << endl << endl;

    remote::MapModuleMemoryRegion client;

    client.start = 0;

    while (client.start == 0) {
        if (!csgo.IsRunning()) {
            cout << "Exited game before client could be located, terminating" << endl;
            return 0;
        }

        csgo.ParseMaps();

        for (auto region : csgo.regions) {
            if (region.filename.compare("client_client.so") == 0 && region.executable) {
                cout << "client_client.so: [" << std::hex << region.start << "][" << std::hex << region.end << "][" <<
                region.pathname << "]" << endl;
                client = region;
                break;
            }
        }

        usleep(500);
    }

    cout << "GlowObject Size: " << std::hex << sizeof(hack::GlowObjectDefinition_t) << endl;

    cout << "Found client_client.so [" << std::hex << client.start << "]" << endl;
    client.client_start = client.start;

// Old sig x86 "\xE8\x00\x00\x00\x00\x8B\x78\x14\x6B\xD6"
//                                             "x????xxxxx"
//Old Sig Pre 11/10/15
//\xE8\x00\x00\x00\x00\x8B\x78\x14\x6B\xD6\x34
//x????xxxxxx
//New Sig as of 11/10/15
//\xE8\x00\x00\x00\x00\x8B\x78\x14\x6B\xD6\x38
//x????xxxxxx
//11/10/15 Sig reduction, we don't need the size
//\xE8\x00\x00\x00\x00\x8B\x78\x14\x6B\xD6
//x????xxxxx

 void* foundGlowPointerCall = client.find(csgo,
                                             "\xE8\x00\x00\x00\x00\x48\x8b\x10\x48\xc1\xe3\x06\x44",
                                             "x????xxxxxxxx");

    cout << "Glow Pointer Call Reference: " << std::hex << foundGlowPointerCall <<
    " | Offset: " << (unsigned long) foundGlowPointerCall - client.start << endl;
    
    unsigned long call = csgo.GetCallAddress(foundGlowPointerCall);

    cout << "Glow function address: " << std::hex << call << endl;
    cout << "Glow function address offset: " << std::hex << call - client.start << endl;

    unsigned int addressOfGlowPointerOffset;

    if (!csgo.Read((void*) (call + 0x10), &addressOfGlowPointerOffset, sizeof(unsigned int))) {
        cout << "Unable to read address of glow pointer" << endl;
        return 0;
    }
    
    cout << "Glow Array offset: " << std::hex << addressOfGlowPointerOffset << endl << endl;
    
    unsigned long addressOfGlowPointer = (call + 0x10) + addressOfGlowPointerOffset + 0x4  ;
    
    cout << "Glow Array pointer " << std::hex << addressOfGlowPointer << endl << endl;

    while (csgo.IsRunning()) {
		while (XPending(dpy) > 0) {
			XNextEvent(dpy, &ev);
			switch (ev.type) {
				case KeyPress:
					cout << "Toggling glow..." << endl;
					XUngrabKey(dpy, keycode, modifiers, root);
					shouldGlow = !shouldGlow;
					break;
				default:
					break;
			}

			XGrabKey(dpy, keycode, modifiers, root, false,
						GrabModeAsync, GrabModeAsync);
			XSelectInput(dpy, root, KeyPressMask);
		}

		if (shouldGlow)
	        hack::Glow(&csgo, &client, addressOfGlowPointer);

        usleep(1000);
    }

//    cout << "Game ended." << endl;

    return 0;
}
