/*
	Copyright 2012 - Le Padellec Sylvain

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef ONTICKLISTENER
#define ONTICKLISTENER

#include "Misc/temp_basiclist.h"
#include "Players/NczPlayerManager.h"

/////////////////////////////////////////////////////////////////////////
// BaseFramedTester
/////////////////////////////////////////////////////////////////////////

class OnTickListener
{
	typedef basic_slist<OnTickListener *> TickListenersListT;

private:
	static TickListenersListT m_tickTestersList;

	SlotStatus m_filter;

public:
	OnTickListener();

	virtual ~OnTickListener(){};

	/* Appel� par le plugin � chaque frame
	   Permet d'appeler les classes filles qui sont � l'�coute */
	static void OnTick();

	/* Filtre du testeur, peut/doit �tre red�finit dans la classe fille */
	virtual SlotStatus GetFilter() const {return m_filter;};

protected:
	/* Appel� par OnTick, sert � i�trer entre les diff�rents joueurs en fonction du filtre */
	virtual void ProcessTestsOnTick(float const curtime);

	/* Appel� par ProcessTestsOnTick() lorsque le joueur correspond au filtre */
	virtual void ProcessPlayerTestOnTick(PlayerHandler::const_iterator ph, float const curtime) = 0;

	virtual void ProcessOnTick(float const curtime) = 0;

	/* Permet de se mettre � l'�coute de l'�v�nement, appel� par Load/Unload des testeurs */
	static void RegisterOnTickListener(OnTickListener const * const listener);

	static void RemoveOnTickListener(OnTickListener const * const listener);
};

#endif // ONTICKLISTENER
