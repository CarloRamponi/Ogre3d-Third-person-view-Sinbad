/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
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

#include "TutorialApplication.h"

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
	: mTerrainGlobals(0), mTerrainGroup(0), mTerrainsImported(false)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//---------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f,0.7f,0.7f));
	Light *mLight = mSceneMgr->createLight("Sun");
	mLight->setType(Light::LT_DIRECTIONAL);
	mLight->setDiffuseColour(1.0, 0.6, 0.0);
	mLight->setSpecularColour(1,1,1);
	mLight->setAttenuation(8000, 1, 0.0005, 0);
	mLight->setDirection(Vector3(1.0f, -0.25f, 0.0f));
	mSceneMgr->setSkyBox(true, "SkyBox");
	Ogre::Plane oceanSurface;
	oceanSurface.normal = Ogre::Vector3::UNIT_Y;
	int width = 10000;
	int height = 10000;
	int xsegment = 50;
	int ysegment = 50;
	Ogre::MeshManager::getSingleton().createPlane("OceanSurfaceMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, oceanSurface, width, height, xsegment, ysegment, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
	Ogre::Entity *oceanEntity = mSceneMgr->createEntity("OceanSurfaceEntity", "OceanSurfaceMesh");
	Ogre::SceneNode * oceanNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("OceanNode", Ogre::Vector3(0,30,0));
	oceanNode->attachObject(oceanEntity);
	oceanEntity->setMaterialName("Ocean2_Cg");
	this->createTerrain(mLight);
	
	sinbad = new Sinbad(mCamera, mSceneMgr, mTerrainGroup);

	StaticGeometry* mStaticGeometry = mSceneMgr->createStaticGeometry("Trees");
	mStaticGeometry->setRegionDimensions(Vector3(10000, 10000, 1000));
	Ogre::Entity * tree[100];
	Quaternion orientation;
	orientation.FromAngleAxis(Degree(180), Vector3::UNIT_Y);
	for(int i=0;i<100;i++){
		std::string nome = "tree";
		nome.append(std::to_string(i));
		tree[i]=mSceneMgr->createEntity(nome.c_str(), "PineTree.mesh");
		int x = Math::RangeRandom(-1000,1000);
		int z = Math::RangeRandom(-1000,1000);
		mStaticGeometry->addEntity(tree[i], Vector3(x, HeightFunction::getTerrainHeight(x,z, *mTerrainGroup),z), orientation, Vector3(5.0, 5.0, 5.0));
	}
	mStaticGeometry->build();
}
//---------------------------------------------------------------------------

void TutorialApplication::createTerrain(Ogre::Light *light)
{
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 4500.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("TutorialApplication"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
    configureTerrainDefaults(light);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
 
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }
 
    mTerrainGroup->freeTemporaryResources();
}
void TutorialApplication::destroyScene(void)
{
    OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain3333333.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
 
}
//-------------------------------------------------------------------------------------
void TutorialApplication::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}
//-------------------------------------------------------------------------------------
void TutorialApplication::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}
//-------------------------------------------------------------------------------------
void TutorialApplication::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(3000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;

    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

}
//---------------------------------------------------------------------------
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	//movimento
	Ogre::Vector3 move(0,0,0);
	int rot = 0;
	bool isMoving = false;

	if(mKeyboard->isKeyDown(OIS::KC_K) || mKeyboard->isKeyDown(OIS::KC_S))   // move forward
	{
		isMoving = true;
		move.z = -1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_I) || mKeyboard->isKeyDown(OIS::KC_W))   // move back
	{
		isMoving = true;
		move.z = 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_L) || mKeyboard->isKeyDown(OIS::KC_D))
	{
		if(move.z == -1.0f)
			rot+=1;
		else
			rot-=1;
	}
	if(mKeyboard->isKeyDown(OIS::KC_J) || mKeyboard->isKeyDown(OIS::KC_A))
	{
		if(move.z == -1.0f)
			rot-=1;
		else
			rot+=1;
	}
	
	if(mKeyboard->isKeyDown(OIS::KC_RSHIFT) || mKeyboard->isKeyDown(OIS::KC_LSHIFT))
	{
		sinbad->setRunning(true);
	}
	else
	{
		sinbad->setRunning(false);
	}
	if(mKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		sinbad->drawSwords();
	}
	if(mMouse->getMouseState().buttonDown(OIS::MB_Left))
	{
		sinbad->slice(1);
	}
	if(mMouse->getMouseState().buttonDown(OIS::MB_Right))
	{
		sinbad->slice(0);
	}

	int rotSpeed = 2;

	//rotazione
	sinbad->rotation(rotSpeed, rot, evt.timeSinceLastFrame);
	

	bool backward;

	if(move.z == -1)
		backward = true;
	else
		backward = false;

	
	move.normalise();
	Real speed=40;
	sinbad->movement(speed, move, evt.timeSinceLastFrame);
	
	if(isMoving)
		sinbad->runStart();
	else
		sinbad->runStop();

	sinbad->update(evt, backward);

	return BaseApplication::frameRenderingQueued(evt);
}
//---------------------------------------------------------------------------

bool TutorialApplication::mouseMoved(const OIS::MouseEvent &e)
{

	sinbad->mouseMoved(e);

	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
