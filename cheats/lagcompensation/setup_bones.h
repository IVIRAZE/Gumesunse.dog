#pragma once
#include "..\..\includes.hpp"

class CSetupBones
{
public:
    void setup();
    void setup_bones_server();
    void get_skeleton();
    void studio_build_matrices(CStudioHdr* hdr, const matrix3x4_t& worldTransform, Vector* pos, Quaternion* q, int boneMask, matrix3x4_t* out, uint32_t* boneComputed);
    void attachment_helper();
    void fix_bones_rotations();

    matrix3x4_t* m_boneMatrix = nullptr;
    Vector m_vecOrigin = ZERO;
    Vector m_angAngles = ZERO;
    CStudioHdr* m_pHdr = nullptr;
    Vector* m_vecBones = nullptr;
    Quaternion* m_quatBones = nullptr;
    bool m_bShouldDoIK = false;
    bool m_bShouldAttachment = true;
    bool m_bShouldDispatch = true;
    int m_boneMask = 0;
    float m_flPoseParameters[24];
    float m_flWorldPoses[24];
    int m_nAnimOverlayCount = 0;
    AnimationLayer* m_animLayers = nullptr;
    float m_flCurtime = 0.0f;
    player_t* m_animating = nullptr;
};

/*
bool setup_bones( player_t* player, matrix3x4_t* bone, int max_bones, int mask ) {
    const int backup_ik *( int* )( uintptr_t( player ) + ( offset_force_bone - 0x1c ) );
    const int backup_client_effects = *( int* )( uintptr_t( player ) + 0x68 );
    const int backup_effects = player->effects( ); // ( "CBaseEntity", "m_fEffects" )
    const int backup_last_non_skipped_frame_cnt = *( int* )( uintptr_t( player ) + 0xa68 );
    const int backup_anim_lod_flags = *( int* )( uintptr_t( player ) + 0xa28 );
    const vec3_t backup_abs_origin = player->abs_origin( ); // 10 index from c_base_ent virtual table..

    // jiggle fix but if u hookin build transformation and set 0x292c( jiggle bones ) to false then delete that shit
    // r_jiggle_bones -> cvar->find_var( "r_jiggle_bones" );
    const int backup_jiggle_bones = r_jiggle_bones->get_int( );
    r_jiggle_bones->set_value( 0 );

    *( int* )( uintptr_t( player ) + ( offset_force_bone - 0x1c ) ) = 0;
    player->invalidate_bone_cache( );
    *( int* )( uintptr_t( player ) + 0xa68 ) = 0;
    *( int* )( uintptr_t( player ) + 0xa28 ) &= ~2;
    *( int* )( uintptr_t( player ) + 0xA24 ) = -1;
    *( int* )( uintptr_t( player ) + 0x68 ) |= 2;
    player->effects( ) |= 8;

    // fix model sway...
    int v105 = -1;
    if ( player->anim_layers_count( ) >= 12 ) {
        v105 = player->anim_layers( )[ 12 ].m_weight;
         player->anim_layers( )[ 12 ].m_weight = 0;
    }

    // set un-interpolated origin.
    if ( player != local_player )
        player->set_abs_origin( player->origin( ) );

    // set time too.
    float time = 0;
    if ( player != local_player )
        time = player->sim_time( );
    else
        time = ticks_to_time( client_state->server_tick ) );

    // es0 shizo.
    const int backup_frame_cnt = g_global_vars->m_frame_cnt;
    g_global_vars->m_frame_cnt = -999; // bypass should_skip_anim_frame.

    float v107 = ( float* )( uintptr_t( player ) + ( 0x3ad0 + 0x4 ) );
    float v108 = ( float* )( uintptr_t( player ) + ( 0x6f20 + 0x4 ) );

    float backup_unk = *v107;
    float backup_unk2 = *v108;

    *v107 = 0.0;
    *v108 = 0.0;

    // idc so mb use that for pvs fix or something else.
    // build bones.
    can_build = true;
    bool premium_geng = player->renderable( )->setup_bones( bone, max_bones, mask, time );
    can_build = false;

    // restore frame cnt.
    g_global_vars->m_frame_cnt = backup_frame_cnt;

    // restore unk funcs.
    *v107 = backup_unk;
    *v108 = backup_unk2;

    // restore prev.
    if ( player->anim_layers_count( ) >= 12 && v105 >= 0 )
        player->anim_layers( )[ 12 ].m_weight = v105;

    // ..........
    *( int* )( uintptr_t( player ) + ( offset_force_bone - 0x1c ) ) = backup_ik;
    player->effects( ) = backup_effects;
    *( int* )( uintptr_t( player ) + 0x68 ) = backup_client_effects;
    *( int* )( uintptr_t( player ) + 0xa28 ) = backup_anim_lod_flags;

    r_jiggle_bones->set_value( backup_jiggle_bones );

    // restore interpolated origin.
    if ( player != local_player )
        player->set_abs_origin( backup_abs_origin );

    return premium_geng;
}
*/