#include "Sinbad.h"

using namespace Ogre;

Sinbad::Sinbad(Camera *mCamera, SceneManager * mSceneManager, TerrainGroup * mTerrainGroup)
	: camera(mCamera), manager(mSceneManager), terrainGroup(mTerrainGroup)
{
	HeightFunction::initialize(manager);
	entity = manager->createEntity("SinbadEntity", "sinbad.mesh");
	node = manager->getRootSceneNode()->createChildSceneNode("SinbadNode", Ogre::Vector3(0,HeightFunction::getTerrainHeight(0,0,*terrainGroup) + 15,0));
	node->scale(3.0,3.0,3.0);
	node->attachObject(entity);

	sword1 = manager->createEntity("sword1", "Sword.mesh");
	sword2 = manager->createEntity("sword2", "Sword.mesh");
	entity->attachObjectToBone("Sheath.L", sword1);
	entity->attachObjectToBone("Sheath.R", sword2);

	entity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	Ogre::String animNames[9] = { "IdleTop", "IdleBase", "RunTop", "RunBase", "SliceVertical", "SliceHorizontal", "DrawSwords", "HandsClosed", "HandsRelaxed" } ;

	for(int i=0; i<9; i++){
		animations[i] = entity->getAnimationState(animNames[i]);
		animations[i]->setEnabled(false);
	}

	for(int i=0; i<4; i++)
		animations[i]->setLoop(true);

	for(int i=4; i<9; i++)
		animations[i]->setLoop(false);

	//inizializzazioni

	timer=0.0f;
	offset = 50;  //Distanza iniziale della telecamera dal personaggio
	cameraAngle=Degree(90);
	scambioOssa=spadeEstratte=isRunning=false;
	cameraRelativePosition=Vector3(0,5,0);

}

Sinbad::~Sinbad(){}

bool Sinbad::update(const FrameEvent &evt){
	for(int i=0; i<9; i++){
		if(i!=2 && i!=3)
			animations[i]->addTime(evt.timeSinceLastFrame);
		else if(isRunning)
			animations[i]->addTime(evt.timeSinceLastFrame*2);	//raddoppio la velocità dell'animazione
		else
			animations[i]->addTime(evt.timeSinceLastFrame);

		if(animations[i]->hasEnded() && i!=6) animations[i]->setEnabled(false);
	}

	if(animations[6]->hasEnded()){
		animations[6]->setEnabled(false);
		timer=0.0f;
		scambioOssa=false;
	}

	if(animations[6]->getEnabled()){
		timer+=evt.timeSinceLastFrame;
		if(timer>animations[6]->getLength()/2 && !scambioOssa){ // sono a metà (circa) dell'animazione, scambio ossa serve per evitare di ripetere questo blocco di istruzioni più di una volta nella stessa animazione
			scambioOssa=true;
			entity->detachAllObjectsFromBone();
			if(!spadeEstratte){ //attacco le spade alle mani e le chiudo
				entity->attachObjectToBone("Handle.R", sword1);
				entity->attachObjectToBone("Handle.L", sword2);
				animations[7]->setTimePosition(0);
				animations[7]->setEnabled(true);
				spadeEstratte=true;
			} else { //attacco le spade alla schiena e riapro le mani
				entity->attachObjectToBone("Sheath.R", sword1);
				entity->attachObjectToBone("Sheath.L", sword2);
				animations[8]->setTimePosition(0);
				animations[8]->setEnabled(true);
				spadeEstratte=false;
			}
		}
	}

	//sposto il nodo a livello del terreno
	node->setPosition(Vector3(node->getPosition().x, HeightFunction::getTerrainHeight(node->getPosition().x, node->getPosition().z, *terrainGroup) + 15, node->getPosition().z));

	//calcolo la pozione relativa al personaggio (come se esso fosse in 0,0,0)
	cameraRelativePosition=Vector3(cos(cameraAngle.valueDegrees())*offset, cameraRelativePosition.y, sin(cameraAngle.valueDegrees())*offset);
	//pozione assoluta (posizione del personaggio + posizioneRelativa)
	camera->setPosition(node->getPosition() + cameraRelativePosition);
	//controllo che la camera non sia nel terreno, in quel caso la sposto più su
	if(camera->getPosition().y<HeightFunction::getTerrainHeight(camera->getPosition().x, camera->getPosition().z, *terrainGroup))
		camera->setPosition(Vector3(camera->getPosition().x, HeightFunction::getTerrainHeight(camera->getPosition().x, camera->getPosition().z, *terrainGroup) + 10, camera->getPosition().z));
	if(cameraRelativePosition.y>offset)
		cameraRelativePosition.y=offset;
	if(cameraRelativePosition.y<0)
		cameraRelativePosition.y=0;
	
	//la faccio guardare il personagio
	camera->lookAt(node->getPosition());

	return true;
}

