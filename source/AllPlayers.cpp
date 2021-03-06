#include "AllPlayers.h"
    
using namespace SparCraft;

Player * AllPlayers::getPlayer(const IDType & playerID, const IDType & type)
{
	if			(type == PlayerModels::AttackClosest)		{ return new Player_AttackClosest(playerID); }
	else if		(type == PlayerModels::AttackDPS)			{ return new Player_AttackDPS(playerID); }
	else if		(type == PlayerModels::AttackWeakest)		{ return new Player_AttackWeakest(playerID); }
	else if		(type == PlayerModels::Kiter)				{ return new Player_Kiter(playerID); }
	else if		(type == PlayerModels::KiterDPS)			{ return new Player_KiterDPS(playerID); }

	else if (type == PlayerModels::KiterSD)     { return new Player_KiterSD(playerID); }
	else if (type == PlayerModels::KiterMvmt)     { return new Player_KiterMvmt(playerID); }
	else if (type == PlayerModels::KiterEvo1)     { return new Player_KiterEvo1(playerID); }
	else if (type == PlayerModels::KiterEvo2)     { return new Player_KiterEvo2(playerID); }
	else if (type == PlayerModels::POE)     { return new Player_POE(playerID); }

	else if		(type == PlayerModels::Kiter_NOKDPS)		{ return new Player_Kiter_NOKDPS(playerID); }
    else if		(type == PlayerModels::Cluster)		    	{ return new Player_Cluster(playerID); }
	else if		(type == PlayerModels::NOKDPS)              { return new Player_NOKDPS(playerID); }
	else if		(type == PlayerModels::Random)				{ return new Player_Random(playerID); }
	else													{ return NULL; }
}

PlayerPtr AllPlayers::getPlayerPtr(const IDType & playerID, const IDType & type)
{
	if			(type == PlayerModels::AttackClosest)		{ return PlayerPtr(new Player_AttackClosest(playerID)); }
	else if		(type == PlayerModels::AttackDPS)			{ return PlayerPtr(new Player_AttackDPS(playerID)); }
	else if		(type == PlayerModels::AttackWeakest)		{ return PlayerPtr(new Player_AttackWeakest(playerID)); }
	else if		(type == PlayerModels::Kiter)				{ return PlayerPtr(new Player_Kiter(playerID)); }
	else if		(type == PlayerModels::KiterDPS)			{ return PlayerPtr(new Player_KiterDPS(playerID)); }

	else if (type == PlayerModels::KiterSD)     { return PlayerPtr(new Player_KiterSD(playerID)); }
	else if (type == PlayerModels::KiterMvmt)     { return PlayerPtr(new Player_KiterMvmt(playerID)); }
	else if (type == PlayerModels::KiterEvo1)     { return PlayerPtr(new Player_KiterEvo1(playerID)); }
	else if (type == PlayerModels::KiterEvo2)     { return PlayerPtr(new Player_KiterEvo2(playerID)); }
	else if (type == PlayerModels::POE)     { return PlayerPtr(new Player_POE(playerID)); }

    else if		(type == PlayerModels::Kiter_NOKDPS)		{ return PlayerPtr(new Player_Kiter_NOKDPS(playerID)); }
    else if		(type == PlayerModels::Cluster)		    	{ return PlayerPtr(new Player_Cluster(playerID)); }
	else if		(type == PlayerModels::NOKDPS)              { return PlayerPtr(new Player_NOKDPS(playerID)); }
	else if		(type == PlayerModels::Random)				{ return PlayerPtr(new Player_Random(playerID)); }
	else													{ return PlayerPtr(new Player_NOKDPS(playerID)); }
}

std::string AllPlayers::getPlayerName(const IDType & type) {
	if (type == PlayerModels::AttackClosest)		{ return "AttackClosest"; }
	else if (type == PlayerModels::AttackDPS)			{ return "AttackDPS"; }
	else if (type == PlayerModels::AttackWeakest)		{ return "AttackWeakest"; }
	else if (type == PlayerModels::Kiter)				{ return "Kiter"; }
	else if (type == PlayerModels::KiterDPS)			{ return "KiterDPS"; }

	else if (type == PlayerModels::KiterSD)     { return "KiterSD"; }
	else if (type == PlayerModels::KiterMvmt)     { return "KiterMvmt"; }
	else if (type == PlayerModels::KiterEvo1)     { return "KiterEvo1"; }
	else if (type == PlayerModels::KiterEvo2)     { return "KiterEvo2"; }
	else if (type == PlayerModels::POE)     { return "POE"; }

	else if (type == PlayerModels::Kiter_NOKDPS)		{ return "Kiter_NOKDPS"; }
	else if (type == PlayerModels::Cluster)		    	{ return "Cluster"; }
	else if (type == PlayerModels::NOKDPS)              { return "NOKDPS"; }
	else if (type == PlayerModels::Random)				{ return "Random"; }
	else if (type == PlayerModels::PortfolioGreedySearch) { return "PGS"; }
	else													{ return "No name specified"; }
}