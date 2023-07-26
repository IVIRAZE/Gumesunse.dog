// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"

#ifndef NOMINMAX2

#ifndef max2
#define max2(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min2
#define min2(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX2 */
#define DEBUG
#define RELEASE


int missed_shots[64];
std::vector<float>brutelist
{
		118.f,
		-120.f,
		180.f,
		-30.f,
		98,
		-2,
		+150.f,
		-119.f
};

std::string resolver_method[65];

float EyeAngles[64] = { 0 };

inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}

float ApproachAngle(float target, float value, float speed)
{
	target = anglemod(target);
	value = anglemod(value);

	float delta = target - value;

	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}



float resolver::b_yaw(player_t* player, float angle, int n)
{

	auto animState = player->get_animation_state();

	Vector velocity = player->m_vecVelocity();
	float spd = velocity.LengthSqr();
	if (spd > std::powf(1.2f * 260.0f, 2.f)) {
		Vector velocity_normalized = velocity.Normalized();
		velocity = velocity_normalized * (1.2f * 260.0f);


	}

	float Resolveyaw = animState->m_flGoalFeetYaw;

	auto delta_time
		= fmaxf(m_globals()->m_curtime - animState->m_flLastClientSideAnimationUpdateTime, 0.f);

	float deltatime = fabs(delta_time);
	float stop_to_full_running_fraction = 0.f;
	bool is_standing = true;
	float v25 = std::clamp(player->m_flDuckAmount() + animState->m_fLandingDuckAdditiveSomething, 0.0f, 1.0f);
	float v26 = animState->m_fDuckAmount;
	float v27 = deltatime * 6.0f;
	float v28;

	// clamp
	if ((v25 - v26) <= v27) {
		if (-v27 <= (v25 - v26))
			v28 = v25;
		else
			v28 = v26 - v27;
	}
	else {
		v28 = v26 + v27;
	}

	float flDuckAmount = std::clamp(v28, 0.0f, 1.0f);

	Vector animationVelocity = velocity;
	float speed = std::fminf(animationVelocity.Length(), 260.0f);

	auto weapon = player->m_hActiveWeapon().Get();


	auto wpndata = weapon->get_csweapon_info();


	float flMaxMovementSpeed = 260.0f;
	if (weapon) {
		flMaxMovementSpeed = std::fmaxf(wpndata->flMaxPlayerSpeed, 0.001f);
	}

	float flRunningSpeed = speed / (flMaxMovementSpeed * 0.520f);
	float flDuckingSpeed_2 = speed / (flMaxMovementSpeed * 0.340f);

	flRunningSpeed = std::clamp(flRunningSpeed, 0.0f, 1.0f);

	float flYawModifier = (((stop_to_full_running_fraction * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;
	if (flDuckAmount > 0.0f) {
		float flDuckingSpeed = std::clamp(flDuckingSpeed_2, 0.0f, 1.0f);
		flYawModifier += (flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier);
	}

	float flMaxBodyYaw = 58.f * flYawModifier;
	float flMinBodyYaw = -58.f * flYawModifier;


	//float flMaxBodyYaw = (*(float*)(uintptr_t(animState) + 0x338) * flYawModifier);
		//float flMinBodyYaw = (*(float*)(uintptr_t(animState) + 0x334) * flYawModifier);

	float flEyeYaw = player->m_angEyeAngles().y;

	float flEyeDiff = std::remainderf(flEyeYaw - Resolveyaw, 360.f);

	if (flEyeDiff <= flMaxBodyYaw) {
		if (flMinBodyYaw > flEyeDiff)
			Resolveyaw = fabs(flMinBodyYaw) + flEyeYaw;
	}
	else {
		Resolveyaw = flEyeYaw - fabs(flMaxBodyYaw);
	}

	if (speed > 0.1f || fabs(velocity.z) > 100.0f) {
		Resolveyaw = ApproachAngle(
			flEyeYaw,
			Resolveyaw,
			((stop_to_full_running_fraction * 20.0f) + 30.0f)
			* deltatime);
	}
	else {
		Resolveyaw = ApproachAngle(
			player->m_flLowerBodyYawTarget(),
			Resolveyaw,
			deltatime * 100.0f);
	}

	if (stop_to_full_running_fraction > 0.0 && stop_to_full_running_fraction < 1.0)
	{
		const auto interval = m_globals()->m_intervalpertick * 2.f;

		if (is_standing)
			stop_to_full_running_fraction = stop_to_full_running_fraction - interval;
		else
			stop_to_full_running_fraction = interval + stop_to_full_running_fraction;

		stop_to_full_running_fraction = std::clamp(stop_to_full_running_fraction, 0.f, 1.f);
	}

	if (speed > 135.2f && is_standing)
	{
		stop_to_full_running_fraction = fmaxf(stop_to_full_running_fraction, .0099999998f);
		is_standing = false;
	}

	if (speed < 135.2f && !is_standing)
	{
		stop_to_full_running_fraction = fminf(stop_to_full_running_fraction, .99000001f);
		is_standing = true;
	}

	//float Left = flEyeYaw + flMinBodyYaw;
	//float Right = flEyeYaw + flMaxBodyYaw;
	float gfy = Resolveyaw;
	//brute_yaw = std::remainderf(brute_yaw, 360.f);

	if (n == 1)
		return flMinBodyYaw;
	else if (n == 2)
		return flMaxBodyYaw;
	else if (n == 3)
		return flEyeYaw;

	if (n == 4)
	{
		return speed;
	}// get player speed

}

float resolver::get_angle(player_t* player) {
	return math::NormalizedAngle(player->m_angEyeAngles().y);
}

float resolver::get_backward_yaw(player_t* player) {
	return math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y;
}

float GetBackwardYaw(player_t* player)
{
	return math::calculate_angle(g_ctx.local()->GetAbsOrigin(), player->GetAbsOrigin()).y;
}

float resolver::get_forward_yaw(player_t* player) {
	return math::NormalizedAngle(get_backward_yaw(player) - 180.f);
}

bool CanSeeHitbox(player_t* entity, int HITBOX) {
	return g_ctx.local()->CanSeePlayer(entity, entity->hitbox_position(HITBOX));
}

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::initialize_yaw(player_t* e, adjust_data* record)
{
	player = e;

	player_record = record;

	player_record->left = b_yaw(player, player->m_angEyeAngles().y, 1);
	player_record->right = b_yaw(player, player->m_angEyeAngles().y, 2);
	player_record->middle = b_yaw(player, player->m_angEyeAngles().y, 3);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	safe_matrix_shot = false;
	freestand_side = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	was_roll_first_bruteforce = false;
	was_roll_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

int DetectStand(player_t* player)
{
	Vector src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	math::angle_vectors(Vector(0, GetBackwardYaw(player), 0), &forward, &right, &up);

	filter.pSkip = player;
	src3D = player->get_shoot_position();
	dst3D = src3D + (forward * 384); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.

	ray.Init(src3D, dst3D);
	m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 30, dst3D + right * 30);
	m_trace()->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 30, dst3D - right * 30);
	m_trace()->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	if (left_two > right_two)
		return -1;
	else if (right_two > left_two)
		return 1;
	else
		return 0;
}

resolver_side resolver::FreeStand(player_t* e)
{

    float angle = math::normalize_yaw(e->m_angEyeAngles().y);

    bool forward = fabsf(math::normalize_yaw(angle - get_forward_yaw(e))) < 90.f;

    auto misses = g_ctx.globals.missed_shots[e->EntIndex()];

    int S = DetectStand(e);

    bool HIGHDELTA = misses >= 3;


    switch (misses % 2)
    {
    case 0:
        switch (last_side)
        {
        case RESOLVER_SECOND:
            return RESOLVER_FIRST;
            break;
        case RESOLVER_FIRST:
            return RESOLVER_SECOND;
            break;
        default:
            if (S == 1)
            {
                if (forward)
                    return RESOLVER_SECOND;
                else
                    return RESOLVER_FIRST;
            }
            else
            {
                if (forward)
                    return RESOLVER_FIRST;
                else
                    return RESOLVER_SECOND;
            }
            break;
        }
        break;
    case 1:
        switch (last_side)
        {
        case RESOLVER_SECOND:
            return RESOLVER_SECOND;
            break;
        case RESOLVER_FIRST:
            return RESOLVER_FIRST;
            break;
        default:
            if (S == 1)
            {
                if (forward)
                    return RESOLVER_FIRST;
                else
                    return RESOLVER_SECOND;
            }
            else
            {
                if (forward)
                    return RESOLVER_SECOND;
                else
                    return RESOLVER_FIRST;
            }
            break;
        }
        break;
    }

}

bool ValidPitch(player_t* entity)
{
	int pitch = entity->m_angEyeAngles().x;
	return pitch == 0 || pitch > 90 || pitch < -90;
}

resolver_side resolver::TraceSide(player_t* entity)
{
	auto first_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.positive), player, g_ctx.local());
	auto second_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.negative), player, g_ctx.local());
	auto main_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.main), player, g_ctx.local());
	if (main_visible)
	{
		if (first_visible)
			return RESOLVER_SECOND;
		else if (second_visible)
			return  RESOLVER_FIRST;
	}
	else
	{
		if (first_visible)
			return  RESOLVER_FIRST;
		else if (second_visible)
			return  RESOLVER_SECOND;
	}

	return RESOLVER_ORIGINAL;
}

