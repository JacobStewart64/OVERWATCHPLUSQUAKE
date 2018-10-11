#pragma once

#include <irrlicht.h>

enum LOOPMODE {
	LOGIN = 0,
	QUEUE,
	GAME,
	GAME_TO_QUEUE,
	RESET_DEVICE,
	QUIT
};


struct LOOP_PARAM_PACK {
	irr::IrrlichtDevice* device = irr::createDevice(
		irr::video::E_DRIVER_TYPE::EDT_DIRECT3D9,
		irr::core::dimension2d<unsigned int>(800, 900), 16, false);
	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0,
		irr::core::vector3df(0, 30, -40), irr::core::vector3df(0, 5, 0));
	irr::scene::ITriangleSelector* selector = 0;
	LOOPMODE mode = LOOPMODE::LOGIN;
	bool runloop;

	LOOP_PARAM_PACK() {}
};