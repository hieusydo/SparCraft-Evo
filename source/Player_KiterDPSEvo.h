#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
	/*----------------------------------------------------------------------
	| Kiter DPS Evolution Player 
	|----------------------------------------------------------------------
	| Chooses an action with following priority:
	| 1) Use evolution to find a good safeDistance
	| 1) If it can attack, ATTACK highest DPS/HP enemy unit in range
	| 2) If it cannot attack:
	|    a) If it is not in range of safeDist, move away from closest one
	|    b) If it is in range of safeDist, MOVE towards closest one
	`----------------------------------------------------------------------*/
	class Player_KiterDPSEvo : public Player
	{
	private:
		size_t _safeDist;
		bool _offline;
	public:
		Player_KiterDPSEvo(const IDType & playerID);
		void setSafeDist(size_t d);
		size_t getSafeDist() const;
		void switchOnOffline();
		void switchOffOffline();
		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::KiterDPSEvo; }
	};
}