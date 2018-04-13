#include "Player.h"

using namespace SparCraft;


void Player::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	// not implemented
}

void POEScriptPlayer::getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec)
{
	// not implemented
}

const IDType Player::ID() 
{ 
	return _playerID; 
}

void Player::setID(const IDType & playerID)
{
	_playerID = playerID;
}
