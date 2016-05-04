#pragma once

#define DS4_VenderID 0x54C
#define DS4_ProductID 0x5C4
#define MAX_Serial_Length 127
#define MAX_Input_Length 10+8
#define DS4_Latency_Count 100


#include "DS4Button.h"


class DS4Device
{
	HANDLE hDS4Handle = INVALID_HANDLE_VALUE;
	WCHAR m_DS4Serial[MAX_Serial_Length];
	WCHAR m_TargetSerial[MAX_Serial_Length];
	int m_inputLen, m_outputLen;
	bool m_bBluetooth;
	
	BYTE m_Red = 0;
	BYTE m_Green = 0;
	BYTE m_Blue = 0;
	BYTE m_Right = 0;
	BYTE m_Left = 0;

	HANDLE m_hWEvent,m_hREvent;
	OVERLAPPED m_oWrite,m_oRead;

	BYTE *m_OutputBuf = 0;
	BYTE *m_InputBuf = 0;
	int m_outputOffset;

	HANDLE m_hThread;
	UINT32 m_threadID = 0;
	HANDLE m_hThread2;
	UINT32 m_threadID2 = 0;
	bool m_threadShutdown = 0;

	BYTE m_receivedData[MAX_Input_Length];
	BYTE m_receivedLastData[MAX_Input_Length];
	int m_receivedLength;

	double m_latency[DS4_Latency_Count];
	int m_latencyIdx=0;

	std::mutex m_write_mutex;
	std::condition_variable m_write_cv;
	BYTE m_write_count, m_write_count2;

	bool _write();
	bool _read();
	bool _parse();

	DS4Button m_buttons[DS4Button::button_Count];



	enum _inputBuffer {
		InputOffsetLX = 0,
		InputOffsetLY,
		InputOffsetRX,
		InputOffsetRY,
		InputOffsetDPad = 4,
		InputOffsetSquare = 4,
		InputOffsetCross = 4,
		InputOffsetCircle = 4,
		InputOffsetTriangle = 4,
		InputOffsetL1 = 5,
		InputOffsetR1 = 5,
		InputOffsetL2 = 5,
		InputOffsetR2 = 5,
		InputOffsetShare = 5,
		InputOffsetOption = 5,
		InputOffsetL3 = 5,
		InputOffsetR3 = 5,
		InputOffsetPS = 6,
		InputOffsetTouchPadClick = 6,
		InputOffsetCounter = 6,
		InputOffsetLeftTrigger = 7,
		InputOffsetRightTrigger,
		InputOffsetBattery = 11,
		InputOffsetBatteryLevel = 29,
		InputOffsetTouchs = 32,
		InputOffsetPacketCounter = 33,
		InputOffsetFinger1 = 34,
		InputOffsetFinger2 = 38,
	};
	enum _parseBuffer {
		OffsetLX = 0,
		OffsetLY,
		OffsetRX,
		OffsetRY,
		OffsetDPad = 4,
		OffsetSquare = 4,
		OffsetCross = 4,
		OffsetCircle = 4,
		OffsetTriangle = 4,
		OffsetL1 = 5,
		OffsetR1 = 5,
		OffsetL2 = 5,
		OffsetR2 = 5,
		OffsetShare = 5,
		OffsetOption = 5,
		OffsetL3 = 5,
		OffsetR3 = 5,
		OffsetPS = 6,
		OffsetTouchPadClick = 6,
		OffsetCounter = 6,
		OffsetLeftTrigger = 7,
		OffsetRightTrigger,
		OffsetBatteryLevel = 9,
		OffsetTouch = 10

	};

public:
	typedef void(*DS4InputCallback)(DS4Device*,BOOL, void*);

	DS4Device();

	bool Active();
	bool Open();
	bool Close();
	void SetCallback(DS4InputCallback cb, void *param);
	WCHAR * GetSerial();
	bool SetTargetSerial(const WCHAR *);
	bool SetLED(BYTE R, BYTE G, BYTE B);
	bool SetMoter(BYTE left, BYTE right);
	bool Write();
	void InputLoop();
	void OutputLoop();

	bool isBT();
	int Battery();
	void DisconnectBT();
	DS4Button * GetButton(DS4ButtonID);

	//i=0 or 1
	bool TouchActive(int i = 0);
	BYTE TouchId(int i = 0);
	int TouchX(int i = 0);
	int TouchY(int i = 0);

	void String();
	~DS4Device();
private:
	DS4InputCallback m_callback;
	void *m_callbackdata;

};


