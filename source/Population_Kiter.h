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
	typedef std::pair<size_t, int> Chromosome;

	class KiterComparator {
        bool increasing;
	public:
        KiterComparator(bool i) : increasing(i) {}
		
		template <class T>
		bool operator() (const T& lhs, const T& rhs) const {
			if (increasing) {
				return lhs.second < rhs.second;
			}
			return lhs.second > rhs.second;
		}
	};

	class Population_Kiter
	{
	private:
		size_t _mu; 
		size_t _lambda;
		size_t _popSize;
		size_t _epoch;
		int _evalIter; // type is int instead of size_t to make sure division of kiterScore/evalIter does not give garbage value

		//std::priority_queue<Chromosome, std::vector<Chromosome>, KiterComparator> _genePool;
        std::vector<Chromosome> _genePool;

		// initialize method for population of many KiterDPSEvo
		void initialize(const GameState& state, PlayerPtr & p1, PlayerPtr & p2);

		// mutate method
		Chromosome mutate(size_t mutationDelta, const Chromosome& c, const GameState & state, PlayerPtr & p1, PlayerPtr & p2);

		// evaluation method
		int eval(const GameState & state, PlayerPtr & p1, PlayerPtr & p2);

		void printDist();

	public:
		Population_Kiter(size_t mu, size_t lambda, size_t epoch, size_t evalIter);
		size_t evolveSafeDist(const GameState & state, PlayerPtr & p1, PlayerPtr & p2);
	};

}