void Sinbad::movement(Real speed, Vector3 direction, Real time){
	if(!isRunning)
		node->translate(direction * speed * time ,Ogre::Node::TS_LOCAL);
	else
		node->translate(direction * speed * 2 * time ,Ogre::Node::TS_LOCAL);
}

void Sinbad::rotation(Ogre::Real speed, Ogre::Real direction, Ogre::Real time){
	node->rotate(Vector3::UNIT_Y, Radian::Radian(direction * speed * time));
}

void Sinbad::runStart(){
	idleStop();
	if(!animations[2]->getEnabled())	animations[2]->setEnabled(true);
	if(!animations[3]->getEnabled())	animations[3]->setEnabled(true);
}

void Sinbad::runStop(){
	if(animations[2]->getEnabled())		animations[2]->setEnabled(false);
	if(animations[3]->getEnabled())		animations[3]->setEnabled(false);
	idleStart();
}

void Sinbad::setRunning(bool run){
	isRunning=run;
}

void Sinbad::idleStart(){
	if(!animations[0]->getEnabled())	animations[0]->setEnabled(true);
	if(!animations[1]->getEnabled())	animations[1]->setEnabled(true);
}

void Sinbad::idleStop(){
	if(animations[0]->getEnabled())		animations[0]->setEnabled(false);
	if(animations[1]->getEnabled())		animations[1]->setEnabled(false);
}

void Sinbad::drawSwords(){
	if(!animations[6]->getEnabled()){
		animations[6]->setTimePosition(0);
		animations[6]->setEnabled(true);
	}
}

void Sinbad::mouseMoved(const OIS::MouseEvent &e){
	
	float speed = 0.01f;	//sensibilità del mouse

	if(e.state.X.rel)		//cambio l'angolo della telecamera
		cameraAngle-=Degree(e.state.X.rel * speed);

	float ySpeed = 0.005f; //sensibilità verticale del mouse

	if(e.state.Y.rel)
		cameraRelativePosition.y-= e.state.Y.rel * ySpeed * offset; //metto anche offset perchè man mano che la telecamera si allontana il movimento deve essere più veloce

	float zoomSpeed = 0.1f;	//velocità con cui si zoomma sul personaggio

	if(e.state.Z.rel)		//cambio la distanza tra il personaggio e la telecamera
		offset-=e.state.Z.rel * zoomSpeed;

	if(offset<20)			//minima distanza
		offset=20;

	if(offset>500)			//massima distanza
		offset=500;
}

void Sinbad::slice(Real orientation){ // 0 verticale, 1 orizzontale
	if(spadeEstratte){
		if(orientation){
			if(!animations[5]->getEnabled()){
				animations[5]->setTimePosition(0);
				animations[5]->setEnabled(true);
			}
		} else {
			if(!animations[4]->getEnabled()){
				animations[4]->setTimePosition(0);
				animations[4]->setEnabled(true);
			}
		}
	}
}