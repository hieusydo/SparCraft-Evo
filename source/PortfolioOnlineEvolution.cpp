#include "PortfolioOnlineEvolution.h"
#include <algorithm>

using namespace SparCraft;
using std::cout;
using std::endl;


PortfolioOnlineEvolution::PortfolioOnlineEvolution(const IDType & player, const IDType & enemyScript, 
	const size_t & num_generations, const size_t & timeLimit, std::vector<POEScriptPlayerPtr>  poePlayerPtrPortfolio)
	: _player(player)
	, _enemyScript(enemyScript) //this is the enemy script used for playout.
	, _num_generations(num_generations)
	, _timeLimit(timeLimit)
{
	//now we use PlayerPtr instead of enum.
	_poePlayerPtrPortfolio = poePlayerPtrPortfolio;//passed to this from a POE player

	//POE hyperparameters
	_poeTimeLength = 10; //this means how many rows a chromosome has, this is different from max round limit in playout
	_population_size = 18;
	_num_best_to_keep = 6;
	_num_generation=8;
	_mutate_rate = 0.5f;
	_poe_playout_round_limit = 10;

	_random_generator = std::mt19937_64(std::time(0));//seed the random generator
}


std::vector<Action> PortfolioOnlineEvolution::search(const IDType & player, const MoveArray & moves, GameState & state)
{
	//main function for PGS
	//give the state and player id, return a vector of actions
	//this function runs every time you need unit actions

	//set a timer
	Timer t;
	t.start();

	// get the id of the enemy
	const IDType enemyPlayer(state.getEnemy(player));
	// get an empty moveVec
	std::vector<Action> moveVec;

	// call evolve to get the best chromosome we want.
	POEChromosome bestChromosome = evolve(player,state, 20);//TODO testing now, later should add more parameters

	bestChromosome.generateMoves(player, moves, state, 0, moveVec);

	return moveVec;
}

std::vector<POEChromosome> PortfolioOnlineEvolution::init_population(){
	//get a randomly initialized population
	//actually, if population size is equal or bigger than script number, then 
	//you also have individuals that only just use one script
	//you get <num of script> number of such individuals
	std::vector<POEChromosome> population;

	if (_population_size >= _poePlayerPtrPortfolio.size()){
		for (size_t i = 0; i < _poePlayerPtrPortfolio.size(); i++)
		{
			population.push_back(POEChromosome(_player, Constants::Max_Units, _poeTimeLength, _poePlayerPtrPortfolio.size(), _poePlayerPtrPortfolio));
			population[i].initialize_with_script_index(i);
		}

		for (size_t i = _poePlayerPtrPortfolio.size(); i < _population_size; i++)
		{
			population.push_back(POEChromosome(_player, Constants::Max_Units, _poeTimeLength, _poePlayerPtrPortfolio.size(), _poePlayerPtrPortfolio));
			population[i].initialize_randomly();
		}
	}
	else{
		//if population is really small
		for (size_t i = 0; i < _population_size; i++)
		{
			population.push_back(POEChromosome(_player, Constants::Max_Units, _poeTimeLength, _poePlayerPtrPortfolio.size(), _poePlayerPtrPortfolio));
			population[i].initialize_randomly();
		}
	}

	return population;
}

vector<vector<int>> PortfolioOnlineEvolution::getMutatedScriptMap(POEChromosome parentChromosome ,float mutateRate){
	//call this function on a chromosome to get a mutated copy of its scriptMap
	vector<vector<int>> parentScriptMap = parentChromosome.getCopyOfScriptMap();

	int num_script = parentChromosome._num_scripts;
	
	std::uniform_int_distribution<int> uniform_int(0, num_script-1);//this distribution include the bigger number

	//std::cout << num_script << " " << uniform_int(_random_generator) << " " << uniform_int(_random_generator) << " " << _unif_double(_random_generator) << std::endl;

	vector<vector<int>> newScriptMap(parentScriptMap.size());
	for (size_t i = 0; i < parentScriptMap.size(); i++)
	{
		newScriptMap[i].resize(parentScriptMap[i].size());
		for (size_t j = 0; j < parentScriptMap[i].size(); j++)
		{
			double randomNumber = _unif_double(_random_generator);
			if (randomNumber < mutateRate)
			{
				newScriptMap[i][j] = uniform_int(_random_generator);
			}
			else{
				newScriptMap[i][j] = parentScriptMap[i][j];
			}
		}
	}

	return newScriptMap;
}



