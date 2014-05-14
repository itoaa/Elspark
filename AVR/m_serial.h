#ifndef _m_serial_h
#define _m_serial_h

int SendSerialByte(int l_sendbyte);
int Send_Sec(int l_sec);
int Send_Rev(int l_CodeRev);
int Send_Realtimedata();
int Send_EpromVar();
int Get_EpromVar();

#endif
