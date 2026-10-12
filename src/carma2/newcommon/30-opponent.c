#include "30-opponent.h"

#include "00-car.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include <c2_hooks.h>
#include <string.h>
#include <math.h>

// GLOBAL: CARMA2_HW 0x0069171c
float gShortest_route_length;

// GLOBAL: CARMA2_HW 0x0069172c
int gRoute_search_target_count;

// GLOBAL: CARMA2_HW 0x0069174c
int gRoute_search_last_target_count;

// GLOBAL: CARMA2_HW 0x0069175c
tU8* gBit_per_node;

// GLOBAL: CARMA2_HW 0x0069176c
int gRoute_search_calls;

// GLOBAL: CARMA2_HW 0x00691770
int gRoute_search_attempts;

// GLOBAL: CARMA2_HW 0x00691774
int gRoute_search_total_calls;

// GLOBAL: CARMA2_HW 0x00691778
int gRoute_search_max_calls;

/* PC53 operations retain the original x87 rounding and NaN operand selection.
 * In particular, a scalar store need not discard its unrounded register value. */
static double C2_HOOK_CDECL OpponentFloatDifference(const float* pA, const float* pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("flds %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(*pA), "m"(*pB) : "st");
#else
    result = (double)*pA - (double)*pB;
#endif
    return result;
}

static double C2_HOOK_CDECL OpponentFloatAdd(const float* pA, const float* pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fadd dword ptr [edx]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("flds %1; fadds %2; fstpl %0" : "=m"(result) : "m"(*pA), "m"(*pB) : "st");
#else
    result = (double)*pA + (double)*pB;
#endif
    return result;
}

static double C2_HOOK_CDECL OpponentAdd(double pA, double pB) {
    volatile double result = pA + pB;
    return result;
}

static double C2_HOOK_CDECL OpponentSubtract(double pA, double pB) {
    volatile double result = pA - pB;
    return result;
}

static double C2_HOOK_CDECL OpponentMultiply(double pA, double pB) {
    volatile double result = pA * pB;
    return result;
}

static double C2_HOOK_CDECL OpponentDivide(double pA, double pB) {
    volatile double result = pA / pB;
    return result;
}

static double C2_HOOK_CDECL OpponentSqrt(double pA) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fsqrt
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fsqrt; fstpl %0" : "=m"(result) : "m"(pA) : "st");
#else
    result = sqrt(pA);
#endif
    return result;
}

static float C2_HOOK_CDECL OpponentScalarRound(double pA) {
    volatile union { float scalar; tU32 bits; } stored;
    union { float scalar; tU32 bits; } result;

    stored.scalar = (float)pA;
    result.bits = stored.bits;
    return result.scalar;
}

static double C2_HOOK_CDECL OpponentHybridLengthSquared(double pX, double pY, double pZ) {
    float x = OpponentScalarRound(pX);
    float y = OpponentScalarRound(pY);
    return OpponentAdd(OpponentAdd(OpponentMultiply(pX, x), OpponentMultiply(pY, y)), OpponentMultiply(pZ, pZ));
}

// GLOBAL: CARMA2_HW 0x00691744
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x0065a3cc
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
int gMaxTimeOpponentRepair;

// FUNCTION: CARMA2_HW 0x004a8160
void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...) {
    /* The original diagnostic function is a single RET. */
    (void)pMessage;
}

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// PointVisibleFromHere

// WeightedFindNearestNodeAndSection

