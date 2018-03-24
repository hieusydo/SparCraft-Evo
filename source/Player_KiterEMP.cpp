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
	_Wleft.init(0);
	_Wright.init(0);
}

void Player_KiterEMP::switchOnOffline() {
	_offline = true;
}

void Player_KiterEMP::switchOffOffline() {
	_offline = false;
}

// Order of direction Array in vector: Left Right Up Down
void Player_KiterEMP::setWeights(const std::vector<Array<double, Constants::Num_Params>>& weights) {
	for (size_t i = 0; i < weights[0].size(); ++i) { _Wleft[i] = weights[0][i]; }
	for (size_t i = 0; i < weights[1].size(); ++i) { _Wright[i] = weights[1][i]; }
	for (size_t i = 0; i < weights[2].size(); ++i) { _Wup[i] = weights[2][i]; }
	for (size_t i = 0; i < weights[3].size(); ++i) { _Wdown[i] = weights[3][i]; }
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

void Player_KiterEMP::normalize(Array<double, Constants::Num_Params>& X) {
	double meanX = 0;
	double minX = DBL_MAX;
	double maxX = -DBL_MAX;
	for (size_t i = 0; i < X.size(); ++i) {
		if (minX > X[i]) { minX = X[i]; }
		if (maxX < X[i]) { maxX = X[i]; }
		meanX += X[i];
	}
	meanX /= X.size();

	// Normalize using formula: (x - x_bar)/(x_max - x_min)
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] = (X[i] - meanX) / (maxX - minX);
	}
}

void Player_KiterEMP::printWeights() const {
	std::cout << "\nPrinting...\n";
	std::cout << "_Ws:" << _Wleft << "\n" << _Wright << "\n" << _Wup << "\n" << _Wdown << "\n";
}

void Player_KiterEMP::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	moveVec.clear();

	if (_offline == false) {
		//std::cout << "Reading params from file\n";
		std::ifstream ifs("kiterEMP/finalRes.txt");
		if (!ifs) { std::cerr << "Error opening file\n"; }

		std::vector<Array<double, Constants::Num_Params>> weights;
		for (size_t i = 0; i < 4; ++i) {
			Array<double, Constants::Num_Params> w;
			w.init(0);
			weights.push_back(w);
		}

		double w;
		for (size_t d = 0; d < weights.size(); ++d) {
			for (size_t i = 0; i < weights[d].capacity(); ++i) {
				ifs >> w;
				weights[d][i] = w;
			}
		}

		this->setWeights(weights);
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
		IDType closestMoveIndex = 0;
		double actionHighestDPS = 0;

		size_t furthestMoveDist = 0;
		unsigned long long closestMoveDist = std::numeric_limits<unsigned long long>::max();

		const Unit & ourUnit = state.getUnit(_playerID, u);
		const Unit & closestUnit = ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u);

		_X.clear();

		const Unit& closestAlly = state.getClosestOurUnit(_playerID, u);
		const Unit& closestEnemy = state.getClosestEnemyUnit(_playerID, u);
		Position allyCenter = state.getAllyCenter(_playerID);
		Position enemyCenter = state.getEnemyCenter(_playerID);

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
			// dx, dy (encode direction and distance) --> Use cut-off to normalize?
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

			// hp --> Use sqrt or log to normalize?
			double hp = ourUnit.currentHP();

			// Fill  _X
			std::vector<double> tmp = { dxClosestEnemy, dyClosestEnemy,
				dxClosestAlly, dyClosestAlly,
				dxCenterEnemy, dyCenterEnemy,
				dxCenterAlly, dyCenterAlly,
				hp };
			for (size_t i = 0; i < tmp.size(); ++i) { _X.add(tmp[i]); }

			// For now, use standard method
			this->normalize(_X);

			//std::cout << "_Ws:" << _Wleft << "\n" << _Wright << "\n" << _Wup << "\n" << _Wdown << "\n";
			//std::cout << "_X" << _X << "\n";

			// Calculate V
			// _W can be either from initialized values or read input
			double Vleft = _X.dot(_Wleft);
			double Vright = _X.dot(_Wright);
			double Vup = _X.dot(_Wup);
			double Vdown = _X.dot(_Wdown);

			//std::cout << "Vs: " << Vleft << " " << Vright << " " << Vup << " " << Vdown << "\n";

			// Get best V and corresponding move index
			double allV[4] = { Vleft, Vright, Vup, Vdown };
			bestMoveIndex = this->getMaxVDir(allV);

			//std::cout << "Best move index: " << bestMoveIndex << "\n";
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
