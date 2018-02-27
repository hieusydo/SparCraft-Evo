#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
	/*----------------------------------------------------------------------
	| Kiter Evo Multiple Params
	|----------------------------------------------------------------------
	| Chooses an action with following priority:
	| 1) Use evolution to find a good safeDistance
	| 1) If it can attack, ATTACK highest DPS/HP enemy unit in range TO OVERKILL?
	| 2) If it cannot attack:
	|    a) If it is not in range of safeDist, move away from closest one
	|    b) If it is in range of safeDist, MOVE towards closest one
	`----------------------------------------------------------------------*/
	class Player_KiterEMP : public Player
	{
	private:
		Array<double, Constants::Num_Params> _X; // all features -> NORMALIZE!
		Array<double, Constants::Num_Params> _W; // weights for each feature. Range: (0, 1)
		bool _offline;
	public:
		Player_KiterEMP(const IDType & playerID);
		void setParams(Array<double, Constants::Num_Params> w);
		size_t getSafeDist() const;
		void switchOnOffline();
		void switchOffOffline();
		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::KiterEMP; }
	};
}