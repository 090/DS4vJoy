#include "stdafx.h"
#include "DS4.h"
#include "Log.h"

unsigned __stdcall inputloop(void  *v)
{
	((DS4Device*)v)->InputLoop();
	return 0;
}
unsigned __stdcall outputloop(void  *v)
{
	((DS4Device*)v)->OutputLoop();
	return 0;
}

DS4Device::DS4Device():
	m_callback(0),m_callbackdata(0)
	, hDS4Handle(INVALID_HANDLE_VALUE)
	, m_hThread(NULL)
	, m_Red(60),m_Green(0),m_Blue(0)
{
	memset(m_DS4Serial, 0, sizeof(m_DS4Serial));
	m_oRead.hEvent = m_hREvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_oWrite.hEvent = m_hWEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_buttons[DS4Button::SQUARE].setBit(m_receivedData + OffsetSquare, 0b10000);
	m_buttons[DS4Button::TRIANGLE].setBit(m_receivedData + OffsetTriangle, 0b10000000);
	m_buttons[DS4Button::CROSS].setBit(m_receivedData + OffsetCross, 0b100000);
	m_buttons[DS4Button::CIRCLE].setBit(m_receivedData + OffsetCircle, 0b1000000);
	m_buttons[DS4Button::L1].setBit(m_receivedData + OffsetL1, 0b1);
	m_buttons[DS4Button::R1].setBit(m_receivedData + OffsetR1, 0b10);
	m_buttons[DS4Button::L2].setBit(m_receivedData + OffsetL2, 0b100);
	m_buttons[DS4Button::R2].setBit(m_receivedData + OffsetR2, 0b1000);
	m_buttons[DS4Button::L3].setBit(m_receivedData + OffsetL3, 0b1000000);
	m_buttons[DS4Button::R3].setBit(m_receivedData + OffsetR3, 0b10000000);
	m_buttons[DS4Button::SHARE].setBit(m_receivedData + OffsetShare, 0b10000);
	m_buttons[DS4Button::OPTIONS].setBit(m_receivedData + OffsetOption, 0b100000);
	m_buttons[DS4Button::PS].setBit(m_receivedData + OffsetPS, 0b1);
	m_buttons[DS4Button::TOUCH].setBit(m_receivedData + OffsetTouchPadClick, 0b10);
	m_buttons[DS4Button::LX].setStick(m_receivedData + OffsetLX);
	m_buttons[DS4Button::LY].setStick(m_receivedData + OffsetLY);
	m_buttons[DS4Button::RX].setStick(m_receivedData + OffsetRX);
	m_buttons[DS4Button::RY].setStick(m_receivedData + OffsetRY);
	m_buttons[DS4Button::L2TRIGGER].setTrigger(m_receivedData + OffsetLeftTrigger);
	m_buttons[DS4Button::R2TRIGGER].setTrigger(m_receivedData + OffsetRightTrigger);
	m_buttons[DS4Button::DPAD].setDPad(m_receivedData + OffsetDPad);
	m_buttons[DS4Button::DPAD_UP].setDPadType(m_receivedData + OffsetDPad, DS4Button::typeDPadUP);
	m_buttons[DS4Button::DPAD_RIGHT].setDPadType(m_receivedData + OffsetDPad, DS4Button::typeDPadRIGHT);
	m_buttons[DS4Button::DPAD_DOWN].setDPadType(m_receivedData + OffsetDPad, DS4Button::typeDPadDOWN);
	m_buttons[DS4Button::DPAD_LEFT].setDPadType(m_receivedData + OffsetDPad, DS4Button::typeDPadLEFT);

}

DS4Device::~DS4Device()
{
	Close();
	CloseHandle(m_hREvent);
	CloseHandle(m_hWEvent);
}


bool DS4Device::Active()
{
	return hDS4Handle != INVALID_HANDLE_VALUE;
}

