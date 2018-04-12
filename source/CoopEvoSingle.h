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
	// a chromosome, vector size 4 (Wleft, Wright, Wup, Wdown)
	typedef std::vector<Array<double, Constants::Num_Params>> Chrms; 

	// Constants::Dna_Len individual chromosomes linked together to evolve
	// considering as the baseline before cooperative coevolution
	typedef std::pair<std::vector<Chrms>, int> Dna;

	class CoopEvoSingle
	{
	private:
		size_t _mu;
		size_t _lambda;
		size_t _popSize;
		size_t _epoch;

		std::vector<Dna> _genePool;

		// Order of direction Array in vector: Left Right Up Down
		void initRandomWeights(std::vector<Array<double, Constants::Num_Params>>& weights);

		// initialize method for population of many KiterDPSEvo
		void initialize(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);

		// mutate method
		Dna mutate(const Dna& c, const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) const;

		// evaluation method
		int eval(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const;

		void writeFinalResult(const Dna& c) const;
		void printChrom(const Dna& c, std::ostream& os = std::cout) const;

	public:
		CoopEvoSingle(size_t mu, size_t lambda, size_t epoch);
		void evolveParams(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);
	};

}