/*bool resolver::IsFakewalking(player_t* entity)
{
	bool
		bFakewalking = false,
		stage1 = false,            // stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;

	for (int i = 0; i < entity->animlayer_count(); i++)
	{
		if (player_record->layers[i].m_nSequence == 26 && player_record->layers[i].m_flWeight < 0.4f)
			stage1 = true;
		if (player_record->layers[i].m_nSequence == 7 && player_record->layers[i].m_flWeight > 0.001f)
			stage2 = true;
		if (player_record->layers[i].m_nSequence == 2 && player_record->layers[i].m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		bFakewalking = (stage3 || (player->m_fFlags() & FL_DUCKING)); // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2   
	else
		bFakewalking = false;

	return bFakewalking;
}*/

bool resolver::desync_detect()
{
	if (!player->is_alive())
		return false;
	if (~player->m_fFlags() & FL_ONGROUND)
		return false;
	if (~player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
		return false;
	if (!player->m_hActiveWeapon().Get()->can_fire(true))
		return false;
	if (player->get_move_type() == MOVETYPE_NOCLIP
		|| player->get_move_type() == MOVETYPE_LADDER)
		return false;

	return true;
}

bool delta_58(float first, float second)
{
	if (first - second < 58.f && first - second > -58.f)
	{
		return true;
	}
	return false;
}

bool delta_60(float first, float second)
{
	if (first - second < 60.f && first - second > -60.f)
	{
		return true;
	}
	return false;
}

bool delta_35(float first, float second)
{
	if (first - second <= 35.f && first - second >= -35.f)
	{
		return true;
	}
	return false;
}

float flAngleModx(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}
float ApproachAnglex(float target, float value, float speed)
{
	target = flAngleModx(target);
	value = flAngleModx(value);

	float delta = target - value;
	if (speed < 0)
		speed = -speed;

#pragma region release nn shit
#ifdef RELEASE
	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;
#endif

#pragma region debug nn shit
#ifdef DEBUG
	if (delta > 180)
		delta -= 360;
	else if (delta < 180)
		delta += 360;
#endif

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

/*
bool resolver::low_delta()
{
	auto record = player_record;

	if (!g_ctx.local()->is_alive())
		return false;

	float angle_diff = math::AngleDiff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);

	Vector first = ZERO, second = ZERO, third = ZERO;

	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);

	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;

	fl.pSkip = player;

	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);

	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);

	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 35 && lby > -35)
		return true;
	return false;
}

bool resolver::low_delta2()
{
	auto record = player_record;
	if (!g_ctx.local()->is_alive())
		return false;
	float angle_diff = math::AngleDiff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 20 && lby > -20)
		return true;
	return false;

}
*/

bool resolver::Saw(player_t* entity)
{
	if (!(CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT))
		|| (CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT)))
		return false;

	return true;
}

