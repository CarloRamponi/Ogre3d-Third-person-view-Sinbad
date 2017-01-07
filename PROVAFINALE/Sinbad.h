#ifndef __SINBAD_H__
#define __SINBAD_H__

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#  include <OIS/OISEvents.h>
#  include <OIS/OISInputManager.h>
#  include <OIS/OISKeyboard.h>
#  include <OIS/OISMouse.h>

#  include <OGRE/SdkTrays.h>
#  include <OGRE/SdkCameraMan.h>
#else
#  include <OISEvents.h>
#  include <OISInputManager.h>
#  include <OISKeyboard.h>
#  include <OISMouse.h>

#  include <SdkTrays.h>
#  include <SdkCameraMan.h>
#endif

#ifdef OGRE_STATIC_LIB
#  define OGRE_STATIC_GL
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define OGRE_STATIC_Direct3D9
#    if OGRE_USE_D3D10
#      define OGRE_STATIC_Direct3D10
#    endif
#  endif
#  define OGRE_STATIC_BSPSceneManager
#  define OGRE_STATIC_ParticleFX
#  define OGRE_STATIC_CgProgramManager
#  ifdef OGRE_USE_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#  else
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#  include "OgreStaticPluginLoader.h"
#endif

#include "HeightFunction.h"

class Sinbad{

protected:

	Ogre::SceneManager * manager;
	Ogre::TerrainGroup * terrainGroup;
	Ogre::Camera * camera;
	Ogre::Entity * entity;
	Ogre::SceneNode * node;
	Ogre::Entity * sword1;
	Ogre::Entity * sword2;
	Ogre::AnimationState * animations[9];
	float timer;
	bool scambioOssa, spadeEstratte, isRunning;
	Ogre::Vector3 cameraRelativePosition;
	Ogre::Degree XAngle, YAngle;
	unsigned int offset;

public:
	~Sinbad();
	Sinbad(Ogre::Camera *mCamera, Ogre::SceneManager * mSceneManager, Ogre::TerrainGroup * mTerrainGroup);
	bool setCamera(Ogre::Camera * mCamera);
	bool update(const Ogre::FrameEvent &evt, bool backward);
	void movement(Ogre::Real speed, Ogre::Vector3 direction, Ogre::Real time);
	void rotation(Ogre::Real speed, Ogre::Real direction, Ogre::Real time);
	void runStart();
	void runStop();
	void idleStart();
	void idleStop();
	void drawSwords();
	void setRunning(bool run);
	void slice(Ogre::Real orientation);
	void mouseMoved(const OIS::MouseEvent &e);

};

#endif