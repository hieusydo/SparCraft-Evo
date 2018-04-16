#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include <memory>
#include "POEPlayer_NOKDPS.h"
#include "POEPlayer_KiterDPS.h"
#include "POEChromosome.h"
#include "random"
 
namespace SparCraft
{

	typedef	std::shared_ptr<Player> PlayerPtr;

	class PortfolioOnlineEvolution
	{
		//the scripts used by this are passed to it from POE Player
	protected:

		const IDType				_player;
		const IDType				_enemyScript;
		const size_t				_num_generations;
		float _mutate_rate;

		std::mt19937_64 _random_generator;
		std::uniform_real_distribution<double> _unif_double;

		//POE hyperparameters
		int _poeTimeLength; //this means how many rows a chromosome has, this is different from max round limit in playout
		// TODO add max time limit
		int _population_size;
		int _num_best_to_keep;
		int _num_generation;
		int _poe_playout_round_limit;

		std::vector<POEScriptPlayerPtr>      _poePlayerPtrPortfolio;

		size_t                      _totalEvals;
		size_t                      _timeLimit;

		std::vector<Action>     getMoveVec(const IDType & player, const GameState & state, const std::vector<IDType> & playerScripts);

	public:
		PortfolioOnlineEvolution(const IDType & player, const IDType & enemyScript, const size_t & num_generations, const size_t & timeLimit, std::vector<POEScriptPlayerPtr>  poePlayerPtrPortfolio);
		std::vector<Action> search(const IDType & player, const MoveArray & moves, GameState & state);
		ScoreType evaluateChromosome(const IDType & player, const GameState & state, POEChromosome chr); //given a chromosome, return its evaluation score
		POEChromosome evolve(const IDType & player, const GameState & state, const int num_generation);//give back the best chromosome throughout evolution
		std::vector<POEChromosome> init_population();
		vector<vector<int>> getMutatedScriptMap(POEChromosome ch, float mutateRate);
	};

}