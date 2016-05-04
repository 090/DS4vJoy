#pragma once
#include "DS4.h"
#include "vJoy.h"


class Mapping {
	DS4Button * m_ds4[4] = { 0 };
	vJoyButton * m_vjoy = 0;
	int m_toggle;
	BYTE m_data;
public:
	Mapping();

	DS4ButtonID  DS4ID[4] = { DS4ButtonID::none };
	bool Enable;
	bool Disbale[4];
	bool Force;
	bool Toggle;
	vJoyButtonID vJoyID;
	const WCHAR * StrDS4();
	const WCHAR * StrvJoy();
	bool LoadDevice(DS4Device*,vJoyDevice*);
	bool Run();
};


typedef std::vector<Mapping> Mappings;
