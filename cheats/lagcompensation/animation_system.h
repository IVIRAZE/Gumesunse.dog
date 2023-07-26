#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum
{
	MAIN,
	NONE,
	FIRST,
	SECOND,
	LOW_FIRST,
	LOW_SECOND,
	LOW_FIRST_20,
	LOW_SECOND_20
};

enum resolver_type
{
	ORIGINAL,
	LBY,
	LAYERS,
	TRACE,
	JITTER,
	BRUTEFORCE,
	ANIMATION,
	DIRECTIONAL,
	ENGINE,
	FREESTAND,
	HURT,
	NON_RESOLVED
};

enum resolver_history
{
	HISTORY_UNKNOWN = -1,
	HISTORY_ORIGINAL,
	HISTORY_ZERO,
	HISTORY_DEFAULT,
	HISTORY_LOW
};

enum resolver_side
{
	RESOLVER_ORIGINAL,
	RESOLVER_DEFAULT,
	RESOLVER_ZERO,
	RESOLVER_FIRST,
	RESOLVER_SECOND,
	RESOLVER_LOW_FIRST,
	RESOLVER_LOW_SECOND,
	RESOLVER_LOW_FIRST_20,
	RESOLVER_LOW_SECOND_20,
	RESOLVER_JITTER_FIRST,
	RESOLVER_JITTER_SECOND,
	RESOLVER_ON_SHOT
};

enum modes
{
	AIR,
	SLOW_WALKING,
	MOVING,
	STANDING,
	FREESTANDING,
	NO_MODE
};

enum get_side_move
{
	no_sie,
	lft,
	rght,
};


/*
enum AnimationLayer_t
{
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT
};
*/
/*
class CResolver
{
	player_t* player = nullptr;
	adjust_data *player_record = nullptr;

	bool side = false;
	bool fake = false;
	bool was_first_bruteforce = false;
	bool was_second_bruteforce = false;

	struct
	{
		int missed_shots_corrected[65] = { 0 };;
	}restype[8];


	float lock_side = 0.0f;
	float original_goal_feet_yaw = 0.0f;
	float original_pitch = 0.0f;

public:
	void initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch);
	void initialize_yaw(player_t* e, adjust_data* record);
	void reset();

	bool IsAdjustingBalance();

	bool is_breaking_lby(AnimationLayer cur_layer, AnimationLayer prev_layer);

	void get_side_moving();

	void check_low();

	void layer_test();

	void layer_detection();

	bool low_delta();

	void get_side_standing();
	void detect_side();
	void get_side_trace();
	int GetChokedPackets();
	bool DesyncDetect();
	bool update_walk_data();
	void setmode();
	bool MatchShot();
	bool is_slow_walking();


	void final_detection();

	void missed_shots_correction(adjust_data* record, int missed_shots);

	void layer_ot4();

	float build_yaw(player_t* player, float angle, int n);

	float b_yaw(player_t* player, float angle, int n);

	float get_velocity(player_t* player);

	bool hrt_res();

	void resolve();

	float resolve_pitch();
	void laith(float p, float n);

	void resolve_desync();

	bool hurt_resolver(shot_info* shot);

	//void hurt_resolver();


	AnimationLayer resolver_layers[7][13];
	AnimationLayer previous_layers[13];
	float resolver_goal_feet_yaw[7];



	resolver_side last_side = RESOLVER_ORIGINAL;
	resolver_type type = ORIGINAL;
};
*/

struct matrixes
{
	matrix3x4_t main[MAXSTUDIOBONES];
	matrix3x4_t zero[MAXSTUDIOBONES];
	matrix3x4_t first[MAXSTUDIOBONES];
	matrix3x4_t second[MAXSTUDIOBONES];
	matrix3x4_t positive[MAXSTUDIOBONES];
	matrix3x4_t negative[MAXSTUDIOBONES];
	matrix3x4_t low_first[MAXSTUDIOBONES];
	matrix3x4_t low_second[MAXSTUDIOBONES];
	matrix3x4_t low_first_20[MAXSTUDIOBONES];
	matrix3x4_t low_second_20[MAXSTUDIOBONES];
};

class adjust_data;

class resolver : public singleton <resolver>
{
	player_t* player = nullptr;
	adjust_data* player_record = nullptr;

	bool side = false;
	bool fake = false;

	bool was_first_bruteforce = false;
	bool was_second_bruteforce = false;

	bool is_player_zero = false;
	bool is_player_faking = false;

