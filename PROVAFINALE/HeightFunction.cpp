#include "HeightFunction.h"


namespace HeightFunction{

	bool MyRaySceneQueryListener::queryResult(SceneQuery::WorldFragment *fragment, Real distance){
		resultDistance = distance;
		return false;
	}

	bool MyRaySceneQueryListener::queryResult(MovableObject *obj, Real distance)
	{
		resultDistance = distance;
		return false;
	}

	bool initialized = false;
	RaySceneQuery* raySceneQuery;
	Ray updateRay;
	MyRaySceneQueryListener *raySceneQueryListener;

	void initialize(SceneManager *sceneMgr){
		if (!initialized){
			initialized = true;
			updateRay.setOrigin(Vector3::ZERO);
			updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
			raySceneQuery = sceneMgr->createRayQuery(updateRay);
			raySceneQueryListener = new MyRaySceneQueryListener;
		}
	}

	float getTerrainHeight(const float x, const float z, Ogre::TerrainGroup &tg){
		updateRay.setOrigin(Vector3(x, 0.0f, z));
		updateRay.setDirection(Vector3::UNIT_Y);
		Ogre::TerrainGroup::RayResult RayResult = tg.rayIntersects( updateRay );
		if( RayResult.hit )
		{
			return RayResult.position.y;
		}
		else return float(40.0);
	}

}