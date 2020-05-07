#include "stm32_keyboard.h"

#define cnth 5
#define cntv 12

int lbtnh = 4, lbtnv = 3;
int pinh[] = {26, 27, 28, 29, 30, 31};
int pinv[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};

int btn[cnth][cntv] = {
	{ESC, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _0_, MNS},
	{TAB, _Q_, _W_, _E_, _R_, _T_, _Y_, _U_, _I_, _O_, _P_, BSP},
	{GRV, _A_, _S_, _D_, _F_, _G_, _H_, _J_, _K_, _L_, SMC, ENT},
	{LSH, _Z_, _X_, _C_, _V_, _B_, _N_, _M_, CMM, DOT, SLS, RSH},
	{  0,   0,   0, LCT, SPC, LWN, INS, SPC, LAL, DEL,   0,   0}
};

int btn1[cnth][cntv] = {
	{F01, F02, F03, F04, F05, F06, F07, F08, F09, F10, F11, F12},
	{TAB, _Q_, PGU, _E_, _R_, _T_, _Y_, _U_, AUP, SBL, SBR, BSP},
	{EQL, HOM, PGD, END, _F_, _G_, _H_, ARG, ADW, ALF, APO, ENT},
	{LSH, _Z_, _X_, _C_, _V_, _B_, _N_, _M_, CMM, DOT, BSL, RSH},
	{  0,   0,   0, RCT, SPC, RWN, INS, SPC, RAL, DEL,   0,   0}
};

int nowbtn[cnth][cntv];
int rea[cnth][cntv];

void setup()
{ 
	Keyboard.begin();  
	for (int i = 0; i < cnth; ++i)
	{  
		pinMode(pinh[i], INPUT);
		digitalWrite(pinh[i], LOW);
	}
	for (int i = 0; i < cntv; ++i)
	{  
		pinMode(pinv[i], INPUT);
		digitalWrite(pinv[i], HIGH);
	}
	for (int i = 0; i < cnth; ++i)
		for (int j = 0; j < cntv; ++j)
			nowbtn[i][j] = 0;
	for (int i = 0; i < cnth; ++i)
		for (int j = 0; j < cntv; ++j)
			rea[i][j] = 0;

	delay(1000);
}

void loop()
{
	for (int i = 0; i < cnth; ++i)
	{
		pinMode(pinh[i], OUTPUT);
		digitalWrite(pinh[i], LOW);
		for (int j = 0; j < cntv; ++j)
			rea[i][j] = !digitalRead(pinv[j]);
		pinMode(pinh[i], INPUT);
		digitalWrite(pinh[i], LOW);
	}

	int lbtn = rea[lbtnh][lbtnv];
	for (int i = 0; i < cnth; ++i)
		for (int j = 0; j < cntv; ++j)
		{
			if (!nowbtn[i][j] && rea[i][j])
			{
				if (lbtn)
					nowbtn[i][j] = btn1[i][j];
				else
					nowbtn[i][j] = btn[i][j];
				if (nowbtn[i][j])
					nowbtn[i][j] = Keyboard.press(nowbtn[i][j]);
			}
			else if (nowbtn[i][j] && !rea[i][j])
			{
				nowbtn[i][j] = Keyboard.release(nowbtn[i][j]);
			}
		}
	
	Keyboard.sendReport();
	delay(1);
}
