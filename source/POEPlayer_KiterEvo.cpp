#include "POEPlayer_KiterEvo.h"
#include "iostream"
#include <numeric>

#include <random>

using namespace SparCraft;

POEPlayer_KiterEvo::POEPlayer_KiterEvo(const IDType & playerID)
	: _Wup(Constants::Num_Params),
	_Wdown(Constants::Num_Params),
	_Wleft(Constants::Num_Params),
	_Wright(Constants::Num_Params)
{
	_playerID = playerID;
} 

POEPlayer_KiterEvo::POEPlayer_KiterEvo(const IDType & playerID, const std::string& filePath)
	: _Wup(Constants::Num_Params),
	_Wdown(Constants::Num_Params),
	_Wleft(Constants::Num_Params),
	_Wright(Constants::Num_Params),
	_wFile(filePath) 
{
	_playerID = playerID;

	//if (_offline == false) {
	//	//std::cout << "Reading params from file\n";
	//	std::ifstream ifs(this->_wFile);
	//	if (!ifs) { std::cerr << "Error opening file\n"; }

	//	std::vector<Array<double, Constants::Num_Params>> weights;
	//	for (size_t i = 0; i < 4; ++i) {
	//		Array<double, Constants::Num_Params> w;
	//		w.init(0);
	//		weights.push_back(w);
	//	}

	//	double w;
	//	for (size_t d = 0; d < weights.size(); ++d) {
	//		for (size_t i = 0; i < weights[d].capacity(); ++i) {
	//			ifs >> w;
	//			weights[d][i] = w;
	//		}
	//	}

	//	this->setWeights(weights);
	//}
}

void POEPlayer_KiterEvo::switchOnOffline() {
	_offline = true;
}

void POEPlayer_KiterEvo::switchOffOffline() {
	_offline = false;
}

// Order of direction Array in vector: Left Right Up Down
void POEPlayer_KiterEvo::setWeights(const std::vector<std::vector<double>>& weights) {
	//std::cout << "Trying to set weights...\n";
	//printAllWeights();
	for (size_t i = 0; i < weights[0].size(); ++i) { _Wleft[i] = weights[0][i]; }
	for (size_t i = 0; i < weights[1].size(); ++i) { _Wright[i] = weights[1][i]; }
	for (size_t i = 0; i < weights[2].size(); ++i) { _Wup[i] = weights[2][i]; }
	for (size_t i = 0; i < weights[3].size(); ++i) { _Wdown[i] = weights[3][i]; }
	//printAllWeights();
}

Dxy POEPlayer_KiterEvo::getDxyClosest(const Unit& closestUnit, const Unit& ourUnit) const {
	Dxy res;
	res.first = closestUnit.x() - ourUnit.x();
	res.second = closestUnit.y() - ourUnit.y();
	return res;
}

Dxy POEPlayer_KiterEvo::getDxyCenter(const Position& centerArmy, const Unit& ourUnit) const {
	Dxy res;
	res.first = centerArmy.x() - ourUnit.x();
	res.second = centerArmy.y() - ourUnit.y();
	return res;
}

size_t POEPlayer_KiterEvo::getMaxVDir(double allV[4]) const {
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

void POEPlayer_KiterEvo::normalize(std::vector<double>& X) {
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

void printWeight(const std::vector<double>& weight) {
	for (double w : weight) { std::cout << w << ' '; }
	std::cout << '\n';
}

void POEPlayer_KiterEvo::printAllWeights() const {
	std::cout << "printAllWeights\n";
	printWeight(_Wleft);
	printWeight(_Wright);
	printWeight(_Wup);
	printWeight(_Wdown);
}

void POEPlayer_KiterEvo::getLimitedMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec, std::set<IDType> allowedUnits, bool clearMoveVec)
{
	
	if (clearMoveVec){
		moveVec.clear();
	}
	if (allowedUnits.size() <= 0) { return; }


	// Not doing NOK here since portfolio expected to have NOKDPS

	//the u here is NOT UID
	//IT IS SOMETHING ELSE
	for (IDType u = 0; u < moves.numUnits(); ++u)
	{
		//POE script player part
		const bool is_in = allowedUnits.find(u) != allowedUnits.end();//check if this unit is allowed
		if (!is_in){ continue; }
		//POE script player part

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
				double dpsHPValue = target.dpf() / target.currentHP();
				//double dpsHPValue = (target.dpf() / hpRemaining[move.index()]);

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
			for (size_t i = 0; i < tmp.size(); ++i) { _X.push_back(tmp[i]); }

			// For now, use standard method
			normalize(_X);

			//printAllWeights();

			// Calculate V
			// _W can be either from initialized values or read input
			double Vleft = std::inner_product(_X.begin(), _X.end(), _Wleft.begin(), 0.0);
			double Vright = std::inner_product(_X.begin(), _X.end(), _Wright.begin(), 0.0);
			double Vup = std::inner_product(_X.begin(), _X.end(), _Wup.begin(), 0.0);
			double Vdown = std::inner_product(_X.begin(), _X.end(), _Wdown.begin(), 0.0);

			// Get best V and corresponding move index
			double allV[4] = { Vleft, Vright, Vup, Vdown };
			bestMoveIndex = getMaxVDir(allV);
		}

		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}


void POEPlayer_KiterEvo::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	
	moveVec.clear();

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
			for (size_t i = 0; i < tmp.size(); ++i) { _X.push_back(tmp[i]); }

			// For now, use standard method
			normalize(_X);

			//std::cout << "_Ws:" << _Wleft << "\n" << _Wright << "\n" << _Wup << "\n" << _Wdown << "\n";
			//std::cout << "_X" << _X << "\n";

			// Calculate V
			// _W can be either from initialized values or read input
			double Vleft = std::inner_product(_X.begin(), _X.end(), _Wleft.begin(), 0.0);
			double Vright = std::inner_product(_X.begin(), _X.end(), _Wright.begin(), 0.0);
			double Vup = std::inner_product(_X.begin(), _X.end(), _Wup.begin(), 0.0);
			double Vdown = std::inner_product(_X.begin(), _X.end(), _Wdown.begin(), 0.0);

			//std::cout << "Vs: " << Vleft << " " << Vright << " " << Vup << " " << Vdown << "\n";

			// Get best V and corresponding move index
			double allV[4] = { Vleft, Vright, Vup, Vdown };
			bestMoveIndex = getMaxVDir(allV);

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
