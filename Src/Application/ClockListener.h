/**
@file ClockListener.h

Contiene la declaraci�n de un interfaz para un listener del temporizador.

@author Alberto Plaza
@date Febrero, 2012
*/

#ifndef __Application_ClockListener_H
#define __Application_ClockListener_H

namespace Application
{
	/**
	Listener del reloj de la aplicaci�n.

	Cada componente que quiera ser avisado por la aplicaci�n pasado un tiempo deber�
	implementar esta interfaz cuyos m�todos ser�n llamados por el temporizador de la
	aplicaci�n y deber�n ser implementados por la clase oyente.

	@author Alberto Plaza
	@date Febrero, 2012
	*/
	class IClockListener
	{
	public:

		/**
		Constructor por defecto de la clase
		*/
		IClockListener();

		/**
		Destructor
		*/
		virtual ~IClockListener();

		/**
		M�todo que ser� llamado por el temporizador de la aplicaci�n
		para que la clase oyente sepa que ha expirado su temporizador.
		*/
		virtual void timeElapsed() = 0;

	}; // IClockListener

} // namespace Application

#endif //  __Application_ClockListener_H