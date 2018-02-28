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
	
	typedef std::pair<double, double> Dxy;
	
	class Player_KiterEMP : public Player
	{
	private:
		bool _offline;

		// All features -> NORMALIZE!
		Array<double, Constants::Num_Params> _X; 

		// Weights for each move dir 
		// IMPORTANT: Order is Up - Down - Left - Right
		Array<double, Constants::Num_Params> _Wup; 
		Array<double, Constants::Num_Params> _Wdown;
		Array<double, Constants::Num_Params> _Wleft;
		Array<double, Constants::Num_Params> _Wright;

		Dxy getDxyClosestEnemy(const Unit& closestEnemy, const Unit& ourUnit) const;
		Dxy getDxyClosestAlly(const Unit& closestAlly, const Unit& ourUnit) const;
		Dxy getDxyCenterEnemy(const Position& enemyCenter, const Unit& ourUnit) const;
		Dxy getDxyCenterAlly(const Position& allyCenter, const Unit& ourUnit) const;

	public:
		Player_KiterEMP(const IDType & playerID);

		void switchOnOffline();
		void switchOffOffline();

		void setWup(Array<double, Constants::Num_Params> w);
		void setWdown(Array<double, Constants::Num_Params> w);
		void setWleft(Array<double, Constants::Num_Params> w);
		void setWright(Array<double, Constants::Num_Params> w);

		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::KiterEMP; }
	};
}