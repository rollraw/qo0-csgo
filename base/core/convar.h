#pragma once
// used: hash
#include "../sdk/hash/fnv1a.h"
// used: cconvar
#include "../sdk/interfaces/iconvar.h"

/*
 * CONVAR MANAGER
 * - game console variables
 */
namespace CONVAR
{
	bool Setup();
	void Dump(const wchar_t* wszFileName);

	/* @section: get */
	/// @param[in] uNameHash hash of console variable to find
	/// @returns: pointer to the console variable matched to given name, null otherwise
	CConVar* Find(const FNV1A_t uNameHash);

	/* @section: values */
	// settings
	inline CConVar* name = nullptr;
	inline CConVar* sensitivity = nullptr;
	// gameplay
	inline CConVar* game_mode = nullptr;
	inline CConVar* game_type = nullptr;
	inline CConVar* inferno_flame_lifetime = nullptr;
	// mouse
	inline CConVar* m_pitch = nullptr;
	inline CConVar* m_yaw = nullptr;
	// server
	inline CConVar* sv_autobunnyhopping = nullptr;
	inline CConVar* sv_coaching_enabled = nullptr;
	inline CConVar* sv_clip_penetration_traces_to_players = nullptr;
	inline CConVar* sv_maxunlag = nullptr;
	// client
	inline CConVar* cl_forwardspeed = nullptr;
	inline CConVar* cl_sidespeed = nullptr;
	inline CConVar* cl_upspeed = nullptr;
	// weapon
	inline CConVar* weapon_recoil_scale = nullptr;
	// friendly fire
	inline CConVar* ff_damage_reduction_bullets = nullptr;
	inline CConVar* ff_damage_bullet_penetration = nullptr;
	// multiplayer
	inline CConVar* mp_teammates_are_enemies = nullptr;
	inline CConVar* mp_damage_headshot_only = nullptr;
	inline CConVar* mp_damage_scale_ct_head = nullptr;
	inline CConVar* mp_damage_scale_ct_body = nullptr;
	inline CConVar* mp_damage_scale_t_head = nullptr;
	inline CConVar* mp_damage_scale_t_body = nullptr;
}