bool DS4Device::Open()
{
	if (hDS4Handle != INVALID_HANDLE_VALUE)
		return false;
	Close();
	GUID guid;
	HidD_GetHidGuid(&guid);
	bool ret = false;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&guid,NULL,NULL,DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == 0) {
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

	HIDD_ATTRIBUTES Attributes;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detail=0;
	DWORD RequiredSize;

	for (int i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, 0, &guid, i, &DeviceInterfaceData) != 0; i++) {
		RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &DeviceInterfaceData, NULL, 0, &RequiredSize, NULL);
		if (RequiredSize == 0)
			continue;
		if (detail != 0)
			free(detail);
		if (hDS4Handle != INVALID_HANDLE_VALUE) {
			CloseHandle(hDS4Handle);
			hDS4Handle = INVALID_HANDLE_VALUE;
		}
		if (m_OutputBuf != NULL) {
			free(m_OutputBuf);
			m_OutputBuf = NULL;
		}
		if (m_InputBuf != NULL) {
			free(m_InputBuf);
			m_InputBuf = NULL;
		}
		detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(RequiredSize);
		if (detail == 0)
			continue;
		detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		DWORD dwRequiredSize;
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &DeviceInterfaceData, detail, RequiredSize, &dwRequiredSize, NULL)) {
			continue;
		}
		hDS4Handle = CreateFile(
			detail->DevicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);
		if (hDS4Handle == INVALID_HANDLE_VALUE) {
			continue;
		}

		Attributes.Size = sizeof(Attributes);
		if (!HidD_GetAttributes(hDS4Handle, &Attributes)) {
			continue;
		}
		if (DS4_VenderID != Attributes.VendorID || DS4_ProductID != Attributes.ProductID) {
			continue;
		}

		PHIDP_PREPARSED_DATA PreparsedData;
		if (!HidD_GetPreparsedData(hDS4Handle, &PreparsedData))
			continue;

		HIDP_CAPS cap;
		NTSTATUS r = HidP_GetCaps(PreparsedData, &cap);
		HidD_FreePreparsedData(PreparsedData);
		if (r != HIDP_STATUS_SUCCESS)
			continue;

		//送受信用のバッファの設定
		m_inputLen = cap.InputReportByteLength;
		m_outputLen = cap.OutputReportByteLength;
		m_bBluetooth = m_inputLen > 64;

		//シリアル判定
		memset(m_DS4Serial, 0, sizeof(m_DS4Serial));
		if (m_bBluetooth) {
			if (!HidD_GetSerialNumberString(hDS4Handle, m_DS4Serial, sizeof(m_DS4Serial)))
				continue;
		}else{
			unsigned char buf[16] = { 18 };
			if (!HidD_GetFeature(hDS4Handle, buf, sizeof(buf)))
				continue;
			wsprintf(m_DS4Serial, L"%02x%02x%02x%02x%02x%02x", buf[6], buf[5], buf[4], buf[3], buf[2], buf[1]);
		}
		if (0 != m_TargetSerial[0] && _wcsicmp(m_TargetSerial, m_DS4Serial) != 0) {
			LogPrintf(L"DualShock4 Serial %s は無視します。", m_DS4Serial);
			continue;
		}
		LogPrintf(L"DualShock4 Serial %s に接続します…", m_DS4Serial);

		//送受信バッファ確保
		m_InputBuf = (BYTE*)malloc(m_inputLen);
		m_OutputBuf = (BYTE*)malloc(m_outputLen);
		memset(m_OutputBuf, 0, m_outputLen);
		memset(m_InputBuf, 0, m_inputLen);
		if (m_bBluetooth) {
			m_OutputBuf[0] = 0x11;
			m_OutputBuf[1] = 0x80;
			m_OutputBuf[3] = 0xff;
			m_outputOffset = 6;
		}
		else {
			m_OutputBuf[0] = 0x05;
			m_OutputBuf[1] = 0xFF;
			m_outputOffset = 4;
		}
		m_write_count = m_write_count2 = 0;
		Write();
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, inputloop, this, 0, &m_threadID);
		m_hThread2 = (HANDLE)_beginthreadex(NULL, 0, outputloop, this, 0, &m_threadID2);
		if (m_hThread != 0 && m_hThread2 != 0) {
			SetThreadAffinityMask(m_hThread, 1);
			ret = true;
			break;
		}
		else {
			LogPrintf(L"DS4送受信スレッドを作成出来ませんでした。");
		}
	}
	if (detail != 0)
		free(detail);
	if (!ret && hDS4Handle != INVALID_HANDLE_VALUE) {
		CloseHandle(hDS4Handle);
		hDS4Handle = INVALID_HANDLE_VALUE;
	}


	SetupDiDestroyDeviceInfoList(hDevInfo);
	return ret;
}

