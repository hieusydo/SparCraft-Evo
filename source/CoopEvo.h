#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include <memory>
#include <queue>

namespace SparCraft
{
	typedef std::pair<std::vector<Array<double, Constants::Num_Params>>, int> ChromosomeEMP;

	class CoopEvo
	{
	private:
		size_t _mu;
		size_t _lambda;
		size_t _popSize;
		size_t _epoch;
		int _evalIter; // type is int instead of size_t to make sure division of kiterScore/evalIter does not give garbage value

		//std::priority_queue<Chromosome, std::vector<Chromosome>, KiterComparator> _genePool;
		std::vector<ChromosomeEMP> _genePool;

		// Order of direction Array in vector: Left Right Up Down
		void initRandomWeights(std::vector<Array<double, Constants::Num_Params>>& weights);

		// initialize method for population of many KiterDPSEvo
		void initialize(const GameState& state, PlayerPtr & p1, PlayerPtr & p2);

		// mutate method
		ChromosomeEMP mutate(const ChromosomeEMP& c, const GameState & state, PlayerPtr & p1, PlayerPtr & p2) const;

		// evaluation method
		int eval(const GameState & state, PlayerPtr & p1, PlayerPtr & p2) const;

		void writeFinalResult(const ChromosomeEMP& c) const;
		void printChrom(const ChromosomeEMP& c, std::ostream& os = std::cout) const;

	public:
		CoopEvo(size_t mu, size_t lambda, size_t epoch, size_t evalIter);
		void evolveParams(const GameState & state, PlayerPtr & p1, PlayerPtr & p2);
	};

}