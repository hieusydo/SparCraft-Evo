#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include <memory>
#include <queue>

#include <chrono> // C++11

namespace SparCraft
{
	typedef std::pair<std::vector<Array<double, Constants::Num_Params>>, int> ChromosomeEMP;

	class Evo_KiterMvmt
	{
	private:
		size_t _mu;
		size_t _lambda;
		size_t _popSize;
		size_t _epoch;

		//std::priority_queue<Chromosome, std::vector<Chromosome>, KiterComparator> _genePool;
		std::vector<ChromosomeEMP> _genePool;

		// Order of direction Array in vector: Left Right Up Down
		void initRandomWeights(std::vector<Array<double, Constants::Num_Params>>& weights);

		// initialize method for population of many KiterSD
		void initialize(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);

		// mutate method
		ChromosomeEMP mutate(const ChromosomeEMP& c, const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) const;

		// evaluation method
		int eval(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const;

		void writeFinalResult(const ChromosomeEMP& c) const;
		void printChrom(const ChromosomeEMP& c, std::ostream& os = std::cout) const;

	public:
		Evo_KiterMvmt(size_t mu, size_t lambda, size_t epoch);
		void evolveParams(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);
	};

}