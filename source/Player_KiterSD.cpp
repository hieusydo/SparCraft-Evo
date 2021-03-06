#include "Player_KiterSD.h"
#include "Evo_KiterSD.h"
#include "iostream"

using namespace SparCraft;

Player_KiterSD::Player_KiterSD (const IDType & playerID) 
{
	_playerID = playerID;
	_safeDist = 0; // default set it to 0, will set to random when initializing the population
	_offline = false;
}

void Player_KiterSD::setSafeDist(size_t d) {
	_safeDist = d;
}

size_t Player_KiterSD::getSafeDist() const {
	return _safeDist;
}

void Player_KiterSD::switchOnOffline() {
	_offline = true;
}

void Player_KiterSD::switchOffOffline() {
	_offline = false;
}

void Player_KiterSD::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();

	if (_offline == false) {
		//std::cout << "Reading safeDist from file\n";
		std::ifstream ifs("best.txt");
		if (!ifs) { std::cerr << "Error opening file\n"; }
		ifs >> _safeDist;
	}

	// Set up for NOK
	IDType enemy(state.getEnemy(_playerID));
	Array<int, Constants::Max_Units> hpRemaining;
	for (IDType u(0); u<state.numUnits(enemy); ++u)
	{
		hpRemaining[u] = state.getUnit(enemy, u).currentHP();
	}


	for (IDType u = 0; u < moves.numUnits(); ++u)
	{
		bool foundAction = false;
		IDType actionMoveIndex = 0;
		IDType furthestMoveIndex = 0;
		IDType closestMoveIndex	= 0;
		double actionHighestDPS	= 0;
		
		size_t furthestMoveDist = 0;
		unsigned long long closestMoveDist = std::numeric_limits<unsigned long long>::max();

		const Unit & ourUnit = state.getUnit(_playerID, u);
		const Unit & closestUnit = ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u);

		for (IDType m = 0; m < moves.numMoves(u); ++m)
		{
			const Action move = moves.getMove(u, m);
				
			if (move.type() == ActionTypes::ATTACK)
			{
				const Unit & target	= state.getUnit(state.getEnemy(move.player()), move.index());
				//double dpsHPValue = target.dpf() / target.currentHP();
				double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target = state.getUnit(move.player(), move.index());
				double dpsHPValue = target.dpf() / target.currentHP();

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::MOVE)
			{
				Position ourDest = Position(ourUnit.x() + Constants::Move_Dir[move.index()][0], ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				size_t dist = closestUnit.getDistanceSqToPosition(ourDest, state.getTime());

				if (dist > furthestMoveDist)
				{
					furthestMoveDist = dist;
					furthestMoveIndex = m;
				}

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		// the move we will be returning
		size_t bestMoveIndex = 0;

		// if we have an attack move we will use that one
		if (foundAction)
		{
			bestMoveIndex = actionMoveIndex;
		}
		// otherwise use the closest move to the opponent
		else
		{
			// Use evolved safeDist
			size_t dist = closestUnit.getDistanceSqToUnit(ourUnit, state.getTime());
			
			if (dist < pow(_safeDist, 2)){
				bestMoveIndex = furthestMoveIndex;
			}
			// otherwise get back into the fight
			else
			{
				bestMoveIndex = closestMoveIndex;
			}

			//// if we are in attack range of the unit, back up
			//if (closestUnit.canAttackTarget(ourUnit, state.getTime()))
			//{
			//	bestMoveIndex = furthestMoveIndex;
			//}
			//// otherwise get back into the fight
			//else
			//{
			//	bestMoveIndex = closestMoveIndex;
			//}
		}
		
		// Update for NOK
		Action theMove(moves.getMove(u, actionMoveIndex));
		if (theMove.type() == ActionTypes::ATTACK)
		{
			hpRemaining[theMove.index()] -= state.getUnit(_playerID, theMove.unit()).damage();
		}

		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
