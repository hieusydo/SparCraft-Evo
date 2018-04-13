#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
	/*----------------------------------------------------------------------
	| Attack HighestDPS Player No Overkill
	|----------------------------------------------------------------------
	| Chooses an action with following priority:
	| 1) If it can attack, ATTACK highest DPS/HP enemy unit to overkill
	| 2) If it cannot attack:
	|    a) If it is in range to attack an enemy, WAIT until attack
	|    b) If it is not in range of enemy, MOVE towards closest
	`----------------------------------------------------------------------*/

	// as POE Script Player, it's designed to be called by POE as a subroutine.
	class POEPlayer_NOKDPS : public POEScriptPlayer
	{
	public:
		POEPlayer_NOKDPS(const IDType & playerID);
		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		void getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec);
		IDType getType() { return PlayerModels::NOKDPS; }
	};
}