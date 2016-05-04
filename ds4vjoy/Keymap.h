#pragma once

#define KEYMAP_MAX_KEYS 10
#include "vJoy.h"

class Keymap {
	bool m_keydownflag;
	vJoyButton *m_button;
	void keydown();
	void keyup();
public:
	Keymap();
	vJoyButtonID ButtonID;
	bool Enable;
	std::vector<BYTE> vk;
	bool LoadDevice(vJoyDevice* vj);
	bool Run();
	void GetState();
	WCHAR* StrVal();
	WCHAR* StrKey();
};
typedef std::vector<Keymap> Keymaps;



