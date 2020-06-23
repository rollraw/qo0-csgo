#pragma once
// used: std:unordered_map
#include <unordered_map>

// used: winapi includes, singleton
#include "../common.h"
// used: itemdefinitionindex; baseentity, baseweapon classes
#include "../sdk/entity.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"

struct SkinObject_t
{
	SkinObject_t(const char* szName, const char* szModel, const char* szKillIcon = nullptr)
		: szName(szName), szModel(szModel), szKillIcon(szKillIcon) { }

	const char* szName = nullptr;
	const char* szModel = nullptr;
	const char* szKillIcon = nullptr;
};

// @note: u can find viewmodel indexes with "sv_precacheinfo"
const std::unordered_map<int, SkinObject_t> mapItemList =
{
	{ WEAPON_DEAGLE, { "Desert Eagle", "models/weapons/v_pist_deagle.mdl", "deagle" } },
	{ WEAPON_ELITE, { "Dual Berettas", "models/weapons/v_pist_elite.mdl", "elite" } },
	{ WEAPON_FIVESEVEN, { "Five-SeveN", "models/weapons/v_pist_fiveseven.mdl", "fiveseven" } },
	{ WEAPON_GLOCK, { "Glock-18", "models/weapons/v_pist_glock18.mdl", "glock" } },
	{ WEAPON_AK47, { "AK-47", "models/weapons/v_rif_ak47.mdl", "ak47" } },
	{ WEAPON_AUG, { "AUG", "models/weapons/v_rif_aug.mdl", "aug" } },
	{ WEAPON_AWP, { "AWP", "models/weapons/v_snip_awp.mdl", "awp" } },
	{ WEAPON_FAMAS, { "FAMAS", "models/weapons/v_rif_famas.mdl", "famas" } },
	{ WEAPON_G3SG1, { "G3SG1", "models/weapons/v_snip_g3sg1.mdl", "g3sg1" } },
	{ WEAPON_GALILAR, { "Galil AR", "models/weapons/v_rif_galilar.mdl", "galilar" } },
	{ WEAPON_M249, { "M249", "models/weapons/v_mach_m249para.mdl", "m249" } },
	{ WEAPON_M4A1, { "M4A4", "models/weapons/v_rif_m4a1.mdl", "m4a1" } },
	{ WEAPON_MAC10, { "MAC-10", "models/weapons/v_smg_mac10.mdl", "mac10" } },
	{ WEAPON_P90, { "P90", "models/weapons/v_smg_p90.mdl", "p90" } },
	{ WEAPON_MP5SD, { "MP5-SD", "models/weapons/v_smg_mp5sd.mdl", "mp5sd" } },
	{ WEAPON_UMP45, { "UMP-45", "models/weapons/v_smg_ump45.mdl", "ump45" } },
	{ WEAPON_XM1014, { "XM1014", "models/weapons/v_shot_xm1014.mdl", "xm1014" } },
	{ WEAPON_BIZON, { "PP-Bizon", "models/weapons/v_smg_bizon.mdl", "bizon" } },
	{ WEAPON_MAG7, { "MAG-7", "models/weapons/v_shot_mag7.mdl", "mag7" } },
	{ WEAPON_NEGEV, { "Negev", "models/weapons/v_mach_negev.mdl", "negev" } },
	{ WEAPON_SAWEDOFF, { "Sawed-Off", "models/weapons/v_shot_sawedoff.mdl", "sawedoff" } },
	{ WEAPON_TEC9, { "Tec-9", "models/weapons/v_pist_tec9.mdl", "tec9" } },
	{ WEAPON_HKP2000, { "P2000", "models/weapons/v_pist_hkp2000.mdl", "hkp2000" } },
	{ WEAPON_MP7, { "MP7", "models/weapons/v_smg_mp7.mdl", "mp7" } },
	{ WEAPON_MP9, { "MP9", "models/weapons/v_smg_mp9.mdl", "mp9" } },
	{ WEAPON_NOVA, { "Nova", "models/weapons/v_shot_nova.mdl", "nova" } },
	{ WEAPON_P250, { "P250", "models/weapons/v_pist_p250.mdl", "p250" } },
	{ WEAPON_SCAR20, { "SCAR-20", "models/weapons/v_snip_scar20.mdl", "scar20" } },
	{ WEAPON_SG556, { "SG 553", "models/weapons/v_rif_sg556.mdl", "sg556" } },
	{ WEAPON_SSG08, { "SSG 08", "models/weapons/v_snip_ssg08.mdl", "ssg08" } },
	{ WEAPON_KNIFE, { "Knife (Counter-Terrorists)", "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ WEAPON_KNIFE_T, { "Knife (Terrorists)", "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ WEAPON_M4A1_SILENCER, { "M4A1-S", "models/weapons/v_rif_m4a1_s.mdl", "m4a1_silencer" } },
	{ WEAPON_USP_SILENCER, { "USP-S", "models/weapons/v_pist_223.mdl", "usp_silencer" } },
	{ WEAPON_CZ75A, { "CZ75 Auto", "models/weapons/v_pist_cz_75.mdl", "cz75a" } },
	{ WEAPON_REVOLVER, { "R8 Revolver", "models/weapons/v_pist_revolver.mdl", "revolver" } },
	{ WEAPON_KNIFE_BAYONET, { "Bayonet", "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ WEAPON_KNIFE_FLIP, { "Flip Knife", "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ WEAPON_KNIFE_GUT, { "Gut Knife", "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ WEAPON_KNIFE_KARAMBIT, { "Karambit", "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ WEAPON_KNIFE_M9_BAYONET, { "M9 Bayonet", "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ WEAPON_KNIFE_TACTICAL, { "Huntsman Knife", "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ WEAPON_KNIFE_FALCHION, { "Falchion Knife", "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, { "Bowie Knife", "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ WEAPON_KNIFE_BUTTERFLY, { "Butterfly Knife", "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ WEAPON_KNIFE_PUSH, { "Shadow Daggers", "models/weapons/v_knife_push.mdl", "knife_push" } },
	{ WEAPON_KNIFE_CORD, { "Paracord Knife", "models/weapons/v_knife_cord.mdl", "knife_cord" } },
	{ WEAPON_KNIFE_CANIS, { "Survival Knife", "models/weapons/v_knife_canis.mdl", "knife_canis" } },
	{ WEAPON_KNIFE_URSUS, { "Ursus Knife", "models/weapons/v_knife_ursus.mdl", "knife_ursus" } },
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE, { "Navaja Knife", "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife" } },
	{ WEAPON_KNIFE_OUTDOOR, { "Nomad Knife", "models/weapons/v_knife_outdoor.mdl", "knife_outdoor" } },
	{ WEAPON_KNIFE_STILETTO, { "Stiletto Knife", "models/weapons/v_knife_stiletto.mdl", "knife_stiletto" } },
	{ WEAPON_KNIFE_WIDOWMAKER, { "Talon Knife", "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker" } },
	{ WEAPON_KNIFE_SKELETON, { "Skeleton Knife", "models/weapons/v_knife_skeleton.mdl", "knife_skeleton" } },
};

const std::pair<std::size_t, std::string> arrItemQuality[] =
{
	{ 0, "Normal" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

// @todo: add valve vdf/vpk parser and get skins, rarity things, etc with it
class CSkinChanger : public CSingleton<CSkinChanger>
{
public:
	// Get
	void Run();
	/* knife kill icons replace */
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);
	/* dump stikers and paintkits */
	void Dump();
private:
	/* ur values and main functionality */
};
