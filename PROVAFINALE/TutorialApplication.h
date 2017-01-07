/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include "Sinbad.h"
#include "HeightFunction.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"

//---------------------------------------------------------------------------

class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
    virtual void createScene(void);
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	Sinbad * sinbad;

	Ogre::TerrainGlobalOptions * mTerrainGlobals;
	Ogre::TerrainGroup * mTerrainGroup;
	bool mTerrainsImported;
	virtual bool mouseMoved(const OIS::MouseEvent &e);
	void createTerrain(Ogre::Light * light);
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain * terrain);
	void configureTerrainDefaults(Ogre::Light * light);
	virtual void destroyScene(void);
};

//---------------------------------------------------------------------------

#endif // #ifndef __TutorialApplication_h_

//---------------------------------------------------------------------------
