#pragma once


class DS4Button {
public:
	enum ButtonType
	{
		typeNone = 0,
		typeBit,
		typeStick,
		typeTrigger,
		typeDPad,
		typeDPadUP,
		typeDPadRIGHT,
		typeDPadDOWN,
		typeDPadLEFT,
	};

	enum ButtonID {
		none = 0,
		SQUARE,
		TRIANGLE,
		CROSS,
		CIRCLE,
		L1,
		R1,
		L2,
		R2,
		L3,
		R3,
		SHARE,
		OPTIONS,
		PS,
		TOUCH,
		LX,
		LY,
		RX,
		RY,
		L2TRIGGER,
		R2TRIGGER,
		DPAD,
		DPAD_UP,
		DPAD_RIGHT,
		DPAD_DOWN,
		DPAD_LEFT,
		button_Count
	};

	DS4Button();
	void setBit(BYTE*data, UINT32 bitmask);
	void setStick(BYTE *data);
	void setTrigger(BYTE *data);
	void setDPad(BYTE*data);
	void setDPadType(BYTE*data, ButtonType type);
	~DS4Button();
	BOOL isPushed();
	void Release();
	BYTE GetVal();
	static WCHAR *String(ButtonID);
private:
	ButtonType m_type;
	BYTE *m_data;
	UINT32 m_mask;
};

typedef DS4Button::ButtonID DS4ButtonID;

