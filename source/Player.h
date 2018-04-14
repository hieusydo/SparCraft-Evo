#pragma once

#include "Common.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include <memory>
#include <vector>
#include <set>

namespace SparCraft
{
 
class GameState;

class Player 
{
protected:
    IDType _playerID;
public:
    virtual void		getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
    const IDType        ID();
    void                setID(const IDType & playerid);
    virtual IDType      getType() { return PlayerModels::None; }
};

class POEScriptPlayer :Player
{
protected:
	IDType _playerID;
	//this type of player is only used as a POE subroutine
public:
	virtual void		getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec);
};

class CompareUnitDPSThreat
{
    const bool operator() (Unit * u1, Unit * u2) const
    {
        double u1Threat = ((double)u1->damage()/(double)u1->attackCooldown()) / u1->currentHP();
        double u2Threat = ((double)u2->damage()/(double)u2->attackCooldown()) / u2->currentHP();

        return u1Threat > u2Threat;
    }
};

typedef	std::shared_ptr<Player> PlayerPtr; 
typedef	std::shared_ptr<POEScriptPlayer> POEScriptPlayerPtr;
}