bool resolver::does_have_jitter(player_t* player, int* new_side)
{
	static float LastAngle[64];
	static int LastBrute[64];
	static bool Switch[64];
	static float LastUpdateTime[64];

	if (!math::IsNearEqual(player->m_angEyeAngles().y, LastAngle[player->EntIndex()], 58.f))
	{
		Switch[player->EntIndex()] = !Switch[player->EntIndex()];
		LastAngle[player->EntIndex()] = player->m_angEyeAngles().y;
		*new_side = Switch[player->EntIndex()] ? 3 : -3;
		LastBrute[player->EntIndex()] = *new_side;
		LastUpdateTime[player->EntIndex()] = m_globals()->m_curtime;
		return true;
	}
	else
	{
		if (fabsf(LastUpdateTime[player->EntIndex()] - m_globals()->m_curtime >= TICKS_TO_TIME(17)) || player->m_flSimulationTime() != player->m_flOldSimulationTime())
			LastAngle[player->EntIndex()] = player->m_angEyeAngles().y;

		*new_side = LastBrute[player->EntIndex()];
	}

	return false;
}

void resolver::antifreestand()
{
	if (!g_ctx.local()->is_alive())
		return;

	if (!g_ctx.globals.weapon->IsGun())
		return;


	for (int i = 0; i < m_engine()->GetMaxClients(); ++i)
	{
		auto player = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (!player || !player->is_alive() || player->IsDormant()
			|| !player->is_player() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum() || g_ctx.globals.missed_shots[player->EntIndex()] > 0 || player->EntIndex() == m_engine()->GetLocalPlayer())
			continue;

		float angToLocal = math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y;
		Vector ViewPoint = g_ctx.local()->m_vecOrigin() + Vector(0, 0, 90);

		Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

		Vector2D Side3 = { (60 * sin(DEG2RAD(angToLocal))),(60 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side4 = { (60 * sin(DEG2RAD(angToLocal + 180))) ,(60 * cos(DEG2RAD(angToLocal + 180))) };

		Vector Origin = player->m_vecOrigin();

		Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };
		Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

		// zZz...
		for (int side = 0; side < 2; side++)
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
			Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (autowall::get().can_hit_floating_point(OriginAutowall, ViewPoint))
			{
				if (side == 0)
				{
					freestand_side = true;
				}
				else if (side == 1)
				{
					freestand_side = false;
				}
			}
			else
			{
				for (int sidealternative = 0; sidealternative < 2; sidealternative++)
				{
					Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x,  Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

					if (autowall::get().can_hit_floating_point(ViewPointAutowallalternative, ViewPointAutowall))
					{
						if (sidealternative == 0)
						{
							freestand_side = true;
						}
						else if (sidealternative == 1)
						{
							freestand_side = false;
						}
					}
				}
			}
		}

		//if (!DetectSide(player, freestand_side))  
			//return;
	}
}

