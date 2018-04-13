#pragma once

#include "Common.h"
#include "Player.h"
#include "PortfolioOnlineEvolution.h"

namespace SparCraft
{
	class Player_POE : public Player
	{
		IDType _seed;
		size_t _num_generations;
		size_t _timeLimit;
		std::vector<POEScriptPlayerPtr>      _poePlayerPtrPortfolio;
	public:
		Player_POE(const IDType & playerID);
		Player_POE(const IDType & playerID, const IDType & seed, const size_t & num_generations, const size_t & responses, const size_t & timeLimit);
		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::POE; }
		void clearPortfolio();//clear all pointers in the _poePlayerPtrPortfolio
		void addPOEScriptPlayer(POEScriptPlayerPtr ptr);//add a new script player to the portfolio
	};
}