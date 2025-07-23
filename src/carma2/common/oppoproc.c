#include "oppoproc.h"

#include "loading.h"
#include "opponent.h"
#include "platform.h"

#include "rec2_macros.h"

void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {
}
C2_HOOK_FUNCTION(0x004b13c0, DoNotDprintf_oppoproc)

br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2) {

    return pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
}

tFollow_path_result FollowCheatyPath(tOpponent_spec* pOpponent_spec) {
    tFollow_path_data* data;
    br_vector3 a;
    br_vector3 p;
    br_vector3 section_v;
    br_vector3 car_to_end;
    br_vector3 car_to_intersect;
    const br_vector3* start;
    const br_vector3* finish;
    br_scalar t;
    br_scalar frame_period_in_secs;
    br_scalar distance_to_end;
    br_scalar distance_to_intersect;
    br_scalar desired_speed_BRU;

    data = &pOpponent_spec->follow_path_data;
    start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
    finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
    if (pOpponent_spec->follow_path_data.cheating != pOpponent_spec->cheating) {
        data->cheating = pOpponent_spec->cheating;
        if (data->cheating) {
            DoNotDprintf_oppoproc("%s: Dematerialising", pOpponent_spec->car_spec->driver_name);
            BrVector3Sub(&section_v, finish, start);
            BrVector3Sub(&p, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, start);
            t = BrVector3Dot(&section_v, &p) / BrVector3LengthSquared(&section_v);
            if (t < 0.0f) {
                BrVector3Copy(&data->cheaty_intersect, start);
            } else if (t > 1.f) {
                BrVector3Copy(&data->cheaty_intersect, finish);
            } else {
                BrVector3Scale(&data->cheaty_intersect, &section_v, t);
                BrVector3Accumulate(&data->cheaty_intersect, start);
            }
            BrVector3Sub(&car_to_intersect, &data->cheaty_intersect,
                &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            distance_to_intersect = BrVector3Length(&car_to_intersect);
            frame_period_in_secs = C2V(gFrame_period_for_this_munging) / 1000.0f * 6.0f;
            TurnOpponentPhysicsOff(pOpponent_spec);
            if (distance_to_intersect >= frame_period_in_secs) {
                data->moving_to_intersect = 1;
                BrVector3Normalise(&car_to_intersect, &car_to_intersect);
                BrVector3Scale(&car_to_intersect, &car_to_intersect, frame_period_in_secs);
                BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_intersect);
                return eFPR_OK;
            } else {
                data->moving_to_intersect = 0;
                BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
                return eFPR_OK;
            }
        }
        BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_end = BrVector3Length(&car_to_end);
        DoNotDprintf_oppoproc("%s: Rematerialising from FollowCheatyPath()...", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->follow_path_data.section_no < 0) {
            PDFatalError("No useful section number in ProcessCheatyPath()");
        }
        if (GetOpponentsSectionMaxSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) < 255) {
            desired_speed_BRU = GetOpponentsSectionMaxSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) / WORLD_SCALE;
        } else if (GetOpponentsSectionMinSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) > 0) {
            desired_speed_BRU = GetOpponentsSectionMinSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) / WORLD_SCALE;
        } else {
            desired_speed_BRU = MIN(7.0f, MAX(1.0f, 2.f * distance_to_end));
        }

        if (RematerialiseOpponentOnThisSection(pOpponent_spec, data->section_no, desired_speed_BRU)) {
            pOpponent_spec->car_spec->brake_force = 0.f;
            pOpponent_spec->car_spec->acc_force = 0.f;
            return eFPR_OK;
        }
        data->made_it = 1;
    }
    frame_period_in_secs = C2V(gFrame_period_for_this_munging) / 1000.0f * 6.0f;
    BrVector3Set(&pOpponent_spec->car_spec->collision_info->v, 0.f, 41.4f, 0.f);

    if (data->moving_to_intersect) {
        BrVector3Sub(&car_to_intersect, &data->cheaty_intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_intersect = BrVector3Length(&car_to_intersect);
        if (distance_to_intersect < frame_period_in_secs) {
            data->moving_to_intersect = 0;
            BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
            BrVector3Sub(&car_to_end, finish, start);
        } else {
            BrVector3Normalise(&car_to_intersect, &car_to_intersect);
            BrVector3Scale(&car_to_intersect, &car_to_intersect, frame_period_in_secs);
            BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_intersect);
        }
        PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
    } else {
        while (frame_period_in_secs > 0.0f) {
            BrVector3Sub(&a, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            distance_to_end = BrVector3Length(&a);
            if (distance_to_end < frame_period_in_secs) {
                BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, finish);
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - current section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                data->section_no = GetOpponentsNextSection(pOpponent_spec, data->section_no);
                if (data->section_no == -1) {
                    pOpponent_spec->car_spec->acc_force = 0.f;
                    pOpponent_spec->car_spec->brake_force = 0.f;
                    DoNotDprintf_oppoproc("%s: ProcessFollowPath() - reached end of path while cheating", pOpponent_spec->car_spec->driver_name);
                    return eFPR_end_of_path;
                }
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - next section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
                finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
                frame_period_in_secs -= distance_to_end;
            } else {
                BrVector3Scale(&p, &p, frame_period_in_secs);
                BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &p);
                frame_period_in_secs = 0.0;
            }
        }
        BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
    }
    return eFPR_OK;
}

int (C2_HOOK_FASTCALL * GetStraight_original)(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec);
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABLED)
    return GetStraight_original(pStart, pFinish, pWidth, pSection, pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b13d0, GetStraight, GetStraight_original)

int (C2_HOOK_FASTCALL * CalcCorners_original)(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABLED)
    return CalcCorners_original(pCorners, pSection, pWidth, pStart, pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b1560, CalcCorners, CalcCorners_original)

int (C2_HOOK_FASTCALL * CalcSOCs_original)(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs);
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs) {

#if defined(C2_HOOKS_ENABLED)
    return CalcSOCs_original(pNext_section, pCount_corners, pCorners, pOpponent_spec, pSocs);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b1ab0, CalcSOCs, CalcSOCs_original)