bool DS4Device::Close()
{
	if (m_hThread) {
		m_threadShutdown = true;
		WaitForSingleObject(m_hThread, INFINITE);
		m_write_cv.notify_one();
		WaitForSingleObject(m_hThread2, INFINITE);
		m_threadShutdown = false;
		m_hThread = NULL;
		m_hThread2 = NULL;
		m_write_count = m_write_count2 = 0;
	}
	if (hDS4Handle != INVALID_HANDLE_VALUE) {
		HANDLE tmp = hDS4Handle;
		hDS4Handle = INVALID_HANDLE_VALUE;
		CloseHandle(tmp);
	}
	if (m_OutputBuf != NULL) {
		free(m_OutputBuf);
		m_OutputBuf = NULL;
	}
	if (m_InputBuf != NULL) {
		free(m_InputBuf);
		m_InputBuf = NULL;
	}

	memset(m_DS4Serial, 0, sizeof(m_DS4Serial));
	return false;
}


//DS4からの受信ループ
void DS4Device::InputLoop()
{
	bool firstflag = true;
	while (!m_threadShutdown) {
		if (!_read()) {
			break;
		}
		if (m_receivedLength == 0) {
			LogPrintf(L"DS4からの受信を終了します。");
			break;
		}
		if (!_parse()) {
			LogPrintf(L"DS4からのデータ解析に失敗しました。");
			break;
		}
		if (firstflag) {
			LogPrintf(L"通信を開始します。");
			firstflag = false;
		}
	}
	m_threadShutdown = true;

	if (hDS4Handle != INVALID_HANDLE_VALUE) {
		CloseHandle(hDS4Handle);
		hDS4Handle = INVALID_HANDLE_VALUE;
	}

}
//DS4への送信ループ
void DS4Device::OutputLoop()
{
	while (!m_threadShutdown) {
		while (m_write_count != m_write_count2) {
			m_write_count2 = m_write_count;
			if (!_write()) {
				goto EndOutputLoop;
			}
		}
		std::unique_lock<std::mutex> lk(m_write_mutex);
		m_write_cv.wait(lk);
	}
	EndOutputLoop:
	m_threadShutdown = true;

}

//受信コールバック
void DS4Device::SetCallback(DS4InputCallback cb, void * param)
{
	m_callback = cb;
	m_callbackdata = param;
}


WCHAR * DS4Device::GetSerial()
{
	return m_DS4Serial;
}
//接続するDS4を指定
bool DS4Device::SetTargetSerial(const WCHAR * szTarget)
{
	if (lstrlenW(szTarget) + 1 > MAX_Serial_Length)
		return false;
	lstrcpyW(m_TargetSerial, szTarget);
	return true;
}

bool DS4Device::SetLED(BYTE R, BYTE G, BYTE B)
{
	m_Red = R;
	m_Green = G;
	m_Blue = B;
	return true;
}

