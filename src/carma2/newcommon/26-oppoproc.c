#include "26-oppoproc.h"

#include "30-opponent.h"
#include "41-utility.h"
#include "08-loading1.h"

#include <math.h>
#include <string.h>

/* PC53 operations retain the original x87 rounding and NaN operand selection.
 * In particular, a scalar store need not discard its unrounded register value. */
static double C2_HOOK_CDECL NavigationFloatDifference(const float* pA, const float* pB) {
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


static double C2_HOOK_CDECL NavigationAdd(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fadd qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; faddl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA + pB;
#endif
    return result;
}

static double C2_HOOK_CDECL NavigationSubtract(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fsub qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fsubl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA - pB;
#endif
    return result;
}

static double C2_HOOK_CDECL NavigationMultiply(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fmul qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fmull %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA * pB;
#endif
    return result;
}

static double C2_HOOK_CDECL NavigationDivide(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fdiv qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fdivl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA / pB;
#endif
    return result;
}

static double C2_HOOK_CDECL NavigationSqrt(double pA) {
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

static float C2_HOOK_CDECL NavigationScalarRound(double pA) {
    volatile union { float scalar; tU32 bits; } stored;
    union { float scalar; tU32 bits; } result;

    stored.scalar = (float)pA;
    result.bits = stored.bits;
    return result.scalar;
}

/* MSVC5 folds self-comparisons and otherwise accepts unordered greater-than. */
static int C2_HOOK_CDECL NavigationOrderedGreater(double pA, double pB) {
    unsigned short status;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fcomp qword ptr [pB]
        fnstsw status
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fcompl %2; fnstsw %0" : "=m"(status) : "m"(pA), "m"(pB) : "st");
#else
    return pA > pB;
#endif
    return (status & 0x4100) == 0;
}

static int C2_HOOK_CDECL NavigationLessOrUnordered(double pA, double pB) {
    unsigned short status;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fcomp qword ptr [pB]
        fnstsw status
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fcompl %2; fnstsw %0" : "=m"(status) : "m"(pA), "m"(pB) : "st");
#else
    return pA < pB || pA != pA || pB != pB;
#endif
    return (status & 0x100) != 0;
}




// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// FUNCTION: CARMA2_HW 0x004b13d0
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec) {
    int section;
    float width;
    br_vector2 finish;
    double x, y, dx, dy;
    double cross, length;
    float rounded_x;

    memcpy(&pStart->v[0], &GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pSection)->v[2], 4);
    memcpy(&pStart->v[1], &GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pSection)->v[0], 4);
    memcpy(&pFinish->v[0], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection)->v[2], 4);
    memcpy(&pFinish->v[1], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection)->v[0], 4);
    *pWidth = NavigationScalarRound(GetOpponentsSectionWidth(pOpponent_spec, (tS16)pSection));
    section = GetOpponentsNextSection(pOpponent_spec, (tS16)pSection);
    while (section != -1) {
        memcpy(&finish.v[0], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)section)->v[2], 4);
        memcpy(&finish.v[1], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)section)->v[0], 4);
        dx = NavigationFloatDifference(&pFinish->v[0], &pStart->v[0]);
        dy = NavigationFloatDifference(&pFinish->v[1], &pStart->v[1]);
        x = NavigationAdd(NavigationFloatDifference(&finish.v[0], &pFinish->v[0]), dx);
        y = NavigationAdd(NavigationFloatDifference(&finish.v[1], &pFinish->v[1]), dy);
        rounded_x = NavigationScalarRound(x);
        if (NavigationLessOrUnordered(NavigationAdd(NavigationMultiply(x, dx), NavigationMultiply(y, dy)), 0.0)) {
            break;
        }
        length = NavigationSqrt(NavigationAdd(NavigationMultiply(rounded_x, rounded_x), NavigationMultiply(y, y)));
        cross = NavigationAdd(NavigationMultiply(-dy, rounded_x), NavigationMultiply(dx, y));
        cross = fabs(NavigationDivide(cross, length));
        if (!NavigationLessOrUnordered(cross, 0.1f)) {
            break;
        }
        pFinish->v[0] = NavigationScalarRound(finish.v[0]);
        pFinish->v[1] = NavigationScalarRound(finish.v[1]);
        width = NavigationScalarRound(GetOpponentsSectionWidth(pOpponent_spec, (tS16)section));
        if (NavigationOrderedGreater(*pWidth, width)) {
            memcpy(pWidth, &width, 4);
        }
        section = GetOpponentsNextSection(pOpponent_spec, (tS16)section);
    }
    return section;
}

