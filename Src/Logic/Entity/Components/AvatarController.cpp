/**
@file AvatarController.cpp

Contiene la implementaci�n del componente que controla el movimiento 
de la entidad.
 
@see Logic::CAvatarController
@see Logic::IComponent

@author David Llans�
@date Agosto, 2010
*/

#include "AvatarController.h"

#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"

// Includes para pruebas de paso de mensajes a componentes de IA
#include "AI/Server.h"
#include "AI/Movement.h"

#include "Logic/Entity/Messages/SetAnimation.h"


namespace Logic 
{
	IMP_FACTORY(CAvatarController);
	
	//---------------------------------------------------------

	bool CAvatarController::spawn(CEntity *entity, CMap *map, const Map::CEntity *entityInfo) 
	{
		if(!IComponent::spawn(entity,map,entityInfo))
			return false;
		
		if(entityInfo->hasAttribute("speed"))
			_speed = entityInfo->getFloatAttribute("speed");

		return true;

	} // spawn
	
	//---------------------------------------------------------

	bool CAvatarController::activate()
	{
		return true;
	} // activate
	
	//---------------------------------------------------------

	void CAvatarController::deactivate()
	{

	} // deactivate
	
	//---------------------------------------------------------

	bool CAvatarController::accept(const TMessage &message)
	{
		return message._type == Message::CONTROL;

	} // accept
	
	//---------------------------------------------------------

	void CAvatarController::process(const TMessage &message)
	{
		switch(message._type)
		{
		case Message::CONTROL:
			if(!message._string.compare("walk"))
				walk();
			else if(!message._string.compare("walkBack"))
				walkBack();
			else if(!message._string.compare("stopWalk"))
				stopWalk();
			else if(!message._string.compare("strafeLeft"))
				strafeLeft();
			else if(!message._string.compare("strafeRight"))
				strafeRight();
			else if(!message._string.compare("stopStrafe"))
				stopStrafe();
			else if(!message._string.compare("specialAction"))
				specialAction();
			else if(!message._string.compare("turn"))
				turn(message._float);
		}

	} // process
	
	//---------------------------------------------------------

	void CAvatarController::turn(float amount) 
	{
		_entity->yaw(amount);

	} // turn
	
	//---------------------------------------------------------

	void CAvatarController::walk() 
	{
		_walking = true;

		// Cambiamos la animaci�n
		CSetAnimation *m = new CSetAnimation();

		m->setAnimationName("Walk");
		m->setLoop(true);

		_entity->emitMessage(m, this);

	} // walk
	
	//---------------------------------------------------------

	void CAvatarController::walkBack() 
	{
		_walkingBack = true;

		// Cambiamos la animaci�n
		CSetAnimation *m = new CSetAnimation();

		m->setAnimationName("WalkBack");
		m->setLoop(true);

		_entity->emitMessage(m, this);

	} // walkBack
	
	//---------------------------------------------------------

	void CAvatarController::stopWalk() 
	{
		_walking = _walkingBack = false;

		// Cambiamos la animaci�n si no seguimos desplaz�ndonos
		// lateralmente
		if(!(_strafingLeft || _strafingRight))
		{
			CSetAnimation *m = new CSetAnimation();

			m->setAnimationName("Idle");
			m->setLoop(true);

			_entity->emitMessage(m, this);
		}

	} // stopWalk
	
	//---------------------------------------------------------

	void CAvatarController::strafeLeft() 
	{
		_strafingLeft = true;

		// Cambiamos la animaci�n
		CSetAnimation *m = new CSetAnimation();

		m->setAnimationName("StrafeLeft");
		m->setLoop(true);

		_entity->emitMessage(m, this);

	} // walk
	
	//---------------------------------------------------------

	void CAvatarController::strafeRight() 
	{
		_strafingRight = true;

		// Cambiamos la animaci�n
		CSetAnimation *m = new CSetAnimation();

		m->setAnimationName("StrafeRight");
		m->setLoop(true);

		_entity->emitMessage(m, this);

	} // walkBack
	
	//---------------------------------------------------------
	void CAvatarController::specialAction() 
	{
		// TODO PR�CTICA IA
		// Este m�todo es llamado cuando se activa la acci�n especial del jugador (por defecto con la tecla E).
		// Aqu� se pueden enviar mensajes para testear el funcionamiento de otros componentes.
		AI::CWaypointGraph *wg = AI::CServer::getSingletonPtr()->getNavigationGraph();
		int waypoints = wg->getWaypointCount();
		int targetId = rand() % waypoints;
		Vector3 target = wg->getWaypoint(targetId);
		std::cout << "Navigating to: " << target << std::endl;
		TMessage message2;
		message2._type = Message::ROUTE_TO;
		message2._vector3 = target;
		_entity->emitMessage(message2, this);
	} // specialAction
	
	//---------------------------------------------------------

	void CAvatarController::stopStrafe() 
	{
		_strafingLeft = _strafingRight = false;

		// Cambiamos la animaci�n si no seguimos andando
		if(!(_walking || _walkingBack))
		{
			CSetAnimation *m = new CSetAnimation();

			m->setAnimationName("Idle");
			m->setLoop(true);

			_entity->emitMessage(m, this);
		}

	} // stopWalk
	
	//---------------------------------------------------------

	void CAvatarController::tick(unsigned int msecs)
	{
		IComponent::tick(msecs);

		// Si nos estamos desplazando calculamos la pr�xima posici�n
		// Calculamos si hay vectores de direcci�n de avance y strafe,
		// hayamos la direcci�n de la suma y escalamos seg�n la
		// velocidad y el tiempo transcurrido.
		if(_walking || _walkingBack || _strafingLeft || _strafingRight)
		{
			Vector3 direction(Vector3::ZERO);
			Vector3 directionStrafe(Vector3::ZERO);

			if(_walking || _walkingBack)
			{
				direction = Math::getDirection(_entity->getYaw());
				if(_walkingBack)
					direction *= -1;
			}

			if(_strafingLeft || _strafingRight)
			{
				directionStrafe = 
						Math::getDirection(_entity->getYaw() + Math::PI/2);
				if(_strafingRight)
					directionStrafe *= -1;
			}

			direction += directionStrafe;
			direction.normalise();
			direction *= msecs * _speed;

			// Enviar un mensaje para que el componente f�sico mueva el personaje
			TMessage message;
			message._type = Message::AVATAR_WALK;
			message._vector3 = direction;
			_entity->emitMessage(message);

			//Vector3 newPosition = _entity->getPosition() + direction;
			//_entity->setPosition(newPosition);
		}

	} // tick

} // namespace Logic

