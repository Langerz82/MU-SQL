////////////////////////////////////////////////////////////////////////////////
// LuaExport.h
#pragma once

#include "winutil.h"
#include "BuffEffectSlot.h"
#include "gObjMonster.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "Bag.h"
#include "BagManager.h"
#include "SetItemOption.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "MuunSystem.h"
#include "TLog.h"
#include "ItemOptionTypeMng.h"
#include "MasterLevelSkillTreeSystem.h"
#include "LargeRand.h"
#include "SocketItemType.h"
#include "CustomMichi.h"

class LuaExport
{
public:
	LuaExport();
	virtual ~LuaExport();

	void	AddTable(lua_State * Lua);
	void	AddGlobal(lua_State * Lua);

	static BOOL		LuaChatTargetSend(lua_State * Lua)
	{
		GSProtocol.ChatTargetSend(&gObj[lua_tointeger(Lua, 2)], (char*)lua_tostring(Lua, 1), lua_tointeger(Lua, 3));
		return true;
	};

	static BOOL		LuaLogAdd(lua_State * Lua)
	{
		g_Log.Add("%s", (char*)lua_tostring(Lua, 1));
		return true;
	};

	static BOOL		LuagObjTeleport(lua_State * Lua)
	{
		gObjTeleport(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tointeger(Lua, 4));
		return true;
	};

	static BOOL		LuaGCServerMsgStringSend(lua_State * Lua)
	{
		GSProtocol.GCServerMsgStringSend((char*)lua_tostring(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3));
		return true;
	};

