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
		// IMPORTANT: Order is Left - Right - Up - Down 
		// Based on Constants::Move_Dir[4][2] 
		// = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 } };
		Array<double, Constants::Num_Params> _Wup; 
		Array<double, Constants::Num_Params> _Wdown;
		Array<double, Constants::Num_Params> _Wleft;
		Array<double, Constants::Num_Params> _Wright;

		Dxy getDxyClosest(const Unit& closestUnit, const Unit& ourUnit) const;
		Dxy getDxyCenter(const Position& centerArmy, const Unit& ourUnit) const;

		size_t getMaxVDir(double allV[4]) const;

		void normalize(Array<double, Constants::Num_Params>& X);

	public:
		Player_KiterEMP(const IDType & playerID);

		void switchOnOffline();
		void switchOffOffline();

		// Order of direction Array in vector: Left Right Up Down
		void setWeights(const std::vector<Array<double, Constants::Num_Params>>& weights);

		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::KiterEMP; }
	};
}