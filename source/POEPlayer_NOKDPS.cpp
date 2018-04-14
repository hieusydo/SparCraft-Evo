#include "POEPlayer_NOKDPS.h"

using namespace SparCraft;

POEPlayer_NOKDPS::POEPlayer_NOKDPS(const IDType & playerID)
{
	_playerID = playerID;
}


void POEPlayer_NOKDPS::getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec)
{
	if (clearMoveVec){
		moveVec.clear();
	}

	IDType enemy(state.getEnemy(_playerID));

	Array<int, Constants::Max_Units> hpRemaining;

	for (IDType u(0); u < state.numUnits(enemy); ++u)
	{
		hpRemaining[u] = state.getUnit(enemy, u).currentHP();
	}

	for (IDType u(0); u < moves.numUnits(); ++u)
	{
		//POE script player part
		if (allowedUnits.size() > 0){//if it's 0 then just allow all units
			const bool is_in = allowedUnits.find(u) != allowedUnits.end();//check if this unit is allowed
			if (!is_in){ continue; }
		}
		//POE script player part

		bool foundAction(false);
		size_t actionMoveIndex(0);
		double actionHighestDPS(0);
		size_t closestMoveIndex(0);
		unsigned long long closestMoveDist(std::numeric_limits<unsigned long long>::max());

		const Unit & ourUnit(state.getUnit(_playerID, u));
		const Unit & closestUnit(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : ourUnit.type().isDetector() ? state.getClosestEnemyUnit(_playerID, u, false) : state.getClosestEnemyUnit(_playerID, u, true));

		for (size_t m(0); m < moves.numMoves(u); ++m)
		{
			const Action move(moves.getMove(u, m));

			if ((move.type() == ActionTypes::ATTACK) && (hpRemaining[move.index()] > 0))
			{
				const Unit & target(state.getUnit(state.getEnemy(move.player()), move.index()));
				double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}

				if (move.index() >= state.numUnits(enemy))
				{
					int e = enemy;
					int pl = _playerID;
					printf("wtf\n");
				}
			}
			else if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target(state.getUnit(move.player(), move.index()));
				double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::RELOAD)
			{
				if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
				{
					closestMoveIndex = m;
					break;
				}
			}
			else if (move.type() == ActionTypes::MOVE)
			{
				Position ourDest(ourUnit.x() + Constants::Move_Dir[move.index()][0],
					ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				size_t dist(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		size_t bestMoveIndex(foundAction ? actionMoveIndex : closestMoveIndex);

		Action theMove(moves.getMove(u, actionMoveIndex));
		if (theMove.type() == ActionTypes::ATTACK)
		{
			hpRemaining[theMove.index()] -= state.getUnit(_playerID, theMove.unit()).damage();
		}

		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}


void POEPlayer_NOKDPS::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();
	IDType enemy(state.getEnemy(_playerID));

	Array<int, Constants::Max_Units> hpRemaining;

	for (IDType u(0); u<state.numUnits(enemy); ++u)
	{
		hpRemaining[u] = state.getUnit(enemy, u).currentHP();
	}

	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundAction(false);
		size_t actionMoveIndex(0);
		IDType furthestMoveIndex(0);
		size_t furthestMoveDist(0);
		double actionHighestDPS(0);
		size_t closestMoveIndex(0);
		unsigned long long closestMoveDist(std::numeric_limits<unsigned long long>::max());

		const Unit & ourUnit(state.getUnit(_playerID, u));
		const Unit & closestUnit(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u));

		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const Action move(moves.getMove(u, m));

			if ((move.type() == ActionTypes::ATTACK) && (hpRemaining[move.index()] > 0))
			{
				const Unit & target(state.getUnit(state.getEnemy(move.player()), move.index()));
				double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}

				if (move.index() >= state.numUnits(enemy))
				{
					int e = enemy;
					int pl = _playerID;
					printf("wtf\n");
				}
			}
			else if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target(state.getUnit(move.player(), move.index()));
				double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::RELOAD)
			{
				if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
				{
					closestMoveIndex = m;
					break;
				}
			}
			else if (move.type() == ActionTypes::MOVE)
			{
				Position ourDest(ourUnit.x() + Constants::Move_Dir[move.index()][0],
					ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				size_t dist(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

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

		size_t bestMoveIndex(0);
		// if we have an attack move we will use that one
		if (foundAction)
		{
			bestMoveIndex = actionMoveIndex;
		}
		// otherwise use the closest move to the opponent
		else
		{
			// if we are in attack range of the unit, back up
			if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
			{
				bestMoveIndex = furthestMoveIndex;
			}
			// otherwise get back into the fight
			else
			{
				bestMoveIndex = closestMoveIndex;
			}
		}

		Action theMove(moves.getMove(u, actionMoveIndex));
		if (theMove.type() == ActionTypes::ATTACK)
		{
			hpRemaining[theMove.index()] -= state.getUnit(_playerID, theMove.unit()).damage();
		}

		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
