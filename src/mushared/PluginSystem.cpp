////////////////////////////////////////////////////////////////////////////////
// PluginSystem.cpp
#include "StdAfx.h"
#include "PluginSystem.h"

CPluginSystem g_IGCPlugins;

CPluginSystem::CPluginSystem()
{
	this->m_Data.active = false;
	this->m_Data.mask = 0;
}

CPluginSystem::~CPluginSystem()
{

}

bool CPluginSystem::CheckPlugin(int code)
{
	if((this->m_Data.mask & code) == code)
	{
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