void resolver::resolve_yaw()
{
	
	
	player_info_t player_info;

	auto animstate = player->get_animation_state();

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum() || player->get_move_type() == MOVETYPE_LADDER || player->get_move_type() == MOVETYPE_NOCLIP)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	safe_matrix_shot = animstate->m_velocity > 180.0f;

	if (!animstate || !desync_detect() || !shot_resolver())
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	resolver_history res_history = HISTORY_UNKNOWN;

	for (auto it = lagcompensation::get().player_sets.begin(); it != lagcompensation::get().player_sets.end(); ++it)
	{
		if (it->id == player_info.steamID64)
		{
			res_history = it->res_type;
			is_player_faking = it->faking;
			break;
		}
	}

	if (res_history == HISTORY_ZERO)
		is_player_zero = true;

	resolver_type type = resolver_type(-1);

	auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
	bool forward = fabsf(math::normalize_yaw(get_angle(player) - get_forward_yaw(player))) < 90.f;

	bool low_delta = false;

	auto first_side = low_delta ? RESOLVER_LOW_FIRST : RESOLVER_FIRST;
	auto second_side = low_delta ? RESOLVER_LOW_SECOND : RESOLVER_SECOND;

	auto jitter_first_side = low_delta ? RESOLVER_JITTER_FIRST : RESOLVER_FIRST;
	auto jitter_second_side = low_delta ? RESOLVER_JITTER_SECOND : RESOLVER_SECOND;

	int new_side = player_record->side = RESOLVER_ORIGINAL;

	if (player->m_vecVelocity().Length2D() > player->GetMaxPlayerSpeed() || !player->m_fFlags() && FL_ONGROUND)
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
			player_record->side = freestand_side ? second_side : first_side;
		else if (Saw(player))
			if (TraceSide(player) != RESOLVER_ORIGINAL)
				player_record->side = TraceSide(player);

		if (does_have_jitter(player, &new_side) && player->m_angEyeAngles().x < 45.0f)
		{
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				resolver_method[player->EntIndex()] = "Detect Jitter 1";
				animstate->m_flGoalFeetYaw = math::normalize_yaw(get_angle(player) +
					freestand_side ? jitter_second_side : jitter_first_side * new_side);
				break;
			case 1:
				resolver_method[player->EntIndex()] = "Detect Jitter 2";
				animstate->m_flGoalFeetYaw = math::normalize_yaw(get_angle(player) -
					freestand_side ? jitter_first_side : jitter_second_side * new_side);
				break;
			}
		}
	}
	else
		player_record->side = RESOLVER_ORIGINAL;

	if (player->m_vecVelocity().Length2D() > 50)
		records.move_lby[player->EntIndex()] = animstate->m_flGoalFeetYaw;

	auto valid_lby = true;
	bool move_anim = false;

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;

	if (int(player_record->layers[3].m_flWeight == 0.f && player_record->layers[3].m_flCycle == 0.f && player_record->layers[6].m_flWeight == 0.f))
		move_anim = true;

	bool ducking = animstate->m_fDuckAmount && player->m_fFlags() & FL_ONGROUND && !animstate->m_bInHitGroundAnimation;

	if (valid_lby && move_anim || player->m_vecVelocity().Length() >= player->GetMaxPlayerSpeed() && ducking || player->m_vecVelocity().Length() >= player->GetMaxPlayerSpeed() && !ducking)
	{
		auto m_delta = math::AngleDiff(player->m_angEyeAngles().y, resolver_goal_feet_yaw[0]);
		player_record->side = m_delta <= 0.0f ? first_side : second_side;
	}
	else
	{
		if (fabs(delta) > 35.0f && valid_lby)
		{
			if (player->m_fFlags() & FL_ONGROUND || player->m_vecVelocity().Length2D() <= 2.0f)
			{
				if (player->sequence_activity(player_record->layers[3].m_nSequence) == 979)
					if (g_ctx.globals.missed_shots[player->EntIndex()])
						delta = -delta;
					else
						player_record->side = delta > 0 ? first_side : second_side;
			}
		}
		else if (!valid_lby && !(int(player_record->layers[12].m_flWeight * 1000.f)) && static_cast<int>(player_record->layers[6].m_flCycle * 1000.f) == static_cast<int>(previous_layers[6].m_flWeight * 1000.f))
		{
			float delta_third = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[0][6].m_flPlaybackRate);
			float delta_second = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[1][6].m_flPlaybackRate);
			float delta_first = abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[2][6].m_flPlaybackRate);

			if (delta_first < delta_second || delta_third <= delta_second || (delta_second * 1000.f))
			{
				if (delta_first >= delta_third && delta_second > delta_third && !(delta_third * 1000.f))
				{
					player_record->type = ANIMATION;
					player_record->side = first_side;
				}
				else
				{
					if (forward)
					{
						player_record->type = DIRECTIONAL;
						player_record->side = freestand_side ? second_side : first_side;
					}
					else
					{
						player_record->type = DIRECTIONAL;
						player_record->side = freestand_side ? first_side : second_side;
					}
				}
			}
			else
			{
				player_record->type = ANIMATION;
				player_record->side = second_side;
			}
		}
		else
		{
			if (m_globals()->m_curtime - lock_side > 2.0f)
			{
				auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
				auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

				if (first_visible != second_visible)
				{
					freestand_side = true;
					side = second_visible;
					lock_side = m_globals()->m_curtime;
				}
				else
				{
					auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
					auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

					if (fabs(first_position - second_position) > 1.0f)
						freestand_side = first_position > second_position;
				}
			}
			else
				freestand_side = true;

			if (forward)
			{
				player_record->type = DIRECTIONAL;
				player_record->side = freestand_side ? second_side : first_side;
			}
			else
			{
				player_record->type = DIRECTIONAL;
				player_record->side = freestand_side ? first_side : second_side;
			}
		}
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] && aim::get().last_target[player->EntIndex()].record.type != LBY && aim::get().last_target[player->EntIndex()].record.type != JITTER)
	{
		switch (last_side)
		{
		case RESOLVER_ORIGINAL:
			g_ctx.globals.missed_shots[player->EntIndex()] = 0;
			fake = true;
			break;
		case RESOLVER_ZERO:
			player_record->type = BRUTEFORCE;

			if (!is_player_zero)
				player_record->side = /*freestand_side ? second_side : */ first_side;
			else
				player_record->side = /*freestand_side ? first_side : */ second_side;

			was_first_bruteforce = false; was_roll_first_bruteforce = false;
			was_second_bruteforce = false; was_roll_second_bruteforce = false;
			return;
		case RESOLVER_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

			was_first_bruteforce = true;
			return;
		case RESOLVER_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

			was_second_bruteforce = true;
			return;
		case RESOLVER_LOW_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_SECOND;
			return;
		case RESOLVER_LOW_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_FIRST;
			return;
		case RESOLVER_DEFAULT:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_FIRST;
			return;
		}
	}

	//auto animstate = player->get_animation_state();

	if (!animstate)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (!ValidPitch(player))
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
			player_record->side = RESOLVER_ORIGINAL;
		else
			player_record->side = FreeStand(player);
		return;
	}

	//auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
	//auto valid_lby = true;

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;


	player_record->type = BRUTEFORCE;
	player_record->side = RESOLVER_DEFAULT;

	// ghetto way
	auto choked = TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime());

	// if his pitch down, or he is choking or we already hitted in desync, we can say that he might use desync
	if (fabs(original_pitch) > 85.f || choked >= 1 || is_player_faking)
		fake = true;
	else if (!fake && !g_ctx.globals.missed_shots[player->EntIndex()])
	{
		player_record->type = ORIGINAL;
		return;
	}

	if (side)
	{
		player_record->type = type;
		player_record->side = freestand_side ? second_side : first_side;
	}
	else
	{
		player_record->type = type;
		player_record->side = freestand_side ? first_side : second_side;
	}

	if (is_player_zero && !g_ctx.globals.missed_shots[player->EntIndex()])
		player_record->side = RESOLVER_ZERO;

	if (!player->IsDormant()) {
		if (side >= -1)
			animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 58;
		else
			animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 58;

		for (; animstate->m_flGoalFeetYaw > 180.0f; animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 360);
		for (; animstate->m_flGoalFeetYaw < -180.0f; animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 360);
	}
	else {
		if (side <= 1)
			animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 58;
		else
			animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 58;

		for (; animstate->m_flGoalFeetYaw > 180.0f; animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 360);
		for (; animstate->m_flGoalFeetYaw < -180.0f; animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 360);
	}

	if ((player_record->player) && player_record->player->m_vecVelocity().Length2D() <= 0.15)
		return;
	else
		return;
	player_record->layers[3].m_flCycle == 0.f && player_record->layers[3].m_flWeight == 0.f;

	if (player) {
		freestand_side = player_record->player->m_flLowerBodyYawTarget();
	}
}

