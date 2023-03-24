#include "LoadPNG.h"



const wchar_t* getMap(BYTE level) {
	switch (level)
	{
	case 2:
		return L"assets\\[ID2]Map_LVL1.png";
	case 3:
		return L"assets\\[ID3]Map_LVL2.png";
	case 4:
		return L"assets\\[ID4]Map_LVL3.png";
	case 5:
		return L"assets\\[ID5]Map_Catacombs.png";
	case 6:
		return L"assets\\[ID6]Map_Archive.png";
	case 7:
		return L"assets\\[ID7]Map_Crossroads.png";
	case 8:
		return L"assets\\[ID8]Map_Golems.png";
	case 9:
		return L"assets\\[ID9]Map_CrossroadsSewers.png";
	case 10:
		return L"assets\\[ID10]Map_Market.png";
	case 11:
		return L"assets\\[ID11]Map_MarketSewer.png";			
	default:
		return L"assets\\title.png";
	}
}

int getMapOffsetX(BYTE level) {
	switch (level)
	{
	case 2:
		return 18338;
	case 3:
		return 19363;
	case 4:
		return 19297;
	case 5:
		return 21064;
	case 6:
		return 18729;
	case 7:
		return 24401;
	case 8:
		return 19639;
	case 9:
		return 17498;
	case 10:
		return 20208;
	case 11:
		return 16853;
	default:
		return 0;		
	}
}
int getMapOffsetY(BYTE level) {
	switch (level)
	{
	case 2:
		return 18975;
	case 3:
		return 18987;
	case 4:
		return 18959;
	case 5:
		return 18999;
	case 6:
		return 18861;
	case 7:
		return 20037;
	case 8:
		return 19054;
	case 9:
		return 18559;
	case 10:
		return 20592;
	case 11:
		return 21677;
	default:
		return 0;	
	}
}
float getMapScale(BYTE level) {
	return 0.1572;
}