bool DS4Device::SetMoter(BYTE left, BYTE right)
{
	m_Left = left;
	m_Right = right;
	return true;
}
//LEDとMotor情報を送信
bool DS4Device::_write()
{
	if (hDS4Handle == INVALID_HANDLE_VALUE)
		return false;

	m_OutputBuf[m_outputOffset] = m_Left;
	m_OutputBuf[m_outputOffset + 1] = m_Right;
	m_OutputBuf[m_outputOffset + 2] = m_Red;
	m_OutputBuf[m_outputOffset + 3] = m_Green;
	m_OutputBuf[m_outputOffset + 4] = m_Blue;
	if (m_bBluetooth) {
		if (HidD_SetOutputReport(hDS4Handle, m_OutputBuf, 78))//78固定 m_outputLen
			return true;
		if (!m_threadShutdown && hDS4Handle != INVALID_HANDLE_VALUE) {
			DWORD errorcode = GetLastError();
			switch (errorcode)
			{
			default:
				LogPrintf(L"DS4(BT)への送信に失敗しました。  Error:%d (0x%x)", errorcode, errorcode);
				break;
			}
		}
		return false;
	}
	ResetEvent(m_oWrite.hEvent);
	DWORD n;
	if (WriteFile(hDS4Handle, m_OutputBuf, m_outputLen, &n, &m_oWrite)) {
		return true;
	}
	else if (GetLastError() == ERROR_IO_PENDING) {
		if (WaitForSingleObject(m_oWrite.hEvent, 5000) != WAIT_OBJECT_0) {
			if (hDS4Handle != INVALID_HANDLE_VALUE) {
				CancelIo(hDS4Handle);
				WaitForSingleObject(m_oWrite.hEvent, INFINITE);
			}
		}
		else {
			if (GetOverlappedResult(hDS4Handle, &m_oWrite, &n, false)) {
				return true;
			}
		}
	}
	DWORD errorcode = GetLastError();
	switch (errorcode)
	{
	default:
		if (!m_threadShutdown&&hDS4Handle != INVALID_HANDLE_VALUE)
			LogPrintf(L"DS4(USB)への送信に失敗しました。  Error:%d (0x%x)", errorcode, errorcode);
		break;
	}
	return false;
}

//送信スレッドに命令するだけ
bool DS4Device::Write() {
	m_write_count++;
	m_write_cv.notify_one();
	return true;
}
//受信
bool DS4Device::_read()
{
	if (hDS4Handle == INVALID_HANDLE_VALUE)
		return false;
	DWORD n;
	ResetEvent(m_oRead.hEvent);
	if (ReadFile(hDS4Handle, m_InputBuf, m_inputLen, &n, &m_oRead)) {
		m_receivedLength = n;
		return true;
	}
	if (GetLastError() == ERROR_IO_PENDING) {
		if (WaitForSingleObject(m_oRead.hEvent, 5000) != WAIT_OBJECT_0) {
			if (hDS4Handle != INVALID_HANDLE_VALUE) {
				CancelIo(hDS4Handle);
				WaitForSingleObject(m_oRead.hEvent, INFINITE);
			}
		}
		else {
			if (GetOverlappedResult(hDS4Handle, &m_oRead, &n, false)) {
				m_receivedLength = n;
				return true;
			}
		}
	}
	DWORD errorcode = GetLastError();
	switch (errorcode)
	{
	case 1167:
		if (!m_threadShutdown&&hDS4Handle != INVALID_HANDLE_VALUE)
			LogPrintf(L"デバイスがまだ接続されていませんでした。");
		break;
	default:
		if (!m_threadShutdown&&hDS4Handle != INVALID_HANDLE_VALUE)
			LogPrintf(L"DS4からの受信に失敗しました。  Error:%d (0x%x)", errorcode, errorcode);
		break;
	}
	return false;
}
//受信データの解析
bool DS4Device::_parse()
{

	BYTE *buffer = m_InputBuf,
		*bufferEnd = m_InputBuf+ m_receivedLength;
	if (m_bBluetooth) {
		if (m_InputBuf[0] != 0x11) {
			return true;
		}
		buffer += 3;
	}
	else {
		if (m_InputBuf[0] != 0x01) {
			return true;
		}
		buffer += 1;
	}

	if (m_receivedLength < 42) {
		LogPrintf(L"受信データのサイズが足りませんでした。");
		return false;
	}

	//ボタンの状態を保存
	m_receivedData[0] = buffer[0];
	m_receivedData[1] = buffer[1];
	m_receivedData[2] = buffer[2];
	m_receivedData[3] = buffer[3];
	m_receivedData[4] = buffer[4];
	m_receivedData[5] = buffer[5];
	m_receivedData[6] = buffer[6] & 0b11;
	m_receivedData[7] = buffer[7];
	m_receivedData[8] = buffer[8];

	//Battery残量
	m_receivedData[OffsetBatteryLevel] = buffer[InputOffsetBatteryLevel];

	//Touch情報を収集
	int n = buffer[InputOffsetTouchs];
	if (n > 4) {
		n = 1;
	}
	BYTE *dst, *src;
	src = buffer + InputOffsetTouchs + 1;
	dst = m_receivedData + OffsetTouch;
	for (int i = 1; i <= 4; i++) {
		if (i == n && src+8 < bufferEnd) {
			dst[0] = src[1];
			dst[1] = src[2];
			dst[2] = src[3];
			dst[3] = src[4];
			dst[4] = src[5];
			dst[5] = src[6];
			dst[6] = src[7];
			dst[7] = src[8];
		}
		src += 9;
	}

	//状態が変化しているか判定
	BOOL updateflag = false;
	if (memcmp(m_receivedData, m_receivedLastData, MAX_Input_Length) != 0) {
		updateflag = true;
		memcpy(m_receivedLastData, m_receivedData, MAX_Input_Length);
	}

	//コールバック
	if (m_callback) {
		m_callback(this,updateflag, m_callbackdata);
	}

	return true;
}
#pragma region get state

