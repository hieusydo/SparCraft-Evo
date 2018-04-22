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

	// Uses NOK
	// TODO: Add POE part
	class POEPlayer_KiterEvo : public POEScriptPlayer
	{
	private:
		std::string _wFile = "kiterEvo/wEvo1.txt";
		bool _offline = false;

		std::vector<double> _X;

		// Weights for each move dir 
		// IMPORTANT: Order is Left - Right - Up - Down 
		// Based on Constants::Move_Dir[4][2] 
		// = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 } };
		std::vector<double> _Wup;
		std::vector<double> _Wdown;
		std::vector<double> _Wleft;
		std::vector<double> _Wright;

		Dxy getDxyClosest(const Unit& closestUnit, const Unit& ourUnit) const;
		Dxy getDxyCenter(const Position& centerArmy, const Unit& ourUnit) const;

		size_t getMaxVDir(double allV[4]) const;

		void normalize(std::vector<double>& X);

	public:
		POEPlayer_KiterEvo(const IDType & playerID);
		POEPlayer_KiterEvo(const IDType & playerID, const std::string& filePath);

		void switchOnOffline();
		void switchOffOffline();

		void printAllWeights() const;

		// Order of direction Array in vector: Left Right Up Down
		void setWeights(const std::vector<std::vector<double>>& weights);

		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);

		void getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec);

		IDType getType() { return PlayerModels::KiterEvo; }
	};
}