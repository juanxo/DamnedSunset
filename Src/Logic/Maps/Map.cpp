/**
@file Map.cpp

Contiene la implementaci�n de la clase CMap, Un mapa l�gico.

@see Logic::Map

@author David Llans�
@date Agosto, 2010
*/

#include "Map.h"

#include "Logic/Entity/Entity.h"
#include "EntityFactory.h"
#include "GridMap.h"

#include "Map/MapParser.h"
#include "Map/MapEntity.h"

#include "Graphics/Server.h"
#include "Graphics/Scene.h"

#include "AI/Server.h"

#include "ScriptManager/Server.h"


#include <cassert>

// HACK. Deber�a leerse de alg�n fichero de configuraci�n
#define MAP_FILE_PATH "./media/maps/"

namespace Logic {
		

	void CMap::setAtributosArquetipos(Map::CEntity* entidad, Map::CMapParser::TEntityList & entityList) {
		Map::CMapParser::TEntityList::const_iterator it, end;
		it = entityList.begin();
		end = entityList.end();

		for(; it != end; it++)
		{
			if ((*it)->getType() == entidad->getType()) {	
				std::map<std::string, std::string> tAttrList = (*it)->getAttributes();

				std::map<std::string, std::string>::const_iterator it2, end2;
				it2 = tAttrList.begin();
				end2 = tAttrList.end();
				// Creamos todas las entidades l�gicas.
				for(; it2 != end2; it2++) {
					if (!entidad->hasAttribute((*it2).first)) {
						entidad->setAttribute((*it2).first,(*it2).second);
					}
				}

			}
		}
	}
	
	CMap* CMap::createMapFromFile(const std::string &filename)
	{
		//----------------------------------------------------

		ScriptManager::CServer::getSingletonPtr()->loadExeScript("map");
		ScriptManager::CServer::getSingletonPtr()->loadExeScript("mapParser");

		if (ScriptManager::CServer::getSingletonPtr()->executeScript("processMap(Map)"))
			int n = 0;

		//----------------------------------------------------

		// Completamos la ruta con el nombre proporcionado
		std::string completePath(MAP_FILE_PATH);
		completePath = completePath + filename;
		// Parseamos el fichero
		if(!Map::CMapParser::getSingletonPtr()->parseFile(completePath))
		{
			assert(!"No se ha podido parsear el mapa.");
			return false;
		}

		// Si se ha realizado con �xito el parseo creamos el mapa.
		//TODO: (LUA) Incluir el tama�o del mapa cuando se parsee del nuevo archivo map.lua
		CMap *map = new CMap(filename);

		// Extraemos las entidades del parseo.
		Map::CMapParser::TEntityList entityList = Map::CMapParser::getSingletonPtr()->getEntityList();
		// Transformamos sus posiciones del editor a posiciones del juego.
		CEntityFactory* entityFactory = CEntityFactory::getSingletonPtr();

		//** PARSEMOS EL ARCHIVO DE ARQUETIPOS**/
		// Cambiamos el nombre del fichero txt para que en "entityListArquetipos" guarde la informaci�n de "arquetipos.txt""
		std::string completePath2(MAP_FILE_PATH);
		completePath2.append("arquetipos.txt");
		if(!Map::CMapParser::getSingletonPtr()->parseFile(completePath2))
		{
			assert(!"No se ha podido parsear el mapa de arquetipos.");
			return false;
		}
		// Completamos la ruta con el nombre proporcionado

		// Guardamos la informaci�n de las entidades le�das de los arquetipos.
		// En el MapParser ahora estar� la informaci�n de arquetipos.txt, ya que lo hemos cambiado anteriormente.
		Map::CMapParser::TEntityList entityListArquetipos = Map::CMapParser::getSingletonPtr()->getEntityList();

		Map::CMapParser::TEntityList::const_iterator it, end;
		it = entityList.begin();
		end = entityList.end();

		// Creamos todas las entidades l�gicas.
		for(; it != end; it++)
		{
			if ((*it)->getType() == "Waypoint") {
				// Procesar waypoint del grafo de navegaci�n
				//AI::CServer::getSingletonPtr()->addWaypoint((*it)->getVector3Attribute("position"));
				// Descomentar la siguiente l�nea para dibujar los nodos del grafo de navegaci�n
				entityFactory->createEntity((*it), map);
			} else {
				if ((*it)->getType() == "Grid") {

					// Recupero las dimensiones y el tama�o del grid del mapa y lo inicializo.
					if (((*it)->hasAttribute("width")) && ((*it)->hasAttribute("height")) && ((*it)->hasAttribute("grid_size"))) {
						map->getGridMap()->initMap((*it)->getIntAttribute("width"), (*it)->getIntAttribute("height"), (*it)->getIntAttribute("grid_size"));

					} else {
						assert(!"No se han encontrado dimensiones y/o tama�o del grid del mapa.");
						return false;
					}
				} else {

					// La propia factor�a se encarga de a�adir la entidad al mapa.
					setAtributosArquetipos(*it, entityListArquetipos);

					if (!entityFactory->createEntity((*it),map)) {
						// @TODO @GRID Cuando sepamos qu� hacer con cada casilla aqu� habr� que hacer algo mas complejo que simplemente poner el tipo.
						// Por ahora lo hago "a pelo" pero igual vendr�a bien una factor�a de casillas o algo as�.
						map->getGridMap()->getTileFromCoord((*it)->getVector2Attribute("position").x, (*it)->getVector2Attribute("position").y)->SetTerrain((*it)->getIntAttribute("terrain"));
					}
				}
			}
		}
		Map::CMapParser::releaseEntityList(entityList);
		//Map::CMapParser::releaseEntityList(entityList2);
		Map::CMapParser::getSingletonPtr()->setEntityList(entityListArquetipos);

		return map;

	} // createMapFromFile