// FUNCTION: CARMA2_HW 0x004a82d0
tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {
    int i;
    int count;
    int nearest_section = -1;
    int nearest_node = -1;
    int nearest_node_section = -1;
    float closest = 3.4028234663852886e38f;
    float nearest_distance = 3.4028234663852886e38f;
    /* The original leaves this slot indeterminate when no segment qualifies. */
    volatile float nearest_t;
    tPath_section* sections = gProgram_state.AI_vehicles.path_sections;
    br_vector3 a;
    br_vector3 p;
    const br_vector3* start;
    const br_vector3* finish;
    double px;
    double py;
    double pz;
    double x;
    double y;
    double z;
    double d;
    double length;
    float t;
    double inv;

    count = pCar != NULL ? pCar->my_trail.number_of_nodes - 1 : gProgram_state.AI_vehicles.number_of_path_sections;
    for (i = 0; i < count; i++) {
        if (pCar != NULL) {
            start = &pCar->my_trail.trail_nodes[i];
            finish = &pCar->my_trail.trail_nodes[i + 1];
        } else {
            start = &gProgram_state.AI_vehicles.path_nodes[(tS16)sections[i].node_indices[0]].pos;
            finish = &gProgram_state.AI_vehicles.path_nodes[(tS16)sections[i].node_indices[1]].pos;
        }
        px = OpponentFloatDifference(&pActor_coords->v[0], &start->v[0]);
        py = OpponentFloatDifference(&pActor_coords->v[1], &start->v[1]);
        py = OpponentAdd(py, py);
        pz = OpponentFloatDifference(&pActor_coords->v[2], &start->v[2]);
        a.v[0] = OpponentScalarRound(OpponentFloatDifference(&finish->v[0], &start->v[0]));
        y = OpponentFloatDifference(&finish->v[1], &start->v[1]);
        a.v[1] = OpponentScalarRound(OpponentAdd(y, y));
        a.v[2] = OpponentScalarRound(OpponentFloatDifference(&finish->v[2], &start->v[2]));
        p.v[0] = OpponentScalarRound(px);
        p.v[1] = OpponentScalarRound(py);
        p.v[2] = OpponentScalarRound(pz);
        x = OpponentSubtract(px, a.v[0]);
        y = OpponentSubtract(py, a.v[1]);
        z = OpponentSubtract(pz, a.v[2]);
        d = OpponentHybridLengthSquared(x, y, z);
        if (d < closest || d != d || closest != closest) {
            closest = OpponentScalarRound(d);
            nearest_node = pCar != NULL ? i : (tS16)sections[i].node_indices[1];
            nearest_node_section = i;
        }
        d = OpponentAdd(OpponentAdd(OpponentMultiply(p.v[0], p.v[0]), OpponentMultiply(p.v[1], p.v[1])), OpponentMultiply(p.v[2], p.v[2]));
        if (d < closest || d != d || closest != closest) {
            closest = OpponentScalarRound(d);
            nearest_node = pCar != NULL ? i + 1 : (tS16)sections[i].node_indices[0];
            nearest_node_section = i;
        }
        length = OpponentAdd(OpponentAdd(OpponentMultiply(a.v[0], a.v[0]), OpponentMultiply(a.v[1], a.v[1])), OpponentMultiply(a.v[2], a.v[2]));
        if (length >= 0.0001f) {
            d = OpponentAdd(OpponentAdd(OpponentMultiply(p.v[0], a.v[0]), OpponentMultiply(p.v[1], a.v[1])), OpponentMultiply(p.v[2], a.v[2]));
            t = OpponentScalarRound(OpponentDivide(d, length));
            if (t >= 0.f && t <= 1.f) {
                x = OpponentSubtract(p.v[0], OpponentMultiply(a.v[0], t));
                y = OpponentSubtract(p.v[1], OpponentMultiply(a.v[1], t));
                z = OpponentSubtract(p.v[2], OpponentMultiply(a.v[2], t));
                d = OpponentHybridLengthSquared(x, y, z);
                if (d < nearest_distance || d != d || nearest_distance != nearest_distance) {
                    nearest_section = i;
                    nearest_distance = OpponentScalarRound(d);
                    nearest_t = t;
                }
            }
        }
    }
    if (closest < nearest_distance || closest != closest || nearest_distance != nearest_distance) {
        nearest_section = nearest_node_section;
    } else {
        nearest_node = -1;
    }
    if (pCar != NULL) {
        start = &pCar->my_trail.trail_nodes[nearest_section];
        finish = &pCar->my_trail.trail_nodes[nearest_section + 1];
    } else {
        start = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_section].node_indices[0]].pos;
        finish = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_section].node_indices[1]].pos;
    }
    x = OpponentFloatDifference(&finish->v[0], &start->v[0]);
    y = OpponentFloatDifference(&finish->v[1], &start->v[1]);
    z = OpponentFloatDifference(&finish->v[2], &start->v[2]);
    if (nearest_node >= 0) {
        for (i = 0; i < 3; i++) {
            start = pCar != NULL ? &pCar->my_trail.trail_nodes[nearest_node] : &gProgram_state.AI_vehicles.path_nodes[nearest_node].pos;
            memcpy(&pIntersect->v[i], &start->v[i], sizeof(float));
        }
    } else {
        px = OpponentMultiply(x, nearest_t);
        py = OpponentMultiply(y, nearest_t);
        pz = OpponentMultiply(z, nearest_t);
        pIntersect->v[0] = OpponentScalarRound(px);
        pIntersect->v[1] = OpponentScalarRound(py);
        pIntersect->v[2] = OpponentScalarRound(pz);
        /* Even the trail case uses global path nodes in this original branch. */
        start = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_section].node_indices[0]].pos;
        pIntersect->v[0] = OpponentScalarRound(OpponentAdd(start->v[0], px));
        start = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_section].node_indices[0]].pos;
        pIntersect->v[1] = OpponentScalarRound(OpponentAdd(start->v[1], OpponentScalarRound(py)));
        start = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_section].node_indices[0]].pos;
        pIntersect->v[2] = OpponentScalarRound(OpponentAdd(start->v[2], OpponentScalarRound(pz)));
    }
    length = OpponentAdd(OpponentAdd(OpponentMultiply(x, x), OpponentMultiply(y, y)), OpponentMultiply(z, z));
    inv = OpponentDivide(1.0, OpponentSqrt(length));
    pPath_direction->v[0] = OpponentScalarRound(OpponentMultiply(inv, x));
    pPath_direction->v[1] = OpponentScalarRound(OpponentMultiply(inv, y));
    pPath_direction->v[2] = OpponentScalarRound(OpponentMultiply(inv, z));
    px = OpponentFloatDifference(&pActor_coords->v[0], &pIntersect->v[0]);
    py = OpponentFloatDifference(&pActor_coords->v[1], &pIntersect->v[1]);
    pz = OpponentFloatDifference(&pActor_coords->v[2], &pIntersect->v[2]);
    *pDistance = OpponentScalarRound(OpponentSqrt(OpponentHybridLengthSquared(px, py, pz)));
    return (tS16)(nearest_section + (pCar != NULL ? 15000 : 0));
}