	struct
	{
		int missed_shots_corrected[65] = { 0 };;
	}restype[8];

	int positives = 0;
	int negatives = 0;

	bool freestand_side = false;
	bool jitter_side = false;


	bool was_roll_first_bruteforce = false;
	bool was_roll_second_bruteforce = false;

	float lock_side = 0.0f;
	float original_goal_feet_yaw = 0.0f;
	float original_pitch = 0.0f;

	std::array < std::array < Vector, MAXSTUDIOBONES >, 65 > m_BoneOrigins;
	std::array < std::array < matrix3x4_t, MAXSTUDIOBONES >, 65 > m_CachedMatrix = { };

	struct { float move_lby[65]; }records;
public:
	AnimationLayer previous_layers[13];
	AnimationLayer left_resolver_layers[13];
	AnimationLayer center_resolver_layers[13];
	AnimationLayer right_resolver_layers[13];
	AnimationLayer low_left_resolver_layers[13];
	AnimationLayer low_right_resolver_layers[13];
	AnimationLayer resolver_layers[3][13];

	bool IsAdjustingBalance();

	bool is_breaking_lby(AnimationLayer cur_layer, AnimationLayer prev_layer);

	//void get_side_moving();

	//void check_low();

	void layer_test();

	//void layer_detection();

	//bool low_delta();

	void get_side_standing();
	void detect_side();
	void get_side_trace();
	int GetChokedPackets();
	bool DesyncDetect();
	bool update_walk_data();
	void setmode();
	bool MatchShot();
	bool is_slow_walking();


	void final_detection();
	void missed_shots_correction(adjust_data* record, int missed_shots);
	//void layer_ot4();
	//float build_yaw(player_t* player, float angle, int n);
	float b_yaw(player_t* player, float angle, int n);
	//float get_velocity(player_t* player);
	//bool hrt_res();
	//void resolve();
	//void laith(float p, float n);
	void resolve_desync();
	//bool hurt_resolver(shot_info* shot);

	//bool IsFakewalking(player_t* entity);
	void initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch);
	void initialize_yaw(player_t* e, adjust_data* record);
	//float b_yaw(player_t* player, float angle, int n);
	bool does_have_jitter(player_t* player, int* new_side);
	resolver_side FreeStand(player_t* e);
	float get_angle(player_t* player);
	float get_backward_yaw(player_t* player);
	float get_forward_yaw(player_t* player);
	void reset();
	bool desync_detect();
	void antifreestand();
	void resolve_yaw();
	float resolve_pitch();
	bool shot_resolver();
	bool Saw(player_t* entity);
	resolver_side TraceSide(player_t* entity);
	bool safe_matrix_shot = false;

	//AnimationLayer resolver_layers[7][13];
	//AnimationLayer previous_layers[13];
	float resolver_goal_feet_yaw[7];



	resolver_side last_side = RESOLVER_ORIGINAL;
	resolver_type type = ORIGINAL;
};

class adjust_data //-V730
{
public:
	player_t* player;
	int i;

	resolver_type type;
	resolver_side side;
	float desync_amount;
	bool break_lagcomp;
	get_side_move curSide;
	modes curMode;
	AnimationLayer resolver_layers[3][13];
	AnimationLayer previous_layers[13];

	AnimationLayer layers[15];
	AnimationLayer left_layers[15];
	AnimationLayer right_layers[15];
	AnimationLayer center_layers[15];
	matrixes matrixes_data;
	//get_side_move curSide;
	//modes curMode;

	//resolver_type type;
	//resolver_side side;

	bool invalid;
	bool immune;
	bool dormant;
	bool bot;
	bool shot;

	int flags;
	int bone_count;

	float last_shot_time;
	bool check_hurt;

	float lft;
	float rght;
	float left;
	float right;
	float middle;

	float simulation_time;
	float duck_amount;
	float lby;

	bool flipped_s = false;
	bool low_delta_s = false;

	Vector angles;
	Vector abs_angles;
	Vector velocity;
	Vector origin;
	Vector mins;
	Vector maxs;

	matrix3x4_t leftmatrixes[128] = {};
	matrix3x4_t rightmatrixes[128] = {};

	std::array<float, 24> left_poses = {};
	std::array<float, 24> right_poses = {};

	adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		player = nullptr;
		i = -1;

		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;
		curSide = no_sie;
		curMode = NO_MODE;
		bot = false;
		invalid = false;
		immune = false;
		dormant = false;

