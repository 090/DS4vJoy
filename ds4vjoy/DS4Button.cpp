#include "stdafx.h"
#include "DS4Button.h"


DS4Button::DS4Button():
	m_data(0),
	m_mask(0),
	m_type(typeNone)
{
}

DS4Button::~DS4Button()
{
}


void DS4Button::setBit(BYTE * data, UINT32 bitmask)
{
	m_type = typeBit;
	m_data = data;
	m_mask = bitmask;
}

void DS4Button::setStick(BYTE * data)
{
	m_type = typeStick;
	m_data = data;
}

void DS4Button::setTrigger(BYTE * data)
{
	m_type = typeTrigger;
	m_data = data;
}

void DS4Button::setDPad(BYTE * data)
{
	m_type = typeDPad;
	m_data = data;
}

void DS4Button::setDPadType(BYTE * data, ButtonType type)
{
	m_type = type;
	m_data = data;
}


BOOL DS4Button::isPushed()
{
	switch (m_type)
	{
	case typeBit:
		return (*m_data & m_mask) != 0;
	case typeStick:
		return *m_data > 135 || *m_data < 121;
	case typeTrigger:
		return *m_data > 0;
	case typeDPad:
		return (*m_data & 0xF) != 8;
	}
	BYTE dpad = (*m_data & 0xF);
	switch (m_type)
	{
	case typeDPadUP:
		return (dpad == 7 || dpad == 0 || dpad == 1);
	case typeDPadRIGHT:
		return (dpad == 1 || dpad == 2 || dpad == 3);
	case typeDPadDOWN:
		return (dpad == 3 || dpad == 4 || dpad == 5);
	case typeDPadLEFT:
		return (dpad == 5 || dpad == 6 || dpad == 7);
	default:
		return FALSE;
	}
}

void DS4Button::Release()
{
	switch (m_type)
	{
	case typeBit:
		*m_data &= ~m_mask;
		break;
	case typeStick:
		*m_data = 128;
		break;
	case typeTrigger:
		*m_data = 0;
		break;
	case typeDPad:
		*m_data = (*m_data & 0xF0) | 8;
		break;
	case typeDPadUP:
		switch (*m_data&0xF){
		case 7:
			*m_data = (*m_data & 0xF0) | 6;
			break;
		case 0:
			*m_data = (*m_data & 0xF0) | 8;
			break;
		case 1:
			*m_data = (*m_data & 0xF0) | 2;
			break;
		}
		break;
	case typeDPadRIGHT:
		switch (*m_data & 0xF) {
		case 1:
			*m_data = (*m_data & 0xF0) | 0;
			break;
		case 2:
			*m_data = (*m_data & 0xF0) | 8;
			break;
		case 3:
			*m_data = (*m_data & 0xF0) | 4;
			break;
		}
		break;
	case typeDPadDOWN:
		switch (*m_data & 0xF) {
		case 3:
			*m_data = (*m_data & 0xF0) | 2;
			break;
		case 4:
			*m_data = (*m_data & 0xF0) | 8;
			break;
		case 5:
			*m_data = (*m_data & 0xF0) | 6;
			break;
		}
		break;
	case typeDPadLEFT:
		switch (*m_data & 0xF) {
		case 5:
			*m_data = (*m_data & 0xF0) | 4;
			break;
		case 6:
			*m_data = (*m_data & 0xF0) | 8;
			break;
		case 7:
			*m_data = (*m_data & 0xF0) | 0;
			break;
		}
		break;
	}
}

BYTE DS4Button::GetVal()
{
	switch (m_type)
	{
	case typeBit:
		return (*m_data & m_mask )?0xFF:0;
	case typeStick:
	case typeTrigger:
		return *m_data;
	case typeDPad:
		return *m_data & 0xF;
	}
	BYTE dpad = (*m_data & 0xF);
	switch (m_type)
	{
	case typeDPadUP:
		return (dpad == 7 || dpad == 0 || dpad == 1) ? 0xFF : 0;
	case typeDPadRIGHT:
		return (dpad == 1 || dpad == 2 || dpad == 3) ? 0xFF : 0;
	case typeDPadDOWN:
		return (dpad == 3 || dpad == 4 || dpad == 5) ? 0xFF : 0;
	case typeDPadLEFT:
		return (dpad == 5 || dpad == 6 || dpad == 7) ? 0xFF : 0;
	}
	return 0;
}

WCHAR * DS4Button::String(ButtonID id)
{
	switch (id)
	{
	case none: return L"";
	case SQUARE: return L"□";
	case TRIANGLE: return L"△";
	case CROSS: return L"×";
	case CIRCLE: return L"○";
	case L1: return L"L1";
	case R1: return L"R1";
	case L2: return L"L2";
	case R2: return L"R2";
	case L3: return L"L3";
	case R3: return L"R3";
	case SHARE: return L"SHARE";
	case OPTIONS: return L"OPTIONS";
	case PS: return L"PS";
	case TOUCH: return L"TouchPad";
	case LX: return L"LX";
	case LY: return L"LY";
	case RX: return L"RX";
	case RY: return L"RY";
	case L2TRIGGER: return L"L2Trigger";
	case R2TRIGGER: return L"R2Trigger";
	case DPAD: return L"十字キー";
	case DPAD_UP: return L"↑";
	case DPAD_RIGHT: return L"→";
	case DPAD_DOWN: return L"↓";
	case DPAD_LEFT: return L"←";
	default: return L"???";
	}
}