bool DS4Device::isBT()
{
	return m_bBluetooth;
}

int DS4Device::Battery()
{
	return m_receivedData[OffsetBatteryLevel] & 0x0F;
}

void DS4Device::DisconnectBT()
{
	if (!Active() || !isBT())
		return;

	BLUETOOTH_ADDRESS ds4addr;
	int ic = 5;
	for (int i = 0; i < 12; i++) {
		BYTE hex = (BYTE)m_DS4Serial[i];
		if ('0' <= hex && hex <= '9') {
			hex -= '0';
		}
		else if ('a' <= hex && hex <= 'f') {
			hex -= 'a' - 10;
		}
		else if ('A' <= hex && hex <= 'F') {
			hex -= 'A' - 10;
		}
		else {
			hex = 0;
		}
		if (i & 1) {
			ds4addr.rgBytes[ic] |= hex;
			ic--;
		}
		else {
			ds4addr.rgBytes[ic] = hex << 4;
		}
	}
	Close();

	BLUETOOTH_FIND_RADIO_PARAMS param = { 0 };
	param.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

	HANDLE radio = 0;
	HBLUETOOTH_RADIO_FIND find = BluetoothFindFirstRadio(&param, &radio);
	if (find) {
		do {
			BLUETOOTH_RADIO_INFO info = { 0 };
			info.dwSize = sizeof(BLUETOOTH_RADIO_INFO);
			if (BluetoothGetRadioInfo(radio, &info) == ERROR_SUCCESS) {
				DWORD n;
				if (DeviceIoControl(radio, 0x41000c, &ds4addr, sizeof(ds4addr), 0, 0, &n, 0)) {
					CloseHandle(radio);
					return;
				}
			}
			CloseHandle(radio);
		} while (BluetoothFindNextRadio(find, &radio));
	}
}

DS4Button * DS4Device::GetButton(DS4ButtonID id)
{
	if(id >= DS4ButtonID::none && id < DS4ButtonID::button_Count)
		return &m_buttons[id];
	return &m_buttons[0];

}

bool DS4Device::TouchActive(int i)
{
	if (i != 0)i = 4;
	return (m_receivedData[OffsetTouch + i] & 0b10000000) == 0;
}

BYTE DS4Device::TouchId(int i)
{
	if (i != 0)i = 4;
	return m_receivedData[OffsetTouch + i] & 0b1111111;
}

int DS4Device::TouchX(int i)
{
	if (i != 0)i = 4;
	return ((m_receivedData[OffsetTouch + 2 + i] & 0x0F) << 8) | m_receivedData[OffsetTouch + 1 + i];
}

int DS4Device::TouchY(int i)
{
	if (i != 0)i = 4;
	return (m_receivedData[OffsetTouch + 2 + i] >> 4) | (m_receivedData[OffsetTouch + 3 + i] << 4);
}

void DS4Device::String()
{

	wchar_t buf[256];
	wchar_t *offset = buf;
	offset += wsprintfW(offset, L"F(%d %d,%d  %d %d,%d) %d ", TouchActive(0), TouchX(0), TouchY(0)
		, TouchActive(1), TouchX(1), TouchY(1));
	if (buf != offset) {
		LogPrintf(buf);
	}
}

#pragma endregion

