#pragma once

#include "LOOPDATA.h"

void GAME_TO_QUEUE_PROC(LOOP_PARAM_PACK& LOOP_PACK)
{
	
}

void RESET_DEVICE_PROC(LOOP_PARAM_PACK& LOOP_PACK)
{

}

void SET_WINDOW_CAPTION(const wchar_t* window_caption, LOOP_PARAM_PACK& LOOP_PACK)
{
	LOOP_PACK.device->setWindowCaption(window_caption);
}