// FindNearestPathSection

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// NewObjective

// FUNCTION: CARMA2_HW 0x004aae20
void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec* pOpponent_spec) {
    tS16 candidates[8];
    tS16 node;
    tS16 section;
    tS16 race_count;
    tS16 forward_count;
    tS16 reverse_count;
    tS16 i;
    int index;
    br_vector3 direction;
    br_vector3 intersect;
    br_scalar distance;

    if (pOpponent_spec->nnext_sections >= 12) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Pissing off 'cos projected route full up", pOpponent_spec->car_spec->driver_name);
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Projected route empty; starting from nearest section", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->complete_race_data.finished_calcing_race_route = 0;
        pOpponent_spec->complete_race_data.found_race_section = 0;
        section = FindNearestGeneralSection(NULL, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
        if (section < 0) {
            return;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 1);
        if (gProgram_state.AI_vehicles.path_sections[section].type == ePST_race_path) {
            pOpponent_spec->complete_race_data.found_race_section = 1;
        }
    }
    while (pOpponent_spec->nnext_sections < 12 && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
        const tRoute_section* last = &pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1];
        node = (tS16)gProgram_state.AI_vehicles.path_sections[last->section_no].node_indices[last->direction];
        race_count = forward_count = reverse_count = 0;
        for (i = 0; i < gProgram_state.AI_vehicles.path_nodes[node].number_of_sections; i++) {
            section = (tS16)gProgram_state.AI_vehicles.path_nodes[node].sections[i];
            if (section != pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no) {
                const tPath_section* candidate = &gProgram_state.AI_vehicles.path_sections[section];
                if (candidate->type == ePST_race_path && (tS16)candidate->node_indices[0] == node) {
                    pOpponent_spec->complete_race_data.found_race_section = 1;
                    candidates[race_count++] = section;
                } else if (race_count == 0 && (tS16)candidate->node_indices[0] == node) {
                    candidates[forward_count++] = section;
                } else if (race_count == 0 && forward_count == 0 && (!candidate->one_way || (tS16)candidate->node_indices[1] != node)) {
                    candidates[reverse_count++] = section;
                }
            }
        }
        if (race_count != 0 || forward_count != 0 || reverse_count != 0) {
            index = IRandomBetween(0, (race_count != 0 ? race_count : forward_count != 0 ? forward_count : reverse_count) - 1);
            AddToOpponentsProjectedRoute(pOpponent_spec, candidates[index], race_count != 0 || forward_count != 0);
        } else if (!pOpponent_spec->complete_race_data.found_race_section) {
            section = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
            index = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0;
            AddToOpponentsProjectedRoute(pOpponent_spec, section, index);
        } else {
            pOpponent_spec->complete_race_data.finished_calcing_race_route = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a93c0
void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add) {
    int target;
    int i;
    int count;
    tS16 candidates[8];
    tS16 section;
    tS16 node;
    tS16 previous;
    int direction;
    tPath_section* sections;
    const tPath_node* path_node;

    if (pSections_to_add == 0) {
        PDEnterDebugger("TopUpRandomRoute() called with no seed (woof, bark, etc.)");
    }
    target = (int)((unsigned int)pOpponent_spec->nnext_sections + (unsigned int)pSections_to_add);
    if (pSections_to_add < 0 || target >= 12) {
        target = 12;
    }
    sections = gProgram_state.AI_vehicles.path_sections;
    while (pOpponent_spec->nnext_sections < target) {
        previous = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
        direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
        node = (tS16)sections[previous].node_indices[direction];
        path_node = &gProgram_state.AI_vehicles.path_nodes[node];
        count = 0;
        if (path_node->number_of_sections > 1) {
            for (i = 0; i < path_node->number_of_sections; i++) {
                section = (tS16)path_node->sections[i];
                if (section != previous
                        && (!sections[section].one_way || (tS16)sections[section].node_indices[1] != node)
                        && (pOpponent_spec->cheating || sections[section].type != ePST_cheat_only)) {
                    candidates[count++] = section;
                }
            }
        }
        section = previous;
        direction = direction == 0;
        if (count > 1) {
            section = candidates[IRandomBetween(0, count - 1)];
            sections = gProgram_state.AI_vehicles.path_sections;
            direction = (tS16)sections[section].node_indices[1] != node;
        } else if (count == 1) {
            section = candidates[0];
            direction = (tS16)sections[section].node_indices[1] != node;
        }
        if (AddToOpponentsProjectedRoute(pOpponent_spec, section, direction)) {
            sections = gProgram_state.AI_vehicles.path_sections;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ac180
int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pRoute, tRoute_section* pBest_route, int* pBest_count, tS16 pTarget, int pDepth, br_scalar pLength, tOpponent_spec* pOpponent_spec) {
    char prefix[32];
    tS16 node;
    tS16 next_node;
    tS16 section;
    tS16 i;
    int direction;
    int byte_index;
    tU8 bit;
    int next_byte;
    tU8 next_bit;
    float length;
    const tPath_node* path_node;

    DoNotDprintf_opponent("%sSearchForSection() - START: depth %d", prefix, pDepth);
    gRoute_search_calls = (int)((unsigned int)gRoute_search_calls + 1u);
    if (pDepth == 1) {
        int bytes = (int)((unsigned int)gProgram_state.AI_vehicles.number_of_path_nodes + 7u) / 8;

        DoNotDprintf_opponent("%sSearchForSection() - First time called", prefix);
        memset(gBit_per_node, 0, (size_t)bytes);
        gShortest_route_length = 3.4028234663852886e38f;
        gRoute_search_target_count = 0;
        *pBest_count = 0;
    }
    if (pDepth >= 10) {
        DoNotDprintf_opponent("%sSearchForSection() - *** Reached max depth ***", prefix);
        return 0;
    }
    node = (tS16)gProgram_state.AI_vehicles.path_sections[pRoute[pDepth - 1].section_no].node_indices[pRoute[pDepth - 1].direction];
    byte_index = node / 8;
    bit = (tU8)(1u << ((unsigned int)(node % 8) & 31));
    gBit_per_node[byte_index] |= bit;
    path_node = &gProgram_state.AI_vehicles.path_nodes[node];
    DoNotDprintf_opponent("%sSearchForSection() - node #%d has %d sections", prefix, node, path_node->number_of_sections);
    for (i = 0; i < path_node->number_of_sections; i++) {
        const tPath_section* candidate;

        section = (tS16)path_node->sections[i];
        DoNotDprintf_opponent("%sSearchForSection() - node's section #%d is #%d", prefix, i, section);
        candidate = &gProgram_state.AI_vehicles.path_sections[section];
        direction = node != (tS16)candidate->node_indices[1];
        next_node = (tS16)candidate->node_indices[direction];
        next_byte = next_node / 8;
        next_bit = (tU8)(1u << ((unsigned int)(next_node % 8) & 31));
        if ((gBit_per_node[next_byte] & next_bit) != 0) {
            DoNotDprintf_opponent("%sSearchForSection() - *** node #%d already visited ***", prefix, next_node);
            continue;
        }
        if (candidate->one_way && node == (tS16)candidate->node_indices[1]) {
            DoNotDprintf_opponent("%sSearchForSection() - *** no entry ***", prefix);
            continue;
        }
        if (!pOpponent_spec->cheating && candidate->type == ePST_cheat_only) {
            continue;
        }
        DoNotDprintf_opponent("%sSearchForSection() - adding section #%d to list", prefix, section);
        pRoute[pDepth].section_no = section;
        pRoute[pDepth].direction = (tU8)direction;
        length = OpponentScalarRound(OpponentFloatAdd(&gProgram_state.AI_vehicles.path_sections[section].length, &pLength));
        if (section == pTarget && (length < gShortest_route_length || length != length || gShortest_route_length != gShortest_route_length)) {
            gShortest_route_length = length;
            *pBest_count = pDepth + 1;
            /* REP MOVSD includes the untouched padding byte in every entry. */
            for (i = 0; i < pDepth + 1; i++) {
                tU32 word;

                memcpy(&word, &pRoute[i], sizeof(word));
                memcpy(&pBest_route[i], &word, sizeof(word));
            }
            gRoute_search_target_count = (int)((unsigned int)gRoute_search_target_count + 1u);
            gRoute_search_last_target_count = gRoute_search_target_count;
            if (gRoute_search_target_count >= 2) {
                DoNotDprintf_opponent("%sSearchForSection() - Found final target", prefix);
                return 1;
            }
            DoNotDprintf_opponent("%sSearchForSection() - Found a target", prefix);
            DoNotDprintf_opponent("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
            gBit_per_node[byte_index] &= (tU8)~bit;
            return 0;
        }
        if (pDepth < 9 && SearchForSection(pRoute, pBest_route, pBest_count, pTarget, pDepth + 1, length, pOpponent_spec)) {
            return 1;
        }
    }
    gBit_per_node[byte_index] &= (tU8)~bit;
    DoNotDprintf_opponent("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
    return 0;
}

// UpdatePlayersSection

// AllowForDecel

// FUNCTION: CARMA2_HW 0x004ab100
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;
    int count;

    if (pOpponent_spec->nnext_sections <= pPlaces) {
        return 0;
    }
    count = (int)(12u - (unsigned int)pPlaces);
    for (i = 0; i < count; i++) {
        pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
        pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
    }
    pOpponent_spec->nnext_sections = (int)((unsigned int)pOpponent_spec->nnext_sections - (unsigned int)pPlaces);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004ac9e0
void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce) {
    br_vector3 direction;
    br_vector3 intersect;
    br_vector3 player_direction;
    br_vector3 player_intersect;
    br_scalar distance;
    tRoute_section route[10];
    tRoute_section best[10];
    int best_count;
    int i;
    int index;
    int count;
    int failed = 0;
    tS16 section;
    tS16 node;
    tU32 now;
    double x;
    double y;
    double z;
    double stopping;
    double length;
    const br_vector3* point;
    const br_matrix34* transform;
    int add;

    if (pOpponent_spec->nnext_sections >= 12) {
        return;
    }
    now = gTime_stamp_for_this_munging;
    if (pPursuee->field_0x155c < 0 || now > pPursuee->field_0x1540) {
        add = IRandomBetween(-100, 100);
        pPursuee->field_0x1540 = gTime_stamp_for_this_munging + (tU32)add + 1000u;
        pPursuee->field_0x155c = FindNearestGeneralSection(NULL, &pPursuee->car_master_actor->t.t.translate.t, &player_direction, &player_intersect, &distance);
    }
    if (pPursuee->field_0x155c < 0) {
        PDEnterDebugger("No path section near player. THIS CAN'T HAPPEN!");
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        pForce = 1;
    }
    if (pForce || pPursuee->field_0x155c != pOpponent_spec->players_section_when_last_calced_full_path) {
        section = FindNearestGeneralSection(NULL, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
        if (section < 0) {
            PDEnterDebugger("No path section near us. THIS CAN'T HAPPEN!");
            return;
        }
        index = -1;
        for (i = 0; i < pOpponent_spec->nnext_sections; i++) {
            /* The original writes before testing section against zero. */
            pOpponent_spec->next_sections[i].section_no = section;
            if (section != 0) {
                index = i;
                break;
            }
        }
        if (index >= 0) {
            const br_vector3* velocity = &pOpponent_spec->car_spec->collision_info->v;

            length = OpponentAdd(OpponentAdd(OpponentMultiply(velocity->v[0], velocity->v[0]), OpponentMultiply(velocity->v[1], velocity->v[1])), OpponentMultiply(velocity->v[2], velocity->v[2]));
            stopping = OpponentMultiply(OpponentMultiply(OpponentSubtract(100.f, length), -0.0625f), 0.14492753623188406);
            if (stopping > 0.f) {
                /* This first lookup uses the route ordinal, not section_no. */
                node = (tS16)gProgram_state.AI_vehicles.path_sections[index].node_indices[pOpponent_spec->next_sections[index].direction];
                point = &gProgram_state.AI_vehicles.path_nodes[node].pos;
                x = OpponentFloatDifference(&point->v[0], &intersect.v[0]);
                y = OpponentFloatDifference(&point->v[1], &intersect.v[1]);
                z = OpponentFloatDifference(&point->v[2], &intersect.v[2]);
                length = OpponentSqrt(OpponentHybridLengthSquared(x, y, z));
                while (index < pOpponent_spec->nnext_sections - 1 && (length < stopping || length != length || stopping != stopping)) {
                    stopping = OpponentSubtract(stopping, length);
                    index++;
                    length = (double)gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[index].section_no].length;
                }
            }
            ShiftOpponentsProjectedRoute(pOpponent_spec, index);
        } else {
            pOpponent_spec->nnext_sections = 0;
            transform = &pOpponent_spec->car_spec->car_master_actor->t.t.mat;
            x = OpponentFloatDifference(&intersect.v[0], &transform->m[3][0]);
            y = OpponentFloatDifference(&intersect.v[1], &transform->m[3][1]);
            z = OpponentFloatDifference(&intersect.v[2], &transform->m[3][2]);
            length = OpponentAdd(OpponentAdd(OpponentMultiply(z, transform->m[2][2]), OpponentMultiply(x, transform->m[2][0])), OpponentMultiply(y, transform->m[2][1]));
            AddToOpponentsProjectedRoute(pOpponent_spec, section, length >= 0.f);
        }
    }
    while (pOpponent_spec->nnext_sections < 12 && !failed) {
        route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
        route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
        gRoute_search_calls = 0;
        gRoute_search_attempts = (int)((unsigned int)gRoute_search_attempts + 1u);
        SearchForSection(route, best, &best_count, pPursuee->field_0x155c, 1, 0.f, pOpponent_spec);
        gRoute_search_total_calls = (int)((unsigned int)gRoute_search_total_calls + (unsigned int)gRoute_search_calls);
        if (gRoute_search_calls > gRoute_search_max_calls) {
            gRoute_search_max_calls = gRoute_search_calls;
        }
        if (best_count > 1) {
            count = best_count - 1;
            if (count >= 12 - pOpponent_spec->nnext_sections) {
                count = 12 - pOpponent_spec->nnext_sections;
            }
            memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], (size_t)count * sizeof(tRoute_section));
            pOpponent_spec->nnext_sections += count;
            if (pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no == pPursuee->field_0x155c) {
                add = gOpponents[pOpponent_spec->index].strength_rating >= 5 ? 1 : IRandomBetween(1, 2);
                TopUpRandomRoute(pOpponent_spec, add);
            }
        } else {
            failed = 1;
            if (pOpponent_spec->nnext_sections < 12) {
                TopUpRandomRoute(pOpponent_spec, gOpponents[pOpponent_spec->index].strength_rating >= 5 ? 2 : 4);
            }
        }
    }
    pOpponent_spec->players_section_when_last_calced_full_path = pPursuee->field_0x155c;
}

// FUNCTION: CARMA2_HW 0x004abf60
void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec) {
    br_vector3 direction;
    br_vector3 intersect;
    br_scalar distance;
    tRoute_section route[10];
    tRoute_section best[10];
    tS16 section;
    int best_count;
    int count;
    double length;
    double inv;
    double dot;

    pOpponent_spec->nnext_sections = 0;
    section = FindNearestGeneralSection(NULL, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
    length = OpponentSqrt(OpponentAdd(OpponentAdd(OpponentMultiply(direction.v[0], direction.v[0]), OpponentMultiply(direction.v[1], direction.v[1])), OpponentMultiply(direction.v[2], direction.v[2])));
    if (length > 2.384185791015625e-7f) {
        inv = OpponentDivide(1.0, length);
        direction.v[0] = OpponentScalarRound(OpponentMultiply(inv, direction.v[0]));
        direction.v[1] = OpponentScalarRound(OpponentMultiply(inv, direction.v[1]));
        direction.v[2] = OpponentScalarRound(OpponentMultiply(inv, direction.v[2]));
    } else {
        direction.v[0] = 1.f;
        direction.v[1] = 0.f;
        direction.v[2] = 0.f;
    }
    dot = OpponentAdd(OpponentAdd(OpponentMultiply(pOpponent_spec->car_spec->direction.v[0], direction.v[0]), OpponentMultiply(pOpponent_spec->car_spec->direction.v[1], direction.v[1])), OpponentMultiply(pOpponent_spec->car_spec->direction.v[2], direction.v[2]));
    AddToOpponentsProjectedRoute(pOpponent_spec, section, dot > 0.f);
    route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
    route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
    gRoute_search_calls = 0;
    gRoute_search_attempts = (int)((unsigned int)gRoute_search_attempts + 1u);
    SearchForSection(route, best, &best_count, pOpponent_spec->return_to_start_data.section_no, 1, 0.f, pOpponent_spec);
    gRoute_search_total_calls = (int)((unsigned int)gRoute_search_total_calls + (unsigned int)gRoute_search_calls);
    if (gRoute_search_calls > gRoute_search_max_calls) {
        gRoute_search_max_calls = gRoute_search_calls;
    }
    DoNotDprintf_opponent(">>>SearchForSection() - max %d, avg %.1f", gRoute_search_max_calls, OpponentDivide((double)gRoute_search_total_calls, (double)gRoute_search_attempts));
    if (best_count > 1) {
        count = best_count - 1;
        if (count >= 12 - pOpponent_spec->nnext_sections) {
            count = 12 - pOpponent_spec->nnext_sections;
        }
        memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], (size_t)count * sizeof(tRoute_section));
        pOpponent_spec->nnext_sections += count;
        TopUpRandomRoute(pOpponent_spec, 2);
    } else {
        DoNotDprintf_opponent("%s: CalcReturnToStartPointRoute() - SearchForSection() produced bugger all", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->nnext_sections <= 12) {
            TopUpRandomRoute(pOpponent_spec, 14 - pOpponent_spec->nnext_sections);
        }
    }
}

