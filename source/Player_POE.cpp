#include "Player_POE.h"

using namespace SparCraft;

Player_POE::Player_POE(const IDType & playerID)
{
	_playerID = playerID;
	_num_generations = 20; //default number of generations
	_seed = PlayerModels::NOKDPS;//the enemy used during playout

	clearPortfolio();
	addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_NOKDPS(playerID)));
	addPOEScriptPlayer(POEScriptPlayerPtr(new POEPlayer_KiterDPS(playerID)));

	//_poePlayerPtrPortfolio.push_back(POEScriptPlayerPtr(new POEPlayer_NOKDPS(playerID)));
	//_poePlayerPtrPortfolio.push_back(POEScriptPlayerPtr(new POEPlayer_KiterDPS(playerID)));
}

void Player_POE::clearPortfolio(){
	_poePlayerPtrPortfolio.clear();
}

void Player_POE::addPOEScriptPlayer(POEScriptPlayerPtr ptr){
	_poePlayerPtrPortfolio.push_back(ptr);
}

Player_POE::Player_POE(const IDType & playerID, const IDType & seed, const size_t & num_generations, const size_t & responses, const size_t & timeLimit)
{
	_playerID = playerID;
	_num_generations = num_generations;
	_seed = seed;
	_timeLimit = timeLimit;
}

void Player_POE::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();
	PortfolioOnlineEvolution poe(_playerID, _seed, _num_generations, _timeLimit, _poePlayerPtrPortfolio);
	
	//based on player ID and current state, give a moveVec for 
	//unit movements for this state
	moveVec = poe.search(_playerID, moves, state);
}
