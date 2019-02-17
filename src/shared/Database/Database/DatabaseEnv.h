/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATABASEENV_H
#define DATABASEENV_H


#include "Platform/Define.h"

enum DatabaseStatements : uint32
{
	/*  Naming standard for defines:
	{DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
	When updating more than one field, consider looking at the calling function
	name for a suiting suffix.
	*/
	MAX_CONNECTDATABASE_STATEMENTS
};

#include "DatabaseWorkerPool.h"

#include "Field.h"
#include "PreparedStatement.h"
#include "QueryCallback.h"
#include "QueryResult.h"
#include "Transaction.h"

#include "Implementation/CSDatabase.h"

#endif