	//--------------------------------------------------------

	CMap::CMap(const std::string &name)
	{
		_name = name;
		_scene = Graphics::CServer::getSingletonPtr()->createScene(name);
		_gridMap = new CGridMap();

		// Se le a�ade el mapa al servidor de IA para tener una referencia
		// y poder calcular rutas con A*
		AI::CServer::getSingletonPtr()->getNavigationMap()->setGridMap(_gridMap);

	} // CMap

	//--------------------------------------------------------

	CMap::~CMap()
	{
		destroyAllEntities();

		AI::CServer::getSingletonPtr()->getNavigationMap()->setGridMap(NULL);
		delete(_gridMap);

		if(Graphics::CServer::getSingletonPtr())
			Graphics::CServer::getSingletonPtr()->removeScene(_scene);

	} // ~CMap

	//--------------------------------------------------------

	bool CMap::activate()
	{
		Graphics::CServer::getSingletonPtr()->setScene(_scene);

		TEntityMap::const_iterator it, end;
		end = _entityMap.end();
		it = _entityMap.begin();

		bool correct = true;

		// Activamos todas las entidades registradas en el mapa.
		for(; it != end; it++)
			correct = (*it).second->activate() && correct;

		return correct;

	} // getEntity

	//--------------------------------------------------------

	void CMap::deactivate()
	{
		TEntityMap::const_iterator it, end;
		end = _entityMap.end();
		it = _entityMap.begin();

		// Desactivamos todas las entidades activas registradas en el mapa.
		for(; it != end; it++)
			if((*it).second->isActivated())
				(*it).second->deactivate();

		Graphics::CServer::getSingletonPtr()->setScene(0);

	} // getEntity

	//---------------------------------------------------------

	void CMap::tick(unsigned int msecs) 
	{
		TEntityMap::const_iterator it;

		for( it = _entityMap.begin(); it != _entityMap.end(); ++it )
			(*it).second->tick(msecs);

	} // tick

	//--------------------------------------------------------

	typedef std::pair<TEntityID,CEntity*> TEntityPair;

	void CMap::addEntity(CEntity *entity)
	{
		if(_entityMap.count(entity->getEntityID()) == 0)
		{
			TEntityPair elem(entity->getEntityID(),entity);
			_entityMap.insert(elem);
		}

	} // addEntity

	//--------------------------------------------------------

	void CMap::removeEntity(CEntity *entity)
	{
		if(_entityMap.count(entity->getEntityID()) != 0)
		{
			if(entity->isActivated())
				entity->deactivate();
			entity->_map = 0;
			_entityMap.erase(entity->getEntityID());
		}

	} // removeEntity

	//--------------------------------------------------------

	void CMap::destroyAllEntities()
	{
		CEntityFactory* entityFactory = CEntityFactory::getSingletonPtr();

		TEntityMap::const_iterator it, end;
		it = _entityMap.begin();
		end = _entityMap.end();

		// Eliminamos todas las entidades. La factor�a se encarga de
		// desactivarlas y sacarlas previamente del mapa.
		while(it != end)
		{
			CEntity* entity = (*it).second;
			it++;
			entityFactory->deleteEntity(entity);
		}

		_entityMap.clear();

	} // removeEntity

	//--------------------------------------------------------

	CEntity* CMap::getEntityByID(TEntityID entityID)
	{
		if(_entityMap.count(entityID) == 0)
			return 0;
		return (*_entityMap.find(entityID)).second;

	} // getEntityByID

	//--------------------------------------------------------

	CEntity* CMap::getEntityByName(const std::string &name, CEntity *start)
	{
		TEntityMap::const_iterator it, end;
		end = _entityMap.end();

		// Si se defini� entidad desde la que comenzar la b�squeda 
		// cogemos su posici�n y empezamos desde la siguiente.
		if (start)
		{
			it = _entityMap.find(start->getEntityID());
			// si la entidad no existe devolvemos NULL.
			if(it == end)
				return 0;
			it++;
		}
		else
			it = _entityMap.begin();

		for(; it != end; it++)
		{
			// si hay coincidencia de nombres devolvemos la entidad.
			if (!(*it).second->getName().compare(name))
				return (*it).second;
		}
		// si no se encontr� la entidad devolvemos NULL.
		return 0;

	} // getEntityByName

	//--------------------------------------------------------

	CEntity* CMap::getEntityByType(const std::string &type, CEntity *start)
	{
		TEntityMap::const_iterator it, end;
		end = _entityMap.end();

		// Si se defini� entidad desde la que comenzar la b�squeda 
		// cogemos su posici�n y empezamos desde la siguiente.
		if (start)
		{
			it = _entityMap.find(start->getEntityID());
			// si la entidad no existe devolvemos NULL.
			if(it == end)
				return 0;
			it++;
		}
		else
			it = _entityMap.begin();

		for(; it != end; it++)
		{
			// si hay coincidencia de nombres devolvemos la entidad.
			if (!(*it).second->getType().compare(type))
				return (*it).second;
		}
		// si no se encontr� la entidad devolvemos NULL.
		return 0;

	} // getEntityByType

} // namespace Logic