// ClearOpponentsProjectedRoute

// FUNCTION: CARMA2_HW 0x004ab150
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {
    volatile int* count = &pOpponent_spec->nnext_sections;

    if (*count >= 12) {
        return 0;
    }
    /* An unchecked negative slot can alias the count: retain each reload. */
    pOpponent_spec->next_sections[*count].section_no = pSection_no;
    pOpponent_spec->next_sections[*count].direction = (tU8)pDirection;
    *count = (int)((unsigned int)*count + 1u);
    return 1;
}

// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// ProcessWaitForSomeHaplessSod

// ProcessReturnToStart

// ProcessLevitate

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

// RemoveAnythingStillInList

// RebuildActiveCarList

// STUB: CARMA2_HW 0x004a7a60
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {
    NOT_IMPLEMENTED();
}

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// CalcDistanceFromHome

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// LoadCopCars

// LoadInOppoPaths

// DisposeOpponentPaths

// MungeOpponents

// InitOpponents

// DisposeOpponents

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

// FUNCTION: CARMA2_HW 0x004ae790
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_net_player:
        return gNet_mode != eNet_mode_none ? (int)((unsigned int)gNumber_of_net_players - 1u) : 0;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    default:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        return gNet_players[pIndex < gThis_net_player_index ? pIndex : pIndex + 1].car;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return gProgram_state.AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)gActive_non_car_list[pIndex];
    default:
        return NULL;
    }
}