	static BOOL		LuaFireCrackerOpen(lua_State * Lua)
	{
		PMSG_SERVERCMD ServerCmd;

		PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
		ServerCmd.CmdType = 0;
		ServerCmd.X = gObj[lua_tointeger(Lua, 1)].X;
		ServerCmd.Y = gObj[lua_tointeger(Lua, 1)].Y;

		GSProtocol.MsgSendV2(&gObj[lua_tointeger(Lua, 1)], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		IOCP.DataSend(lua_tointeger(Lua, 1), (LPBYTE)&ServerCmd, sizeof(ServerCmd));

		return true;
	};

	static BOOL		LuagObjCalDistance(lua_State * Lua)
	{
		BOOL Result = gObjCalDistance(&gObj[lua_tointeger(Lua, 1)], &gObj[lua_tointeger(Lua, 2)]);
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuaGCServerCmd(lua_State * Lua)
	{
		GSProtocol.GCServerCmd(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tointeger(Lua, 4));
		return true;
	};

	static BOOL		LuagUserFindDevilSquareKeyEyes(lua_State * Lua)
	{
		BOOL Result = gUserFindDevilSquareKeyEyes(lua_tointeger(Lua, 1));
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuagObjAddBuffEffect(lua_State * Lua)
	{
		BOOL Result = gObjAddBuffEffect(&gObj[lua_tointeger(Lua, 1)], lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tointeger(Lua, 4), lua_tointeger(Lua, 5), lua_tointeger(Lua, 6), lua_tointeger(Lua, 7));
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuaGCReFillSend(lua_State * Lua)
	{
		GSProtocol.GCReFillSend(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tointeger(Lua, 4), lua_tointeger(Lua, 5));
		return true;
	};

	static BOOL		LuaGCManaSend(lua_State * Lua)
	{
		GSProtocol.GCManaSend(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tointeger(Lua, 4), lua_tointeger(Lua, 5));
		return true;
	};

	static BOOL		LuagQeustNpcTeleport_TalkRefAdd(lua_State * Lua)
	{
		gQeustNpcTeleport.TalkRefAdd();
		return true;
	};

	static BOOL		LuagObjIsConnected(lua_State * Lua)
	{
		BOOL Result = gObjIsConnected(lua_tointeger(Lua, 1));
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuagParty_GetPKPartyPenalty(lua_State * Lua)
	{
		BOOL Result = gParty.GetPKPartyPenalty(lua_tointeger(Lua, 1));
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuagObjGuildMasterCapacityTest(lua_State * Lua)
	{
		BOOL Result = gObjGuildMasterCapacityTest(&gObj[lua_tointeger(Lua, 1)]);
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuagObjMonsterTopHitDamageUser(lua_State * Lua)
	{
		BOOL Result = gObjMonsterTopHitDamageUser(&gObj[lua_tointeger(Lua, 1)]);
		lua_pushnumber(Lua, Result);
		return Result;
	};

	static BOOL		LuaItemSerialCreateSend(lua_State * Lua)
	{
		int iSocketCount = lua_tointeger(Lua, 14);
		int iMuunEvolutionItemID = lua_tointeger(Lua, 16);
		int iItemID = lua_tointeger(Lua, 5);
		BYTE SocketOption[5] = { -1, -1, -1, -1, -1 };
		BYTE btNewOption = 0;
		BYTE Option1 = 0;
		int iNewExcOption[9];

		if (g_CMuunSystem.IsStoneofEvolution(iItemID) == true)
		{
			SocketOption[0] = iMuunEvolutionItemID >> 8;
			SocketOption[1] = ITEM_GET_INDEX(iMuunEvolutionItemID);
		}
		else
		{
			for (int i = 0; i < iSocketCount; i++)
			{
				SocketOption[i] = 0xFE;
			}
		}

		if (gSocketItemType.CheckSocketItemType(iItemID) == true)
		{
			for (int i = 0; i < iSocketCount; i++)
			{
				SocketOption[i] = 0xFE;
			}
		}

		if (g_PentagramSystem.IsPentagramItem(iItemID) == TRUE)
		{
			for (int i = 0; i < iSocketCount; i++)
			{
				SocketOption[i] = 0xFE;
			}
		}

		BYTE btNewExcOption[9];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		for (int i = 0; i < 9; i++)
		{
			btNewExcOption[i] = lua_tointeger(Lua, 17 + i);
		}

		if (btNewExcOption[0] == (BYTE)-2)
		{
			if (ItemAttribute[iItemID].ItemKindA == ITEM_KIND_A_WING)
			{
				btNewOption = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[iItemID].ItemKindA, ItemAttribute[iItemID].ItemKindB, SocketOption);
			}
			else
			{
				btNewOption = g_ItemOptionTypeMng.CommonExcOptionRand(ItemAttribute[iItemID].ItemKindA, SocketOption);
			}
		}
		else if (btNewExcOption[0] == (BYTE)-3)
		{
			if (ItemAttribute[iItemID].ItemKindA == ITEM_KIND_A_WING)
			{
				btNewOption = g_ItemOptionTypeMng.WingExcOptionGetCount(ItemAttribute[iItemID].ItemKindA, ItemAttribute[iItemID].ItemKindB, SocketOption, btNewExcOption[1]);
			}

			else
			{
				btNewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(ItemAttribute[iItemID].ItemKindA, SocketOption, btNewExcOption[1]);
			}
		}
		else if (btNewExcOption[0] == (BYTE)-4)
		{
			btNewOption = NewOptionRand2(g_ConfigRead.MinOptionCount, g_ConfigRead.MaxOptionCount);
			Option1 = 1;
		}
		else
		{
			for (int i = 0; i < 9; i++)
			{
				if (btNewExcOption[i] == -1)
				{
					continue;
				}

				if (btNewExcOption[i] >= 6)
				{
					bool bOptionFound = false;

					for (int j = 0; j < 5; j++)
					{
						if (SocketOption[j] == btNewExcOption[i])
						{
							bOptionFound = true;
							break;
						}
					}

					if (bOptionFound == true)
					{
						continue;
					}

					for (int j = 0; j < 5; j++)
					{
						if (SocketOption[j] == 0xFF)
						{
							SocketOption[j] = btNewExcOption[i];
							break;
						}
					}
				}

				else
				{
					btNewOption |= (1 << (5 - btNewExcOption[i]));
				}
			}
		}

		ItemSerialCreateSend(
			lua_tointeger(Lua, 1), lua_tointeger(Lua, 2),
			lua_tointeger(Lua, 3), lua_tointeger(Lua, 4),
			iItemID, lua_tointeger(Lua, 6),
			lua_tointeger(Lua, 7), lua_tointeger(Lua, 8),
			lua_tointeger(Lua, 9), lua_tointeger(Lua, 10),
			lua_tointeger(Lua, 11), btNewOption,
			lua_tointeger(Lua, 12), lua_tointeger(Lua, 13), SocketOption, lua_tointeger(Lua, 15));
		return true;
	};

	static BOOL		LuaMakeRandomSetItem(lua_State * Lua)
	{
		MakeRandomSetItem(lua_tointeger(Lua, 1));
		return true;
	};

	static BOOL		LuaGetBagItemLevel(lua_State * Lua)
	{
		int iLevel = CBag::GetItemLevel(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2));
		lua_pushnumber(Lua, iLevel);
		return 1;
	}

	static BOOL		LuaAddItemBag(lua_State * Lua)
	{

		g_BagManager.AddItemBag(lua_tointeger(Lua, 1), lua_tointeger(Lua, 2), lua_tointeger(Lua, 3), lua_tostring(Lua, 4));
		return true;
	}

	static BOOL		LuaIsItem(lua_State * Lua)
	{
		bool isItem = IsItem(lua_tointeger(Lua, 1));
		lua_pushboolean(Lua, isItem);
		return true;
	}

	static BOOL		LuaNewOptionRand(lua_State * Lua)
	{
		int Exc = g_Michi.IsNewOptionRand(0);
		lua_pushnumber(Lua, Exc);
		return true;
	}

	static BOOL		LuaGetSetItemOption(lua_State * Lua)
	{
		int SetOption = gSetItemOption.GenSetOption(lua_tointeger(Lua, 1));
		lua_pushnumber(Lua, SetOption);
		return true;
	}

	static BOOL		LuaGetRandomValue(lua_State * Lua)
	{
		int Div = lua_tointeger(Lua, 1);

		if (Div == 0)
		{
			g_Log.AddC(TColor::Red, "[LUA] ERROR - LuaGetRandomValue Divider == 0");
			return false;
		}

		int Val = rand() % Div;
		lua_pushnumber(Lua, Val);
		return true;
	}

	static BOOL		LuaGetLargeRandomValue(lua_State * Lua)
	{
		int Div = lua_tointeger(Lua, 1);

		if (Div == 0)
		{
			g_Log.AddC(TColor::Red, "[LUA] ERROR - LuaGetLargeRandomValue Divider == 0");
			return false;
		}

		int Val = GetLargeRand() % Div;
		lua_pushnumber(Lua, Val);
		return true;
	}

	static BOOL		LuaIsSocketItem(lua_State * Lua)
	{
		CItem Item;
		Item.m_Type = lua_tointeger(Lua, 1);
		bool Result = g_SocketOptionSystem.IsEnableSocketItem(&Item);
		lua_pushboolean(Lua, Result);
		return true;
	}
	static BOOL		LuaIsPentagramItem(lua_State * Lua)
	{
		int ItemID = lua_tointeger(Lua, 1);
		lua_settop(Lua, -1);
		bool Result = false;

		if (g_PentagramSystem.IsPentagramItem(ItemID) == true ||
			g_PentagramSystem.IsPentagramJewel(ItemID) == true)
		{
			Result = true;
		}

		lua_pushboolean(Lua, Result);
		return true;
	}
	static BOOL		LuaIsElementalItem(lua_State * Lua)
	{
		int ItemID = lua_tointeger(Lua, 1);
		lua_settop(Lua, -1);
		bool Result = false;

		if (g_PentagramSystem.IsPentagramItem(ItemID) == true ||
			g_PentagramSystem.IsPentagramJewel(ItemID) == true)
		{
			Result = true;
		}

		lua_pushboolean(Lua, Result);
		return true;
	}

	static BOOL		LuaAddToMLSValueTable(lua_State * Lua)
	{
		g_MasterLevelSkillTreeSystem.AddToValueTable(lua_tointeger(Lua, 1), lua_tostring(Lua, 2), lua_tointeger(Lua, 3));
		return true;
	}

	static BOOL		LuaGetExcOptionByConfig(lua_State * Lua)
	{
		int ItemKindA = lua_tointeger(Lua, 1);
		lua_settop(Lua, -1);

		BYTE btNewExcOption[5];
		int ExcOpt = g_ItemOptionTypeMng.CommonExcOptionRand(ItemKindA, btNewExcOption);

		return true;
	}
};

extern LuaExport gLuaExport;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

