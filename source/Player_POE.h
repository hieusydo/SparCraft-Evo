#pragma once

#include "Common.h"
#include "Player.h"
#include "PortfolioOnlineEvolution.h"
#include "POEPlayer_KiterEvo.h"
namespace SparCraft
{
	class Player_POE : public Player
	{
		IDType _seed;
		size_t _num_generations;
		size_t _poe_playout_round_limit;
		size_t _population_size;
		size_t _num_best_to_keep;
		std::vector<POEScriptPlayerPtr>      _poePlayerPtrPortfolio;
	public:
		Player_POE(const IDType & playerID, const size_t & num_generations=15, const size_t & poe_playout_round_limit=15, const size_t & population_size=18, const size_t & num_best_to_keep=6);
		void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
		IDType getType() { return PlayerModels::POE; }
		void clearPortfolio();//clear all pointers in the _poePlayerPtrPortfolio
		void addPOEScriptPlayer(POEScriptPlayerPtr ptr);//add a new script player to the portfolio
	};
}