// TimeToStopStruggling

// TimeToBeBrakingWhenStruggling

// FUNCTION: CARMA2_HW 0x004b1560
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec) {
    int count = 0;
    br_vector2 previous;
    br_vector2 start;
    br_vector2 finish;
    float width;
    float ax, ay, bx, by, negative_by;
    float previous_length, next_length;
    float rounded_base;
    double x, y, dot, cross, norm, cosine, sine, ratio, base, extent, radius, previous_length_wide;
    tCorner* corner;

    memcpy(&previous, pStart, sizeof(previous));
    while (pSection != -1 && count < 4) {
        corner = &pCorners[count];
        corner->section = (tU32)pSection;
        pSection = GetStraight(&start, &finish, &width, pSection, pOpponent_spec);
        x = NavigationFloatDifference(&previous.v[0], &start.v[0]);
        y = NavigationFloatDifference(&previous.v[1], &start.v[1]);
        ax = NavigationScalarRound(x);
        ay = NavigationScalarRound(y);
        previous_length_wide = NavigationAdd(NavigationMultiply(x, ax), NavigationMultiply(y, ay));
        previous_length = NavigationScalarRound(previous_length_wide);
        x = NavigationFloatDifference(&finish.v[0], &start.v[0]);
        y = NavigationFloatDifference(&finish.v[1], &start.v[1]);
        bx = NavigationScalarRound(x);
        by = NavigationScalarRound(y);
        next_length = NavigationScalarRound(NavigationAdd(NavigationMultiply(x, bx), NavigationMultiply(y, by)));
        if (previous_length_wide == 0.0 || previous_length_wide != previous_length_wide || next_length == 0.f || next_length != next_length) {
            goto fudge;
        }
        dot = NavigationAdd(NavigationMultiply(bx, ax), NavigationMultiply(by, ay));
        negative_by = NavigationScalarRound(-(double)by);
        cross = NavigationAdd(NavigationMultiply(ax, negative_by), NavigationMultiply(bx, ay));
        norm = NavigationSqrt(NavigationAdd(NavigationMultiply(dot, dot), NavigationMultiply(cross, cross)));
        if (NavigationOrderedGreater(norm, 2.384185791015625e-7f)) {
            norm = NavigationDivide(1.0, norm);
            cosine = NavigationMultiply(norm, dot);
            sine = NavigationMultiply(norm, cross);
        } else {
            cosine = 1.0;
            sine = 0.0;
        }
        if (NavigationOrderedGreater(sine, 0.0)) {
            corner->left_not_right = 1;
        } else {
            corner->left_not_right = 0;
            sine = -sine;
        }
        if (NavigationLessOrUnordered(sine, 0.001f)) {
fudge:
            memcpy(&corner->field_0x00, &start, sizeof(start));
            if (NavigationLessOrUnordered(pWidth, width)) {
                memcpy(&corner->field_0x08, &pWidth, 4);
            } else {
                memcpy(&corner->field_0x08, &width, 4);
            }
            corner->field_0x0c = NavigationScalarRound(NavigationMultiply(corner->field_0x08, 0.7f));
            corner->left_not_right = (tU32)IRandomBetween(0, 1);
        } else {
            base = NavigationDivide(NavigationAdd(NavigationMultiply(cosine, width), pWidth), sine);
            ratio = NavigationDivide(sine, NavigationSubtract(cosine, -1.f));
            rounded_base = NavigationScalarRound(base);
            extent = NavigationMultiply(ratio, width);
            x = NavigationMultiply(ratio, ratio);
            x = NavigationMultiply(x, width);
            x = NavigationSubtract(x, NavigationMultiply(NavigationMultiply(base, ratio), -2.f));
            x = NavigationMultiply(NavigationSubtract(x, width), width);
            extent = NavigationAdd(NavigationAdd(NavigationSqrt(x), extent), rounded_base);
            corner->field_0x0c = NavigationScalarRound(extent);
            x = NavigationMultiply(NavigationMultiply(extent, extent), 9.f);
            if (NavigationOrderedGreater(x, previous_length)) {
                corner->field_0x0c = NavigationScalarRound(NavigationMultiply(NavigationSqrt(previous_length), 0.3333333432674408f));
            }
            x = NavigationMultiply(NavigationMultiply(corner->field_0x0c, corner->field_0x0c), 9.f);
            if (NavigationOrderedGreater(x, next_length)) {
                corner->field_0x0c = NavigationScalarRound(NavigationMultiply(NavigationSqrt(next_length), 0.3333333432674408f));
            }
            radius = NavigationMultiply(ratio, corner->field_0x0c);
            corner->field_0x08 = NavigationScalarRound(radius);
            if (corner->left_not_right != 0) {
                x = NavigationAdd(NavigationMultiply(bx, corner->field_0x0c), NavigationMultiply(radius, negative_by));
                corner->field_0x00.v[0] = NavigationScalarRound(x);
                y = NavigationAdd(NavigationMultiply(bx, corner->field_0x08), NavigationMultiply(by, corner->field_0x0c));
            } else {
                x = NavigationAdd(NavigationMultiply(bx, corner->field_0x0c), NavigationMultiply(by, radius));
                corner->field_0x00.v[0] = NavigationScalarRound(x);
                y = NavigationAdd(NavigationMultiply(by, corner->field_0x0c), NavigationMultiply(-corner->field_0x08, bx));
            }
            corner->field_0x00.v[1] = NavigationScalarRound(y);
            norm = NavigationSqrt(next_length);
            x = NavigationAdd(NavigationDivide(corner->field_0x00.v[0], norm), start.v[0]);
            y = NavigationAdd(NavigationDivide(corner->field_0x00.v[1], norm), start.v[1]);
            corner->field_0x00.v[0] = NavigationScalarRound(x);
            corner->field_0x00.v[1] = NavigationScalarRound(y);
        }
        count++;
        memcpy(&previous, &start, sizeof(previous));
        memcpy(&pWidth, &width, 4);
    }
    return count;
}

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// FUNCTION: CARMA2_HW 0x004b18b0
void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* pSocs, int pCount_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorners, tOpponent_spec* pOpponent_spec) {
    br_vector2 previous;
    br_vector2 position;
    const br_vector3* point;
    int i;
    int speed;
    int previous_corner = 0;
    float previous_extent;
    float distance = 0.f;
    float speed_squared;
    float stopping_distance;
    double x, y, length, offset, threshold;

    speed_squared = NavigationScalarRound(NavigationMultiply(pSpeed, pSpeed));
    memcpy(&previous, pPos2d, sizeof(previous));
    threshold = NavigationSubtract(NavigationMultiply(speed_squared, 0.0625f), NavigationMultiply(NavigationMultiply((double)gFrame_period_for_this_munging, pSpeed), -0.001f));
    stopping_distance = NavigationScalarRound(NavigationMultiply(threshold, 0.14492753148078918f));
    for (i = 0; i < pCount_socs; i++) {
        if (!NavigationLessOrUnordered(distance, stopping_distance)) {
            return;
        }
        if (previous_corner) {
            distance = NavigationScalarRound(NavigationSubtract(distance, previous_extent));
        }
        previous_corner = !pSocs[i].field_0x0;
        offset = distance;
        if (previous_corner) {
            point = GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pCorners[pSocs[i].field_0x4].section);
            speed = (int)pCorners[pSocs[i].field_0x4].field_0x18;
            memcpy(&previous_extent, &pCorners[pSocs[i].field_0x4].field_0x0c, 4);
            offset = NavigationSubtract(distance, previous_extent);
        } else {
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSocs[i].field_0x4);
            speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pSocs[i].field_0x4, 1);
        }
        memcpy(&position.v[0], &point->v[2], 4);
        memcpy(&position.v[1], &point->v[0], 4);
        x = NavigationFloatDifference(&previous.v[0], &position.v[0]);
        y = NavigationFloatDifference(&previous.v[1], &position.v[1]);
        length = NavigationSqrt(NavigationAdd(NavigationMultiply(x, NavigationScalarRound(x)), NavigationMultiply(y, y)));
        distance = NavigationScalarRound(NavigationAdd(length, offset));
        if (NavigationLessOrUnordered(distance, 0.f)) {
            distance = 0.f;
        }
        threshold = NavigationMultiply(NavigationSubtract(speed_squared, NavigationMultiply((double)speed, (double)speed)), 0.0625f);
        threshold = NavigationSubtract(threshold, NavigationMultiply(NavigationMultiply((double)gFrame_period_for_this_munging, pSpeed), -0.001f));
        if (NavigationOrderedGreater(threshold, NavigationMultiply(distance, 6.9f)) && NavigationLessOrUnordered((double)speed, *pDesired_speed)) {
            *pDesired_speed = NavigationScalarRound((double)speed);
        }
        memcpy(&previous, &position, sizeof(previous));
    }
}

