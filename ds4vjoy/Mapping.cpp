#include "stdafx.h"
#include "Mapping.h"



Mapping::Mapping()
	:m_toggle(0)
{
}

const WCHAR * Mapping::StrDS4()
{
	static WCHAR buf[256];
	buf[0] = 0;
	WCHAR *head=buf;
	for (int i = 0; i < 4; i++) {
		if (DS4ID[i] == 0)
			break;
		head += wsprintf(head, L"%s+", DS4Button::String( (DS4ButtonID) DS4ID[i]));
	}
	if (buf != head) {
		head--;
		head[0] = 0;
	}

	return buf;
}

const WCHAR * Mapping::StrvJoy()
{
	static WCHAR buf[256];
	wsprintf(buf, L"%s", vJoyButton::String((vJoyButtonID) vJoyID) );

	return buf;
}

bool Mapping::LoadDevice(DS4Device * ds4, vJoyDevice * vjoy)
{
	if (!Enable || DS4ID[0] == 0 || vJoyID == 0)
		return false;
	m_ds4[0] = ds4->GetButton(DS4ID[0]);
	m_ds4[1] = DS4ID[1] ? ds4->GetButton(DS4ID[1]) : 0;
	m_ds4[2] = DS4ID[2] ? ds4->GetButton(DS4ID[2]) : 0;
	m_ds4[3] = DS4ID[3] ? ds4->GetButton(DS4ID[3]) : 0;
	m_vjoy = vjoy->GetButton(vJoyID);
	return true;
}

bool Mapping::Run()
{

	if (( m_ds4[1] == 0 || 
			( m_ds4[1]->isPushed() &&
				( m_ds4[2] == 0 ||
					(m_ds4[2]->isPushed() &&
						( m_ds4[3] == 0 || m_ds4[3]->isPushed() )
					)
				)
			)
		) && ( Force || m_ds4[0]->isPushed() )
	){
		m_data = m_ds4[0]->GetVal();
		if (Toggle) {
			if (m_toggle <= 1) {
				m_vjoy->SetVal(m_data);
				m_toggle = 1;
			}
			else {
				m_toggle = 3;
			}
		}
		else {
			m_vjoy->SetVal(m_data);

		}
		if (Disbale[0])
			m_ds4[0]->Release();
		if (Disbale[1] && m_ds4[1])
			m_ds4[1]->Release();
		if (Disbale[2] && m_ds4[2])
			m_ds4[2]->Release();
		if (Disbale[3] && m_ds4[3])
			m_ds4[3]->Release();

		return true;
	}
	else if(m_toggle) {
		switch (m_toggle)
		{
		case 1:
			m_toggle = 2;
		case 2:
			m_vjoy->SetVal(m_data);
			break;
		case 3:
			m_toggle = 0;
			break;
		}
	}
	return false;
}
