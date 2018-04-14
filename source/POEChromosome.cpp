#include "POEChromosome.h"


using namespace SparCraft;

POEChromosome::POEChromosome(const IDType & player, int num_units, int time_length, int num_scripts, const std::vector<POEScriptPlayerPtr> poePlayerPtrPortfolio){
	//num of units just set this to max number of units
	//time_length is how many timesteps (how many rows for the chromosome)
	_num_scripts = num_scripts;
	_time_length = time_length;
	_scriptMap.resize(time_length);
	_poePlayerId = player;
	for (size_t i = 0; i < _scriptMap.size(); i++)
	{
		_scriptMap[i].resize(num_units);
	}

	_poePlayerPtrPortfolio = poePlayerPtrPortfolio;

	_playoutPlayers.push_back(PlayerPtr(new Player_NOKDPS(0)));
	_playoutPlayers.push_back(PlayerPtr(new Player_NOKDPS(1)));

	initialize_allowed_unit_sets();
}

void POEChromosome::initialize_allowed_unit_sets(){
	//gets called after set number of scripts
	_allowedUnitSets.resize(_num_scripts);
}

void POEChromosome::clear_allowed_unit_sets(){
	for (size_t i = 0; i < _allowedUnitSets.size(); i++)
	{
		_allowedUnitSets[i].clear();
	}
}

void POEChromosome::initialize_randomly(){
	//this will create a random scriptmap
	_score = std::numeric_limits<int>::min();
	for (size_t i = 0; i < _scriptMap.size(); i++)
	{
		//for each timestep
		for (size_t j = 0; j < _scriptMap[0].size(); j++)
		{
			//for each unit
			_scriptMap[i][j] = rand() % _num_scripts;
		}
	}
}

void POEChromosome::initialize_with_script_index(const int script_index){
	//this will create a scriptMap that uses the same script
	_score = std::numeric_limits<int>::min();
	for (size_t i = 0; i < _scriptMap.size(); i++)
	{
		//for each timestep
		for (size_t j = 0; j < _scriptMap[0].size(); j++)
		{
			//for each unit
			_scriptMap[i][j] = script_index;
		}
	}
}

void POEChromosome::setScriptMapAndResetScore(vector<vector<int>> newScriptMap){
	_scriptMap = newScriptMap;
	resetScore();
}

void POEChromosome::generateMoves(const IDType & player, const MoveArray & moves, GameState & state, const int timestep, std::vector<Action> & moveVec){
	//given the player, moves, state, and timestep, generate a moveVec
	//NOTE THAT CHROMOSOME GENERATE POE MOVES FOR THE POE PLAYER, BUT GENERATE NOKAV MOVES FOR THE OTHER PLAYER
	//give back moveVec
	//when given a chromosome, generate the moveVec using the script for a certain timestep

	if (player == _poePlayerId){
		//if the player is poe player, then give poe moves
		if (timestep < getTimeLength()){//if still have poe moves
			//first we need the script mappings for time=timestep
			std::vector<int> scriptMapVector = getScriptMapAtTimestep(timestep);

			//then we assign units to scripts, for each script, we need an allowedUnit set
			//first clear all sets
			clear_allowed_unit_sets();

			for (size_t i = 0; i < scriptMapVector.size(); i++)
			{
				//i means the ith unit in the moves vector
				int scriptIndex = scriptMapVector[i];//scriptIndex is the index of the script to use for unit i
				_allowedUnitSets[scriptIndex].insert(i);//this script is allowed to include this unit i
			}

			for (size_t scriptIndex = 0; scriptIndex < _num_scripts; scriptIndex++)
			{
				_poePlayerPtrPortfolio[scriptIndex]->getLimitedMoves(state, moves, moveVec, _allowedUnitSets[scriptIndex], FALSE);//moveVec gets filled by every script
			}

			//now moveVec is filled with poe moves
		}
		else{
			PlayerPtr playoutPlayerPtr = _playoutPlayers[player];
			playoutPlayerPtr->getMoves(state, moves, moveVec);
			//if no more poe moves available, then give NOKAV moves
		}
	}
	else{
		//give NOKAV moves for the non-poe player
		PlayerPtr playoutPlayerPtr = _playoutPlayers[player];
		playoutPlayerPtr->getMoves(state, moves, moveVec);
	}
}




void POEChromosome::setScore(ScoreType newScore){
	_score = newScore;
}

ScoreType POEChromosome::getScore(){
	return _score;
}

void POEChromosome::printOutChromosome(){
	std::cout << _score << std::endl;
	//basically just print out whatever's inside
	for (size_t i = 0; i < _scriptMap.size(); i++)
	{
		//for each timestep
		for (size_t j = 0; j < _scriptMap[0].size(); j++)
		{
			//for each unit
			std::cout << _scriptMap[i][j] << ",";
		}
		std::cout << std::endl;
	}
}

const vector<vector<int>> POEChromosome::getFullScriptMap(){
	//give back the script map stored in this chromosome
	return _scriptMap;
}

const vector<vector<int>> POEChromosome::getCopyOfScriptMap(){
	vector<vector<int>> newScriptMap(_scriptMap.size());
	for (size_t i = 0; i < _scriptMap.size(); i++)
	{
		newScriptMap[i].resize(_scriptMap[i].size());
		for (size_t j = 0; j < _scriptMap[i].size(); j++)
		{
			newScriptMap[i][j] = _scriptMap[i][j];
		}
	}
	return newScriptMap;
}


const vector<int> POEChromosome::getScriptMapAtTimestep(const int timestep){
	//give back the script map stored in this chromosome
	return _scriptMap[timestep];
}

const int POEChromosome::getTimeLength(){
	return _time_length;
}

bool POEChromosome::hasScoreAlready(){
	//return True if this chromosome has been set score already
	return _score > std::numeric_limits<int>::min();
}

void POEChromosome::resetScore(){
	//will reset the score to minimal 
	_score = std::numeric_limits<int>::min();
}

bool  POEChromosome::operator < (const POEChromosome& chr) const{
	return (_score < chr._score);
}

bool  POEChromosome::operator > (const POEChromosome& chr) const{
	return (_score > chr._score);
}


void POEChromosome::debug_print_scripmap(vector<vector<int>> sm){
	std::cout << std::endl;
	for (size_t i = 0; i < sm.size(); i++)
	{
		for (size_t j = 0; j < sm[0].size(); j++)
		{
			std::cout << sm[i][j] << ",";
		}
		std::cout << std::endl;
	}
}