POEChromosome PortfolioOnlineEvolution::evolve(const IDType & player, const GameState & state, const int num_generation){
	//first initialize population, size of population is one of class memeber variable
	std::vector<POEChromosome> population;


	for (size_t epoch = 0; epoch < num_generation; epoch++)
	{
		//for each generation, first generate the new chromosomes.
		if (population.empty()){ // if has no population yet, then generate new population
			population = init_population();
		}
		else{//if has population, then do natural selection (keep the best several, and change the rest to new ones)
			for (size_t i = _num_best_to_keep; i < population.size(); i++)
			{
				//for each one individual that we will discard. (for each new chromosome that we will generate)
				//first choose a parent
				int parentIndex = rand() % _num_best_to_keep;
				POEChromosome parent = population[parentIndex];
				vector<vector<int>> childScriptMap = getMutatedScriptMap(parent, _mutate_rate);
				population[i].setScriptMapAndResetScore(childScriptMap);
			}
		}

		//then we evaluate the new individuals in the population, and sort the population 
		for (size_t i = 0; i < population.size(); i++)
		{
			POEChromosome chr = population[i];
			if (!chr.hasScoreAlready())//only set score if it doesn't have score set already
			{
				ScoreType score = evaluateChromosome(player, state, chr);
				population[i].setScore(score);
			}
		}

		//now we have fitness score for each individual we need to sort the population
		//since we override < operator in POEChromosome, sorting is very easy
		std::sort(population.begin(), population.end());
		std::reverse(population.begin(), population.end());
		//after sorting, population[0] has the highest score, last individual in population has the lowest

		//FOLLOWING ARE DEBUGGING CODE
		//float aveScore = 0;
		//for (size_t i = 0; i < population.size(); i++)
		//{
		//	aveScore += population[i].getScore();
		//}
		//aveScore /= population.size();
		//cout << "epoch:" << epoch << ", best score:" << population[0].getScore() << ", average score:" << aveScore << endl;
	}

	return population[0];//this will be the chromosome with the highest score, since we sorted population and then reversed it
}

ScoreType PortfolioOnlineEvolution::evaluateChromosome(const IDType & player, const GameState & state, POEChromosome chr){
	//given a chromosome, return its evaluation score
	const IDType enemyPlayer(state.getEnemy(player));
	//make a clone of the game, state here is the initial state of the game
	Game g(state, 100);

	g.POEPlayOut(player, chr, _poe_playout_round_limit);

	return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2).val();
}


/*

std::vector<Action> PortfolioOnlineEvolution::generateMovesFromChromosome(const IDType & player, const MoveArray & moves, GameState & state, POEChromosome chr,const int timestep){
//give back moveVec
//when given a chromosome, generate the moveVec using the script for a certain timestep

if (timestep >= chr.getTimeLength()){
std::cout << "TIME LENGTH EXCEEDED!" << std::endl;
}

//first we need the script mappings for time=timestep
std::vector<int> scriptMapVector = chr.getScriptMapAtTimestep(timestep);

//then we assign units to scripts, for each script, we need an allowedUnit set
//first clear all sets
clear_allowed_unit_sets();

for (size_t i = 0; i < scriptMapVector.size(); i++)
{
//i means the ith unit in the moves vector
int scriptIndex = scriptMapVector[i];//scriptIndex is the index of the script to use for unit i
_allowedUnitSets[scriptIndex].insert(i);//this script is allowed to include this unit i
}


std::vector<Action> moveVec;//init an empty moveVec
for (size_t scriptIndex = 0; scriptIndex < _poePlayerPtrPortfolio.size(); scriptIndex++)
{
_poePlayerPtrPortfolio[scriptIndex]->getLimitedMoves(state, moves, moveVec, _allowedUnitSets[scriptIndex], FALSE);//moveVec gets filled by every script
}

//now moveVec is filled, we can return it
return moveVec;
}



*/
