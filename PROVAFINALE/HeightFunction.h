//This provides functions that can be used to easily get the height of Ogre's terrain at any x/z point.
//Simply call HeightFunction::initialize(), then use HeightFunction::getTerrainHeight() as needed.

//This file is used by the PagedGeometry examples to place trees on the terrain.

#ifndef __HEIGHTFUNCTION_H__
#define __HEIGHTFUNCTION_H__

#include "Ogre.h"
#include "Terrain\OgreTerrain.h"
#include "Terrain\OgreTerrainGroup.h"

using namespace Ogre;

namespace HeightFunction
{
        class MyRaySceneQueryListener: public RaySceneQueryListener
        {
        public:
                bool queryResult(SceneQuery::WorldFragment *fragment, Real distance);
                bool queryResult(MovableObject *obj, Real distance);
                float resultDistance;
        };

        //Initializes the height function. Call this before calling getTerrainHeight()
        void initialize(SceneManager *sceneMgr);

        //Gets the height of the terrain at the specified x/z coordinate
        //The userData parameter isn't used in this implementation of a height function, since
        //there's no need for extra data other than the x/z coordinates.
        float getTerrainHeight(const float x, const float z, Ogre::TerrainGroup &tg);
}

#endif