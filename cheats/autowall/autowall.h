#pragma once
#include "..\..\includes.hpp"

class weapon_info_t;
class weapon_t;

struct FireBulletData_t
{
	Vector            vecPosition = { };
	Vector            vecDirection = { };
	trace_t            enterTrace = { };
	float            flCurrentDamage = 0.0f;
	int                iPenetrateCount = 0;
	bool visible;
	int hitbox;
	int damage;
};

struct fire_bullet_data
{
	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

class autowall : public singleton <autowall>
{
public:

	struct returninfo_t
	{
		bool valid = false;

		bool visible = false;
		int damage = -1;
		int hitbox = -1;

		returninfo_t()
		{
			valid = false;

			visible = false;
			damage = -1;
			hitbox = -1;
		}

		returninfo_t(bool visible, int damage, int hitbox)
		{
			valid = true;

			this->visible = visible;
			this->damage = damage;
			this->hitbox = hitbox;
		}
	};

	struct FireBulletData
	{
		Vector src;
		trace_t enter_trace;
		Vector direction;
		CTraceFilter filter;
		weapon_info_t* wpn_data;
		float trace_length;
		float trace_length_remaining;
		float length_to_end;
		float current_damage;
		int penetrate_count;
	};

	static float GetDamag(const Vector& eye_pos, player_t*, const Vector& vecPoint, FireBulletData_t* pDataOut = nullptr);
	static bool SimulateFireBullet(player_t* pLocal, weapon_t* pWeapon, FireBulletData_t& data);
	static void ScaleDamage(const int iHitGroup, player_t* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage);
	static bool HandleBulletPenetration(player_t* pLocal, weapon_info_t* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data);

	// Main
	static void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, const unsigned int fMask, ITraceFilter* pFilter, trace_t* pTrace, const float flMinRange = 0.0f);
	static bool TraceToExit(trace_t& enterTrace, trace_t& exitTrace, const Vector& vecPosition, const Vector& vecDirection, player_t* pClipPlayer);
	bool is_breakable_entity(IClientEntity* e);
	bool IsBreakableEntity(IClientEntity* e);
	void scale_damage(player_t* e, CGameTrace& enterTrace, weapon_info_t* weaponData, float& currentDamage);
	bool trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, const Vector& direction);
	bool handle_bullet_penetration(weapon_info_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, const Vector& direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, bool draw_impact = false);
	returninfo_t wall_penetration(const Vector& eye_pos, Vector& point, IClientEntity* e);
	float can_hit_floating_point(Vector& vecEyePos, Vector& point);
	bool fire_bullet(weapon_t* pWeapon, Vector& direction, bool& visible, float& currentDamage, int& hitbox, IClientEntity* e = nullptr, float length = 0.f, const Vector& pos = { 0.f,0.f,0.f });
};