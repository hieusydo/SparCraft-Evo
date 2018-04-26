#include "Player_POE.h"

using namespace SparCraft;

void Player_POE::clearPortfolio(){
	_poePlayerPtrPortfolio.clear();
}

void Player_POE::addPOEScriptPlayer(POEScriptPlayerPtr ptr){
	_poePlayerPtrPortfolio.push_back(ptr);
}

Player_POE::Player_POE(const IDType & playerID, const size_t & num_generations, 
	const size_t & poe_playout_round_limit, const size_t & population_size, const size_t & num_best_to_keep)
{
	_playerID = playerID;
	_num_generations = num_generations;
	_poe_playout_round_limit = poe_playout_round_limit;
	_population_size = population_size;
	_num_best_to_keep = num_best_to_keep;

	_seed = PlayerModels::NOKDPS;//the enemy used during playout

	clearPortfolio();
	addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_NOKDPS(playerID)));
	addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_KiterDPS(playerID)));

	// Test with evo in portfolio
	//addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_KiterEvo(playerID, "CC_result/wToBeUsed-1.txt")));
	//addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_KiterEvo(playerID, "CC_result/wToBeUsed-2.txt")));

}

void Player_POE::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();
	PortfolioOnlineEvolution poe(_playerID, _seed, _num_generations, _poe_playout_round_limit, _population_size, _num_best_to_keep, _poePlayerPtrPortfolio);

	//based on player ID and current state, give a moveVec for 
	//unit movements for this state
	moveVec = poe.search(_playerID, moves, state);
}