static int C2_HOOK_CDECL NavigationTruncate(double pValue) {
    br_int_64 result;
#if defined(_MSC_VER) && defined(_M_IX86)
    unsigned short control;
    unsigned short truncate_control;
    __asm { fnstcw control }
    truncate_control = control | 0xc00;
    __asm {
        fldcw truncate_control
        fld qword ptr [pValue]
        fistp qword ptr [result]
        fldcw control
    }
#elif defined(__GNUC__) && defined(__i386__)
    unsigned short control;
    unsigned short truncate_control;
    __asm__ volatile ("fnstcw %0" : "=m"(control));
    truncate_control = control | 0xc00;
    __asm__ volatile ("fldcw %1; fldl %2; fistpll %0; fldcw %3" : "=m"(result) : "m"(truncate_control), "m"(pValue), "m"(control) : "st");
#else
    result = (br_int_64)pValue;
#endif
    return (int)(tU32)result;
}

/* This original measurement starts at the corner section, not the current section. */
static double C2_HOOK_CDECL NavigationCornerSectionLength(tOpponent_spec* pOpponent_spec, tCorner* pCorner, int pSection) {
    const br_vector3* start = GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pCorner->section);
    const br_vector3* finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection);
    double x = NavigationFloatDifference(&finish->v[0], &start->v[0]);
    double z = NavigationFloatDifference(&finish->v[2], &start->v[2]);
    return NavigationSqrt(NavigationAdd(NavigationMultiply(x, NavigationScalarRound(x)), NavigationMultiply(z, z)));
}

