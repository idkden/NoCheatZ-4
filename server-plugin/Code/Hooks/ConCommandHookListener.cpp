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

#include "ConCommandHookListener.h"

#include "Interfaces/InterfacesProxy.h"

#include "Misc/Helpers.h"
#include "plugin.h"
#include "Systems/ConfigManager.h"

ConCommandHookListener::ConCommandListenersListT ConCommandHookListener::m_listeners;

ConCommandHookListener::ConCommandHookListener()
{
}

ConCommandHookListener::~ConCommandHookListener()
{
}

void ConCommandHookListener::HookDispatch(void* cmd )
{
	HookInfo info(cmd, ConfigManager::GetInstance()->vfid_dispatch, (DWORD)nDispatch);
	HookGuard::GetInstance()->VirtualTableHook(info);
}

#ifdef GNUC
void HOOKFN_INT ConCommandHookListener::nDispatch(void* cmd, SourceSdk::CCommand const &args )
#else
void HOOKFN_INT ConCommandHookListener::nDispatch(void* cmd, void*, SourceSdk::CCommand const &args )
#endif
{
	const int index = GET_PLUGIN_COMMAND_INDEX();
	bool bypass = false;
	if(index >= PLUGIN_MIN_COMMAND_INDEX && index <= PLUGIN_MAX_COMMAND_INDEX)
	{
		PlayerHandler::const_iterator ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(index);

		if (ph > INVALID)
		{

#ifdef DEBUG
		printf("Testing ConCommand %s of %s\n", SourceSdk::InterfacesProxy::ConCommand_GetName(cmd), ph->GetName());
#endif

			ConCommandListenersListT::elem_t* it = m_listeners.GetFirst();
			while (it != nullptr)
			{
				int const c = it->m_value.listener->m_mycommands.Find(cmd);
				if (c != -1)
				{
					bypass |= it->m_value.listener->ConCommandCallback(ph, cmd, args);
				}
				it = it->m_next;
			}

#ifdef DEBUG
			printf("Bypassed ConCommand %s of %s\n", SourceSdk::InterfacesProxy::ConCommand_GetName(cmd), ph->GetName());
#endif
		}
	}
	else if(index == 0)
	{
		bypass = false;

		ConCommandListenersListT::elem_t* it = m_listeners.GetFirst();
		while (it != nullptr)
		{
			int const c = it->m_value.listener->m_mycommands.Find(cmd);
			if (c != -1)
			{
				bypass |= it->m_value.listener->ConCommandCallback(PlayerHandler::end(), cmd, args);
			}
			it = it->m_next;
		}
	}

	if(!bypass)
	{
		//Assert(it != nullptr);

		ST_W_STATIC Dispatch_t gpOldFn;
		*(DWORD*)&(gpOldFn) = HookGuard::GetInstance()->GetOldFunction(cmd, ConfigManager::GetInstance()->vfid_dispatch);
		gpOldFn(cmd, args);
	}
	else
	{
#ifdef DEBUG
		printf("Bypassed ConCommand %s of %d\n", SourceSdk::InterfacesProxy::ConCommand_GetName(cmd), index);
#endif
	}
}

void ConCommandHookListener::RegisterConCommandHookListener(ConCommandHookListener const * const listener)
{
	Assert(!listener->m_mycommands.IsEmpty());

	ConCommandListenersListT::elem_t* it = m_listeners.FindByListener(listener);
	if(it == nullptr)
	{
		it = m_listeners.Add(listener);
	}

	size_t cmd_pos = 0;
	size_t const max_pos = listener->m_mycommands.Size();
	do
	{
		HookDispatch(listener->m_mycommands[cmd_pos]);
	} while (++cmd_pos != max_pos);
}

void ConCommandHookListener::RemoveConCommandHookListener(ConCommandHookListener * const listener)
{
	listener->m_mycommands.RemoveAll();
	m_listeners.Remove(listener);
}