		shot = false;
		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		lby = 0.0f;

		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	adjust_data(player_t* e, bool store = true)
	{
		type = ORIGINAL;
		side = RESOLVER_ORIGINAL;


		invalid = false;
		store_data(e, store);
	}

	void store_data(player_t* e, bool store = true)
	{
		if (!e->is_alive())
			return;

		player = e;
		i = player->EntIndex();

		if (store)
		{
			memcpy(layers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
			memcpy(matrixes_data.main, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
		}

		immune = player->m_bGunGameImmunity() || player->m_fFlags() & FL_FROZEN;
		dormant = player->IsDormant();

#if RELEASE
		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		bot = player_info.fakeplayer;
#else
		bot = false;
#endif

		flags = player->m_fFlags();
		bone_count = player->m_CachedBoneData().Count();

		simulation_time = player->m_flSimulationTime();
		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->m_vecOrigin();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void adjust_player()
	{
		if (!valid(false))
			return;

		memcpy(player->get_animlayers(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->m_CachedBoneData().Base(), matrixes_data.main, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807

		player->m_fFlags() = flags;
		player->m_CachedBoneData().m_Size = bone_count;

		player->m_flSimulationTime() = simulation_time;
		player->m_flDuckAmount() = duck_amount;
		player->m_flLowerBodyYawTarget() = lby;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecVelocity() = velocity;
		player->m_vecOrigin() = origin;
		player->set_abs_origin(origin);
		player->GetCollideable()->OBBMins() = mins;
		player->GetCollideable()->OBBMaxs() = maxs;
	}

	bool valid(bool extra_checks = true)
	{
		if (!this) //-V704
			return false;

		if (i > 0)
			player = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!player)
			return false;

		if (player->m_lifeState() != LIFE_ALIVE)
			return false;

		if (immune)
			return false;

		if (dormant)
			return false;

		if (!extra_checks)
			return true;

		if (invalid)
			return false;

		auto net_channel_info = m_engine()->GetNetChannelInfo();

		if (!net_channel_info)
			return false;

		static auto sv_maxunlag = m_cvar()->FindVar(crypt_str("sv_maxunlag"));

		auto outgoing = net_channel_info->GetLatency(FLOW_OUTGOING);
		auto incoming = net_channel_info->GetLatency(FLOW_INCOMING);

		auto correct = math::clamp(outgoing + incoming + util::get_interpolation(), 0.0f, sv_maxunlag->GetFloat());

		auto curtime = g_ctx.local()->is_alive() ? TICKS_TO_TIME(g_ctx.globals.fixed_tickbase) : m_globals()->m_curtime; //-V807
		auto delta_time = correct - (curtime - simulation_time);

		if (fabs(delta_time) > 0.2f)
			return false;

		auto extra_choke = 0;

		if (g_ctx.globals.fakeducking)
			extra_choke = 14 - m_clientstate()->iChokedCommands;

		auto server_tickcount = extra_choke + m_globals()->m_tickcount + TIME_TO_TICKS(outgoing + incoming);
		auto dead_time = (int)(TICKS_TO_TIME(server_tickcount) - sv_maxunlag->GetFloat());

		if (simulation_time < (float)dead_time)
			return false;

		return true;
	}
};

class optimized_adjust_data
{
public:
	int i;
	player_t* player;

	float simulation_time;
	float duck_amount;
	float speed;

	bool shot;

	Vector angles;
	Vector origin;

	optimized_adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		i = 0;
		player = nullptr;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		speed = 0.0f;

		shot = false;

		angles.Zero();
		origin.Zero();
	}
};

struct ResolverSide
{
	float yaw_offset;
	float pitch_offset;
};

struct player_settings
{
	__int64 id;
	resolver_history res_type;
	bool faking;
	int neg;
	int pos;

	player_settings(__int64 id, resolver_history res_type, bool faking, int left, int right) noexcept : id(id), res_type(res_type), faking(faking), neg(neg), pos(pos) { }
};

extern std::deque <adjust_data> player_records[65];

class lagcompensation : public singleton <lagcompensation>
{
public:
	void fsn(ClientFrameStage_t stage);
	bool valid(int i, player_t* e);
	void update_player_animations(player_t* e);
	void FixPvs();
	void apply_interpolation_flags(player_t* e);
	float GetAngle(player_t* player);
	void init();

	std::vector<player_settings> player_sets;

	resolver player_resolver[65];

	bool is_dormant[65];
	float previous_goal_feet_yaw[65];
};