#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include <memory>
#include <random>

#include "POEPlayer_KiterEvo.h"

namespace SparCraft
{
	/*
	* Fixed ChromosomeEMP (makes more biological sense now, hopefully)
	* Multiple genes (for movement, attack, etc.) will make up a 
	* a larger chromosome and more omnipotent player
	* 
	* WeightDNA (weight DNA): a vector of weights for  
	*		9 features characterizing the movement
	* MGene (Movement gene): contains 
	*		1) a vector containing 4 WeightDNA
	*		2) fitness score
	*/
	typedef std::vector<double> WeightDNA;
	typedef std::pair<std::vector<WeightDNA>, int> MGene;

	std::random_device RD;
	unsigned int SEED = 1294198436;
	std::mt19937_64 ENGINE(SEED);
	std::normal_distribution<double> WDISTR(0, 1);
	std::normal_distribution<double> MDISTR(0, 0.1);

	class MGeneComparator {
		bool increasing;
	public:
		MGeneComparator(bool i) : increasing(i) {}

		template <class T>
		bool operator() (const T& lhs, const T& rhs) const {
			if (increasing) {
				return lhs.second < rhs.second;
			}
			return lhs.second > rhs.second;
		}
	};

	class CooperativeCoevolution
	{
	private:
		size_t _mu;
		size_t _lambda;
		size_t _popSize;
		size_t _epoch;
		std::vector<MGene> _subpop; // same as _genePool;

		// An ecosystem contains many mu-lambda ES subpopulations
		size_t _ecosysSize;
		std::vector<std::vector<MGene>> _ecosys;
		
		/*
		* Methods for subpopulation
		*/
		std::vector<MGene> makeSubpop();

		void initRandomWeights(vector<WeightDNA>& weights);

		/*
		* Methods for ecosystem
		*/
		// generate the subpopulations, each having random weights and initial score of -LLInt
		void initEvosys(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);

		int eval(const std::vector<GameState>& states, PlayerPtr & p1, PlayerPtr & p2) const;

		MGene mutate(const MGene& c, const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2) const;

	public:
		CooperativeCoevolution(size_t mu, size_t lambda, size_t epoch, size_t ecosysSize);
		void evolveParams(const std::vector<GameState>& state, PlayerPtr & p1, PlayerPtr & p2);
	};

}