bool resolver::shot_resolver()
{
	bool m_shot;
	float m_flLastShotTime;

	m_flLastShotTime = player->m_hActiveWeapon() ? player->m_hActiveWeapon()->m_fLastShotTime() : 0.f;
	m_shot = m_flLastShotTime > player->m_flOldSimulationTime() && m_flLastShotTime <= player->m_flSimulationTime();

	if (m_flLastShotTime <= player->m_flSimulationTime() && m_shot)
		player_record->side = RESOLVER_ORIGINAL;

	return true;
}

float resolver::resolve_pitch()
{
	return original_pitch;
}


bool resolver::is_slow_walking()
{
	auto entity = player;
	//float large = 0;
	float velocity_2D[64], old_velocity_2D[64];
	if (entity->m_vecVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->m_vecVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->m_vecVelocity().Length2D();
	}
	//if (large == 0)return false;
	Vector velocity = entity->m_vecVelocity();
	Vector direction = entity->m_angEyeAngles();

	float speed = velocity.Length();
	direction.y = entity->m_angEyeAngles().y - direction.y;
	//method 1
	if (velocity_2D[entity->EntIndex()] > 1) {
		int tick_counter[64];
		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			tick_counter[entity->EntIndex()] += 1;
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / m_globals()->m_intervalpertick) * fabsf(0.1f))//should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
			return true;
	}


	return false;
}
int last_ticks[65];
int resolver::GetChokedPackets() {
	auto ticks = TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime());
	if (ticks == 0 && last_ticks[player->EntIndex()] > 0) {
		return last_ticks[player->EntIndex()] - 1;
	}
	else {
		last_ticks[player->EntIndex()] = ticks;
		return ticks;
	}
}
void resolver::layer_test()
{
	player_record->type = LAYERS;

	float center = (abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[0][6].m_flPlaybackRate)) * 1000.f;
	float positive_full = (abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[1][6].m_flPlaybackRate)) * 1000.f;
	float negative_full = (abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[2][6].m_flPlaybackRate)) * 1000.f;
	float positive_40 = (abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[3][6].m_flPlaybackRate)) * 1000.f;
	float negative_40 = (abs(player_record->layers[6].m_flPlaybackRate - resolver_layers[4][6].m_flPlaybackRate)) * 1000.f;

	if ((positive_full > center && negative_full <= center) || (positive_40 > center && negative_40 <= center))
		player_record-> curSide = lft;


	else if ((negative_full > center && positive_full <= center) || (negative_40 > center && positive_40 <= center))
		player_record->curSide = rght;

	else
		get_side_trace();


}

