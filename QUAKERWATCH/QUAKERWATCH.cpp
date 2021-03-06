//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"

#include "LoginEventReceiver.h"
#include "LOOPDATA.h"
#include "TOPLEVELFLOW.h"

boost::asio::io_service io_service;

//Here is the main control flow of the program
//It's a looping switch over the loop mode
//Receivers are like self-contained parts of the game
//just create them and they do their part
//The loop_pack's mode and looprun var
//are controlled from within the receivers!
//seem to simplify things that way rather
//than handling those two variables separetely
//as members to each receiver.
int main(int argc, char* argv[])
{
	LOOP_PARAM_PACK LOOP_PACK;
	while (LOOP_PACK.mode != LOOPMODE::QUIT)
	{
		if (LOOP_PACK.mode == LOOPMODE::LOGIN || LOOP_PACK.mode == LOOPMODE::GAME_TO_QUEUE)
		{
			LoginEventReceiver login_receiver(LOOP_PACK);
		}
		else if (LOOP_PACK.mode == LOOPMODE::QUEUE)
		{
			//QueueEventReceiver queue_receiver(LOOP_PACK);
		}
		else if (LOOP_PACK.mode == LOOPMODE::GAME || LOOP_PACK.mode == LOOPMODE::RESET_DEVICE)
		{
			//GameEventReceiver game_receiver(LOOP_PACK);
		}
		else
		{
			perror("unknown loop mode FAILURE!");
			exit(EXIT_FAILURE);
		}

		if (LOOP_PACK.mode == LOOPMODE::GAME_TO_QUEUE)
		{
			GAME_TO_QUEUE_PROC(LOOP_PACK);
		}
		else if (LOOP_PACK.mode == LOOPMODE::RESET_DEVICE)
		{
			RESET_DEVICE_PROC(LOOP_PACK);
		}
		else
		{
			perror("No top level proc called!");
		}
	}
	return 0;
}