// FUNCTION: CARMA2_HW 0x004b1ab0
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) {
    int count = 0;
    int i;
    int speed;
    double length;

    (void)pCar;
    for (i = 0; i < pCount_corners; i++) {
        while (pNext_section != -1 && count < 10 && pNext_section != (int)pCorners[i].section) {
            length = NavigationCornerSectionLength(pOpponent_spec, &pCorners[i], pNext_section);
            if (!NavigationOrderedGreater(length, pCorners[i].field_0x0c)) {
                break;
            }
            pSocs[count].field_0x0 = -1;
            pSocs[count].field_0x4 = pNext_section;
            count++;
            pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
        }
        if (count < 10) {
            pSocs[count].field_0x0 = 0;
            pSocs[count].field_0x4 = i;
            length = NavigationMultiply(NavigationMultiply(NavigationMultiply(pCorners[i].field_0x08, 6.9f), 7.5f), 1.9600000381469727f);
            pCorners[i].field_0x18 = (tU32)NavigationTruncate(NavigationSqrt(length));
            while (pNext_section != (int)pCorners[i].section) {
                speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                if (speed < (int)pCorners[i].field_0x18) {
                    pCorners[i].field_0x18 = (tU32)(int)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                }
                pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
            }
            while (pNext_section != -1) {
                length = NavigationCornerSectionLength(pOpponent_spec, &pCorners[i], pNext_section);
                if (!NavigationLessOrUnordered(length, pCorners[i].field_0x0c)) {
                    break;
                }
                speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                if (speed < (int)pCorners[i].field_0x18) {
                    pCorners[i].field_0x18 = (tU32)(int)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                }
                pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
            }
            count++;
        }
    }
    return count;
}

// StuffDataFromCorner

// SetCurvature

// ProcessFollowPath

// FollowCheatyPath