float angle_diff_onetap(float a1, float a2)
{
	float i;

	for (; a1 > 180.0f; a1 = a1 - 180.0f)
		;
	for (; 180.0f > a1; a1 = a1 + 360.0f)
		;
	for (; a2 > 180.0f; a2 = a2 - 360.0f)
		;
	for (; 180.0f > a2; a2 = a2 + 360.0f)
		;
	for (i = a2 - a1; i > 180.0f; i = i - 360.0f)
		;
	for (; 180.0f > i; i = i + 360.0f)
		;
	return i;
}
void resolver::get_side_standing()
{
	player_record->type = LBY;
	float EyeDelta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);

	if (fabs(EyeDelta) > 25.0f)
	{

		if (EyeDelta > 25.0f)
			player_record->curSide = rght;

		else if (EyeDelta < -25.0f)
			player_record->curSide = lft;
	}
	else
		get_side_trace();
}


float get_backward_side(player_t* player)
{
	return math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y;
}



void resolver::detect_side()
{
	player_record->type = ENGINE;
	/* externs */
	Vector src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	CGameTrace tr;
	CTraceFilter filter;

	/* angle vectors */
	math::angle_vectors(Vector(0, get_backward_side(player), 0), &forward, &right, &up);

	/* filtering */
	filter.pSkip = player;
	src3D = player->get_shoot_position();
	dst3D = src3D + (forward * 384);

	/* back engine tracers */
	m_trace()->TraceRay(Ray_t(src3D, dst3D), MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	/* right engine tracers */
	m_trace()->TraceRay(Ray_t(src3D + right * 35, dst3D + right * 35), MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	/* left engine tracers */
	m_trace()->TraceRay(Ray_t(src3D - right * 35, dst3D - right * 35), MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	/* fix side */
	if (left_two > right_two) {
		player_record->curSide = lft;
	}
	else if (right_two > left_two) {
		player_record->curSide = rght;
	}
	else
		get_side_trace();
}

void resolver::get_side_trace()
{
	auto m_side = false;
	auto trace = false;
	if (m_globals()->m_curtime - lock_side > 2.0f)
	{
		auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
		auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

		if (first_visible != second_visible)
		{
			trace = true;
			m_side = second_visible;
			lock_side = m_globals()->m_curtime;
		}
		else
		{
			auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
			auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

			if (fabs(first_position - second_position) > 1.0f)
				m_side = first_position > second_position;
		}
	}
	else
		trace = true;

	if (m_side)
	{
		player_record->type = trace ? TRACE : DIRECTIONAL;
		player_record->curSide = rght;
	}
	else
	{
		player_record->type = trace ? TRACE : DIRECTIONAL;
		player_record->curSide = lft;
	}
}



bool resolver::DesyncDetect()
{
	if (!player->is_alive())
		return false;
	if (player->get_max_desync_delta() < 10)
		return false;
	if (player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
		return false;
	if (player->get_move_type() == MOVETYPE_NOCLIP || player->get_move_type() == MOVETYPE_LADDER)
		return false;

	return true;
}

bool resolver::update_walk_data()
{
	auto e = player;


	auto anim_layers = player_record->layers;
	bool s_1 = false,
		s_2 = false,
		s_3 = false;

	for (int i = 0; i < e->animlayer_count(); i++)
	{
		anim_layers[i] = e->get_animlayers()[i];
		if (anim_layers[i].m_nSequence == 26 && anim_layers[i].m_flWeight < 0.47f)
			s_1 = true;
		if (anim_layers[i].m_nSequence == 7 && anim_layers[i].m_flWeight > 0.001f)
			s_2 = true;
		if (anim_layers[i].m_nSequence == 2 && anim_layers[i].m_flWeight == 0)
			s_3 = true;
	}
	bool  m_fakewalking;
	if (s_1 && s_2)
		if (s_3)
			m_fakewalking = true;
		else
			m_fakewalking = false;
	else
		m_fakewalking = false;

	return m_fakewalking;
}

bool resolver::IsAdjustingBalance()
{


	for (int i = 0; i < 15; i++)
	{
		const int activity = player->sequence_activity(player_record->layers[i].m_nSequence);
		if (activity == 979)
		{
			return true;
		}
	}
	return false;
}

bool resolver::is_breaking_lby(AnimationLayer cur_layer, AnimationLayer prev_layer)
{
	if (IsAdjustingBalance())
	{
		if (IsAdjustingBalance())
		{
			if ((prev_layer.m_flCycle != cur_layer.m_flCycle) || cur_layer.m_flWeight == 1.f)
			{
				return true;
			}
			else if (cur_layer.m_flWeight == 0.f && (prev_layer.m_flCycle > 0.92f && cur_layer.m_flCycle > 0.92f))
			{
				return true;
			}
		}
		return false;
	}

	return false;
}

void resolver::setmode()
{
	auto e = player;

	//float speed = e->m_vecVelocity().Length2D();
	float speed = b_yaw(player, player->m_angEyeAngles().y, 4);

	auto cur_layer = player_record->layers;
	auto prev_layer = player_record->previous_layers;

	bool on_ground = e->m_fFlags() & FL_ONGROUND && !e->get_animation_state()->m_bInHitGroundAnimation;

	bool slow_walking1 = is_slow_walking();
	bool slow_walking2 = update_walk_data();

	bool flicked_lby = abs(player_record->layers[3].m_flWeight - player_record->previous_layers[7].m_flWeight) >= 1.1f;
	bool breaking_lby = is_breaking_lby(cur_layer[3], prev_layer[3]);


	bool ducking = player->get_animation_state()->m_fDuckAmount && e->m_fFlags() & FL_ONGROUND && !player->get_animation_state()->m_bInHitGroundAnimation;



	bool stand_anim = false;
	if (player_record->layers[3].m_flWeight == 0.f && player_record->layers[3].m_flCycle == 0.f)
		stand_anim = true;

	bool move_anim = false;
	//if (int(player_record->layers[6].m_flWeight * 1000.f) == int(previous_layers[6].m_flWeight * 1000.f))
	if ((player_record->layers[6].m_flWeight * 1000.f) == (previous_layers[6].m_flWeight * 1000.f))
		move_anim = true;

	auto animstate = player->get_animation_state();
	if (!animstate)
		return;

	auto valid_move = true;
	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_move = animstate->m_flTimeSinceStartedMoving < 0.22f;


	if (!on_ground)
	{
		player_record->curMode = AIR;
	}
	else if ((/*micromovement check pog*/ (speed < 3.1f && ducking) || (speed < 1.2f && !ducking)))
	{
		player_record->curMode = STANDING;

	}
	else if (/*micromovement check pog*/ ((speed >= 3.1f && ducking) || (speed >= 1.2f && !ducking)))
	{
		if ((speed >= 1.2f && speed < 134.f) && !ducking && (slow_walking1 || slow_walking2))
			player_record->curMode = SLOW_WALKING;
		else
			player_record->curMode = MOVING;
	}
	else
		player_record->curMode = FREESTANDING;
}

bool resolver::MatchShot()
{
	// do not attempt to do this in nospread mode.

	float shoot_time = -1.f;

	auto weapon = player->m_hActiveWeapon();
	if (weapon) {
		// with logging this time was always one tick behind.
		// so add one tick to the last shoot time.
		shoot_time = weapon->m_fLastShotTime() + m_globals()->m_intervalpertick;
	}

	// this record has a shot on it.
	if (TIME_TO_TICKS(shoot_time) == TIME_TO_TICKS(player->m_flSimulationTime()))
	{
		return true;
	}

	return false;
}



void resolver::final_detection()
{
	switch (player_record->curMode)
	{
	case MOVING:
		layer_test();
		break;
	case STANDING:
		get_side_standing();
		break;
	case FREESTANDING:
		get_side_trace();
		break;
	case SLOW_WALKING:
		layer_test();
		break;

	}
}

void resolver::missed_shots_correction(adjust_data* record, int missed_shots)
{

	switch (missed_shots)
	{
	case 0:
		restype[record->type].missed_shots_corrected[record->player->EntIndex()] = 0;
		break;
	case 1:
		restype[record->type].missed_shots_corrected[record->player->EntIndex()] = 1;
		break;
	case 2:
		restype[record->type].missed_shots_corrected[record->player->EntIndex()] = 2;
		break;
	case 3:
		restype[record->type].missed_shots_corrected[record->player->EntIndex()] = 3;
		break;
	}
}


void resolver::resolve_desync()
{
	if (!DesyncDetect())
	{
		player_record->side = RESOLVER_ORIGINAL;
		player_record->desync_amount = 0;
		player_record->curMode = NO_MODE;
		player_record->curSide = no_sie;
		return;
	}
	auto e = player;
	//
	auto negative = player_record->left;
	auto positive = player_record->rght;
	auto gfy = player_record->middle;
	//

	bool mside;
	auto pWeapon = player->m_hActiveWeapon();
	auto simtime = player->m_flSimulationTime();
	auto oldsimtime = player->m_flOldSimulationTime();
	float m_flLastShotTime;
	bool m_shot;
	m_flLastShotTime = pWeapon ? pWeapon->m_fLastShotTime() : 0.f;
	m_shot = m_flLastShotTime > oldsimtime && m_flLastShotTime <= simtime;

	setmode();

	if (m_flLastShotTime <= simtime && m_shot || MatchShot())
	{
		player_record->side = RESOLVER_ON_SHOT;
		player_record->desync_amount = 0;
		player_record->curSide = no_sie;
		player_record->shot = true;
		return;
	}

	if (player_record->curMode == AIR)
	{
		player_record->side = RESOLVER_ORIGINAL;
		player_record->desync_amount = 0;
		player_record->curMode = AIR;
		player_record->curSide = no_sie;
		return;
	}

	final_detection();


	float LowDeltaFactor = 0.45f; //testing values :3

	missed_shots_correction(player_record, g_ctx.globals.missed_shots[player->EntIndex()]); // we brute each type (layers,lby...etc) seperatly :3

	// start bruting if we miss baby :3
	switch (restype[player_record->type].missed_shots_corrected[player_record->player->EntIndex()])
	{
	case 0:// skip :3 we resolve bellow with logic
		break;
	case 1:// low delta
		g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()] = false;
		g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()] = true;
		break;
	case 2:// flipped full delta
		g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()] = true;
		g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()] = false;
		break;
	case 3:// flipped low delta
		g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()] = true;
		g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()] = true;
		break;
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] > 3)
		g_ctx.globals.missed_shots[player->EntIndex()] = 0;

	if (restype[player_record->type].missed_shots_corrected[player_record->player->EntIndex()] > 3)
		restype[player_record->type].missed_shots_corrected[player_record->player->EntIndex()] = 0;

	if (player_record->curSide == lft)
	{
		player_record->desync_amount = negative;

		if (g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()])
			player_record->desync_amount = positive;

		if (g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()])
			player_record->desync_amount *= LowDeltaFactor;
	}

	else if (player_record->curSide == rght)
	{
		player_record->desync_amount = positive;

		if (g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()])
			player_record->desync_amount = negative;

		if (g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()])
			player_record->desync_amount *= LowDeltaFactor;
	}

	// for logs xD
	player_record->flipped_s = g_cfg.player_list.types[player_record->type].should_flip[e->EntIndex()];
	player_record->low_delta_s = g_cfg.player_list.types[player_record->type].low_delta[e->EntIndex()];
	g_ctx.globals.mlog1[e->EntIndex()] = player_record->desync_amount;

	// set player's gfy to guessed desync value :3
	player->get_animation_state()->m_flGoalFeetYaw = math::normalize_yaw(e->m_angEyeAngles().y + player_record->desync_amount);

}