// GetDriverName

// FUNCTION: CARMA2_HW 0x004ae8e0
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    int count;

    if ((pCar_spec->car_ID & 0xff00) == 0x200) {
        count = gProgram_state.AI_vehicles.number_of_opponents;
        for (i = 0; i < count; i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if ((pCar_spec->car_ID & 0xff00) == 0x300) {
        count = gNumber_of_cops_before_faffage;
        for (i = 0; i < count; i++) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae970
tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;
    int count = gProgram_state.AI_vehicles.number_of_opponents;

    for (i = 0; i < count; i++) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae9e0
int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {
    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    }
    return pSection_no >= 15000 ? pSection_no - 15000 : pSection_no;
}

// FUNCTION: CARMA2_HW 0x004aea00
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        }
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// FUNCTION: CARMA2_HW 0x004aea30
int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section) {
    if (pCurrent_section >= 20000) {
        if (pCurrent_section - 19999 >= pOpponent_spec->nnext_sections
                || (!pOpponent_spec->cheating && gProgram_state.AI_vehicles.path_sections[pCurrent_section - 19999].type == ePST_cheat_only)) {
            return -1;
        }
        return pCurrent_section + 1;
    }
    if (pCurrent_section >= 15000) {
        if (pCurrent_section - 15000 < pOpponent_spec->pursue_car_data.pursuee->my_trail.number_of_nodes - 2) {
            return (tS16)(pCurrent_section + 1);
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004aeb90
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        tS16 node = (tS16)gProgram_state.AI_vehicles.path_sections[route->section_no].node_indices[route->direction == 0];
        return &gProgram_state.AI_vehicles.path_nodes[node].pos;
    }
    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 15000];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[0].pos;
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionStartNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNodePoint()");
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004aec40
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        tS16 node = (tS16)gProgram_state.AI_vehicles.path_sections[route->section_no].node_indices[route->direction];
        return &gProgram_state.AI_vehicles.path_nodes[node].pos;
    }
    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 14999];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[1].pos;
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionFinishNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionFinishNodePoint()");
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004aece0
double C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        return (double)gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pSection - 20000].section_no].width;
    }
    if (pSection >= 15000) {
        return 0.5;
    }
    if (pSection == 10000) {
        return (double)pOpponent_spec->pursue_car_data.direct_line_section.width;
    }
    return (double)gProgram_state.AI_vehicles.path_sections[pSection].width;
}

// FUNCTION: CARMA2_HW 0x004aed50
tS16 C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].min_speed[(route->direction ^ pTowards_finish) == 0];
    }
    if (pSection < 15000) {
        if (pSection == 10000) {
            return pOpponent_spec->pursue_car_data.direct_line_section.min_speed[pTowards_finish];
        }
        DoNotDprintf_opponent("WARNING - GetOpponentsSectionMinSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("WARNING - GetOpponentsSectionMinSpeed()");
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004aedf0
tS16 C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].max_speed[(route->direction ^ pTowards_finish) == 0];
    }
    if (pSection < 15000) {
        if (pSection == 10000) {
            return pOpponent_spec->pursue_car_data.direct_line_section.max_speed[pTowards_finish];
        }
        DoNotDprintf_opponent("WARNING - GetOpponentsSectionMaxSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("WARNING - GetOpponentsSectionMaxSpeed()");
    }
    return 255;
}

// InitOpponentPsyche

// ClearTwattageOccurrenceVariables

// TwoCarsHitEachOther

// RecordOpponentTwattageOccurrence

// GetOpponentMood

