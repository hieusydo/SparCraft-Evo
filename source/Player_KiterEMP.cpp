#include "Player_KiterEMP.h"
#include "Population_Kiter.h"
#include "iostream"

using namespace SparCraft;

Player_KiterEMP::Player_KiterEMP(const IDType & playerID)
{
	_playerID = playerID;
	_offline = false;
	_Wup.init(0);
	_Wdown.init(0);
	_Wleft.init(1);
	_Wright.init(0);
}

void Player_KiterEMP::switchOnOffline() {
	_offline = true;
}

void Player_KiterEMP::switchOffOffline() {
	_offline = false;
}

void Player_KiterEMP::setWup(Array<double, Constants::Num_Params> w) {
	for (size_t i = 0; i < w.size(); ++i) { _Wup[i] = w[i]; }
}

void Player_KiterEMP::setWdown(Array<double, Constants::Num_Params> w) {
	for (size_t i = 0; i < w.size(); ++i) { _Wdown[i] = w[i]; }
}

void Player_KiterEMP::setWleft(Array<double, Constants::Num_Params> w) {
	for (size_t i = 0; i < w.size(); ++i) { _Wleft[i] = w[i]; }
}

void Player_KiterEMP::setWright(Array<double, Constants::Num_Params> w) {
	for (size_t i = 0; i < w.size(); ++i) { _Wright[i] = w[i]; }
}

Dxy Player_KiterEMP::getDxyClosest(const Unit& closestUnit, const Unit& ourUnit) const {
	Dxy res; 
	res.first = closestUnit.x() - ourUnit.x();
	res.second = closestUnit.y() - ourUnit.y();
	return res;
}

Dxy Player_KiterEMP::getDxyCenter(const Position& centerArmy, const Unit& ourUnit) const {
	Dxy res;
	res.first = centerArmy.x() - ourUnit.x();
	res.second = centerArmy.y() - ourUnit.y();
	return res;
}

size_t Player_KiterEMP::getMaxVDir(double allV[4]) const {
	double maxV = -DBL_MAX;
	size_t maxIndex = 4;
	for (size_t i = 0; i < 4; ++i) { 
		if (allV[i] > maxV) {
			maxV = allV[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

void Player_KiterEMP::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();

	//if (_offline == false) {
	//	//std::cout << "Reading safeDist from file\n";
	//	std::ifstream ifs("best.txt");
	//	if (!ifs) { std::cerr << "Error opening file\n"; }
	//	//ifs >> _safeDist;
	//}

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
		IDType closestMoveIndex = 0;
		double actionHighestDPS = 0;

		size_t furthestMoveDist = 0;
		unsigned long long closestMoveDist = std::numeric_limits<unsigned long long>::max();

		const Unit & ourUnit = state.getUnit(_playerID, u);
		const Unit & closestUnit = ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u);

		IDType evolvedMoveIndex = 0;

		for (IDType m = 0; m < moves.numMoves(u); ++m)
		{
			const Action move = moves.getMove(u, m);

			if (move.type() == ActionTypes::ATTACK)
			{
				const Unit & target = state.getUnit(state.getEnemy(move.player()), move.index());
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
				//
				// Skip. Follow rule below
				//
				//Position ourDest = Position(ourUnit.x() + Constants::Move_Dir[move.index()][0], ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				//size_t dist = closestUnit.getDistanceSqToPosition(ourDest, state.getTime());
				//if (dist > furthestMoveDist)
				//{
				//	furthestMoveDist = dist;
				//	furthestMoveIndex = m;
				//}

				//if (dist < closestMoveDist)
				//{
				//	closestMoveDist = dist;
				//	closestMoveIndex = m;
				//}
				continue;
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
			const Unit& closestAlly = state.getClosestOurUnit(_playerID, u);
			const Unit& closestEnemy = state.getClosestEnemyUnit(_playerID, u);
			Position allyCenter = state.getAllyCenter(_playerID);
			Position enemyCenter = state.getEnemyCenter(_playerID);

			// dx, dy (encode direction and distance) --> Use cut-off to normalize
			Dxy dxyClosestEnemy = this->getDxyClosest(closestEnemy, ourUnit);
			double dxClosestEnemy = dxyClosestEnemy.first;
			double dyClosestEnemy = dxyClosestEnemy.second;

			Dxy dxyClosestAlly = this->getDxyClosest(closestAlly, ourUnit);
			double dxClosestAlly = dxyClosestAlly.first;
			double dyClosestAlly = dxyClosestAlly.second;

			Dxy dxyCenterEnemy = this->getDxyCenter(enemyCenter, ourUnit);
			double dxCenterEnemy = dxyCenterEnemy.first;
			double dyCenterEnemy = dxyCenterEnemy.second;

			Dxy dxyCenterAlly = this->getDxyCenter(allyCenter, ourUnit);
			double dxCenterAlly = dxyCenterAlly.first;
			double dyCenterAlly = dxyCenterAlly.second;

			// hp --> Use sqrt or log to normalize
			double hp = ourUnit.currentHP();

			// Fill  _X
			std::vector<double> tmp = { dxClosestEnemy, dyClosestEnemy,
				dxClosestAlly, dyClosestAlly,
				dxCenterEnemy, dyCenterEnemy,
				dxCenterAlly, dyCenterAlly,
				hp };
			for (size_t i = 0; i < tmp.size(); ++i) { _X.add(tmp[i]); }

			std::cout << "_Ws:" << _Wleft << "\n" << _Wright << "\n" << _Wup << "\n" << _Wdown << "\n";
			std::cout << "_X" << _X << "\n";

			// Calculate V
			// _W can be either from initialized values or read input
			double Vleft = _X.dot(_Wleft);
			double Vright = _X.dot(_Wright);
			double Vup = _X.dot(_Wup);
			double Vdown = _X.dot(_Wdown);

			std::cout << "Vs: " << Vleft << " " << Vright << " " << Vup << " " << Vdown << "\n";

			// Get best V and corresponding move index
			double allV[4] = { Vleft, Vright, Vup, Vdown };
			bestMoveIndex = this->getMaxVDir(allV);

			std::cout << "Best move index: " << bestMoveIndex << "\n";

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
