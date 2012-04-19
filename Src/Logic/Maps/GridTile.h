
/**
@file GridTile.h

Contiene la declaraci�n de la clase CGridTile, Las celdas del mapa l�gico.

@see Logic::GridTile

@author Daniel Flamenco
@date Febrero, 2012
*/
#ifndef __Logic_GridTile_H
#define __Logic_GridTile_H

#include "Logic/Entity/Entity.h"

// Declaraci�n de la clase
namespace Logic
{

	/**
	Clase que contiene el tipo de datos de un tile del mapa.
	Tiene una serie de atributos que se utilizan para almacenar
	las entidades y el terreno que tiene cada casilla del mapa.

	@author Daniel Flamenco Gonz�lez
	@date Febrero, 2012
	*/
	class CGridTile
	{
	public:
		/**
		Constructor.
		*/
		CGridTile (unsigned int row, unsigned int col) : _row(row), _col(col), _terrain(0), _building(NULL) {}

		/**
		Destructor.
		*/
		~CGridTile() {}

		/**
		Devuelve la fila en la que se encuentra la casilla dentro de la tabla.
		*/
		unsigned int GetRow() { return _row; }

		/**
		Devuelve la columna en la que se encuentra la casilla dentro de la tabla.
		*/
		unsigned int GetCol() { return _col; }

		/**
		Pone el tipo de terreno de la casilla.
		*/
		void SetTerrain(int terrainType) { _terrain = terrainType; }

		/**
		Obtiene el tipo de terreno de la casilla.
		*/
		int GetTerrain() { return _terrain; }

		/**
		Establece el edificio que se encuentra en la casilla.
		*/
		void SetBuilding(CEntity* building) { _building = building; }

		/**
		Elimina el edificio que tiene la casilla.
		*/
		void DeleteBuilding() { _building = NULL; }

		/**
		Obtiene el edificio que se encuentra en la casilla.

		Devuelve NULL si no hay edificio.
		*/
		CEntity* GetBuilding() { return _building; }

		/**
		Indica si la casilla est� ocupada por un edificio o tiene
		alguna unidad en ella.

		@return true Si hay un edificio construido, o cuando hay una unidad
		en la casilla.
		*/
		bool IsPopulated() { return _building != NULL; }

		/**
		A�ade una nueva entidad a la casilla.
		NOT IMPLEMENTED
		*/
		void AddUnit(CEntity* entity) { /*_entityList.insert(std::pair<TEntityID, CEntity*>(entity->getEntityID(), entity));*/ }

		/**
		Elimina una entidad de la casilla.
		NOT IMPLEMENTED
		*/
		void DeleteUnit(CEntity* entity) { /*_entityList.erase(entity->getEntityID());*/ }

		void FillData(){} //Map::CMapParser::TTileMatrix tileMatrix){}

	private:

		/**
		Atributos que indican la columna y la fila en las que se encuentra
		la casilla dentro de la tabla.
		*/
		int _row, _col;

		/**
		Atributo que almacena el tipo de terreno que hay en la casilla.
		*/
		int _terrain;

		/**
		Atributo para almacenar la entidad de un edificio.
		*/
		CEntity* _building;

		/**
		Lista de agentes que se encuentran en la casilla.
		*/
		//std::map<TEntityID, CEntity*> _entityList;

	};

}

#endif // __Logic_GridTile_H