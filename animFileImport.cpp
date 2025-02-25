//
// File Name: animFileImport.cpp
//
// Description:
// Utility classes to read .anim files.
//
#include <maya/MGlobal.h>

#include <maya/MIOStream.h>
#include <maya/MFStream.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "animFileImport.h"
#include <maya/MString.h>
#include <maya/MFnAnimCurve.h>

//-------------------------------------------------------------------------
// Class animUnitNames
//-------------------------------------------------------------------------

// String names for units.
//
const char *kMmString = "mm";
const char *kCmString = "cm";
const char *kMString = "m";
const char *kKmString = "km";
const char *kInString = "in";
const char *kFtString = "ft";
const char *kYdString = "yd";
const char *kMiString = "mi";

const char *kMmLString = "millimeter";
const char *kCmLString = "centimeter";
const char *kMLString = "meter";
const char *kKmLString = "kilometer";
const char *kInLString = "inch";
const char *kFtLString = "foot";
const char *kYdLString = "yard";
const char *kMiLString = "mile";

const char *kRadString = "rad";
const char *kDegString = "deg";
const char *kMinString = "min";
const char *kSecString = "sec";

const char *kRadLString = "radian";
const char *kDegLString = "degree";
const char *kMinLString = "minute";
const char *kSecLString = "second";

const char *kHourTString = "hour";
const char *kMinTString = "min";
const char *kSecTString = "sec";
const char *kMillisecTString = "millisec";

const char *kGameTString = "game";
const char *kFileTString = "film";
const char *kPalTString = "pal";
const char *kNtscTString = "ntsc";
const char *kShowTString = "show";
const char *kPalFTString = "palf";
const char *kNtscFTString = "ntscf";

const char *kUnitlessString = "unitless";
const char *kUnknownTimeString = "Unknown Time Unit";
const char *kUnknownAngularString = "Unknown Angular Unit";
const char *kUnknownLinearString = "Unknown Linear Unit";

animUnitNames::animUnitNames()
//
// Description:
// Class constructor.
//
{
}

animUnitNames::~animUnitNames()
//
// Description:
// Class destructor.
//
{
}

/* static */
void animUnitNames::setToLongName(const MAngle::Unit& unit, MString& name)
//
// Description:
// Sets the string with the long text name of the angle unit.
//
{
    switch(unit) {
            case MAngle::kDegrees:
            name.set(kDegLString);
            break;
            case MAngle::kRadians:
            name.set(kRadLString);
            break;
            case MAngle::kAngMinutes:
            name.set(kMinLString);
            break;
            case MAngle::kAngSeconds:
            name.set(kSecLString);
            break;
            default:
            name.set(kUnknownAngularString);
        break;
    }
}

/* static */
void animUnitNames::setToShortName(const MAngle::Unit& unit, MString& name)
//
// Description:
// Sets the string with the short text name of the angle unit.
//
{
    switch(unit) {
        case MAngle::kDegrees:
        name.set(kDegString);
        break;
        case MAngle::kRadians:
        name.set(kRadString);
        break;
        case MAngle::kAngMinutes:
        name.set(kMinString);
        break;
        case MAngle::kAngSeconds:
        name.set(kSecString);
        break;
        default:
        name.set(kUnknownAngularString);
        break;
    }
}

/* static */
void animUnitNames::setToLongName(const MDistance::Unit& unit, MString& name)
//
// Description:
// Sets the string with the long text name of the distance unit.
//
{
    switch(unit) {
        case MDistance::kInches:
        name.set(kInLString);
        break;
        case MDistance::kFeet:
        name.set(kFtLString);
        break;
        case MDistance::kYards:
        name.set(kYdLString);
        break;
        case MDistance::kMiles:
        name.set(kMiLString);
        break;
        case MDistance::kMillimeters:
        name.set(kMmLString);
        break;
        case MDistance::kCentimeters:
        name.set(kCmLString);
        break;
        case MDistance::kKilometers:
        name.set(kKmLString);
        break;
        case MDistance::kMeters:
        name.set(kMLString);
        break;
        default:
        name.set(kUnknownLinearString);
        break;
    }
}

/* static */
void animUnitNames::setToShortName(const MDistance::Unit& unit, MString& name)
//
// Description:
// Sets the string with the short text name of the distance unit.
//
{
    switch(unit) {
        case MDistance::kInches:
        name.set(kInString);
        break;
        case MDistance::kFeet:
        name.set(kFtString);
        break;
        case MDistance::kYards:
        name.set(kYdString);
        break;
        case MDistance::kMiles:
        name.set(kMiString);
        break;
        case MDistance::kMillimeters:
        name.set(kMmString);
        break;
        case MDistance::kCentimeters:
        name.set(kCmString);
        break;
        case MDistance::kKilometers:
        name.set(kKmString);
        break;
        case MDistance::kMeters:
        name.set(kMString);
        break;
        default:
        name.set(kUnknownLinearString);
        break;
    }
}

/* static */
void animUnitNames::setToLongName(const MTime::Unit &unit, MString &name)
//
// Description:
// Sets the string with the long text name of the time unit.
//
{
    switch(unit) {
        case MTime::kHours:
        name.set(kHourTString);
        break;
        case MTime::kMinutes:
        name.set(kMinTString);
        break;
        case MTime::kSeconds:
        name.set(kSecTString);
        break;
        case MTime::kMilliseconds:
        name.set(kMillisecTString);
        break;
        case MTime::kGames:
        name.set(kGameTString);
        break;
        case MTime::kFilm:
        name.set(kFileTString);
        break;
        case MTime::kPALFrame:
        name.set(kPalTString);
        break;
        case MTime::kNTSCFrame:
        name.set(kNtscTString);
        break;
        case MTime::kShowScan:
        name.set(kShowTString);
        break;
        case MTime::kPALField:
        name.set(kPalFTString);
        break;
        case MTime::kNTSCField:
        name.set(kNtscFTString);
        break;
        default:
        name.set(kUnknownTimeString);
        break;
    }
}

/* static */
void animUnitNames::setToShortName(const MTime::Unit &unit, MString &name)
//
// Description:
// Sets the string with the short text name of the time unit.
//
{
    setToLongName(unit, name);
}

//-------------------------------------------------------------------------
// Class animBase
//-------------------------------------------------------------------------

// Tangent type words
//
const char *kWordTangentGlobal = "global";
const char *kWordTangentFixed = "fixed";
const char *kWordTangentLinear = "linear";
const char *kWordTangentFlat = "flat";
const char *kWordTangentSmooth = "spline";
const char *kWordTangentStep = "step";
const char *kWordTangentSlow = "slow";
const char *kWordTangentFast = "fast";
const char *kWordTangentClamped = "clamped";

// Infinity type words
//
const char *kWordConstant = "constant";
const char *kWordLinear = "linear";
const char *kWordCycle = "cycle";
const char *kWordCycleRelative = "cycleRelative";
const char *kWordOscillate = "oscillate";

// Param Curve types
//
const char *kWordTypeUnknown = "unknown";
const char *kWordTypeLinear = "linear";
const char *kWordTypeAngular = "angular";
const char *kWordTypeTime = "time";
const char *kWordTypeUnitless = "unitless";

// Keywords
//
const char *kAnim = "anim";
const char *kAnimData = "animData";
const char *kMovData = "movData";
const char *kMayaVersion = "mayaVersion";
const char *kAnimVersion = "animVersion";

const char *kTimeUnit = "timeUnit";
const char *kLinearUnit = "linearUnit";
const char *kAngularUnit = "angularUnit";
const char *kStartTime = "startTime";
const char *kEndTime = "endTime";
const char *kStartUnitless = "startUnitless";
const char *kEndUnitless = "endUnitless";

// animVersions:
//
const char *kAnimVersionString = "1.1";

const char *kTwoSpace = "  ";

// animData keywords
//
const char *kInputString = "input";
const char *kOutputString = "output";
const char *kWeightedString = "weighted";
const char *kPreInfinityString = "preInfinity";
const char *kPostInfinityString = "postInfinity";
const char *kInputUnitString = "inputUnit";
const char *kOutputUnitString = "outputUnit";
const char *kTanAngleUnitString = "tangentAngleUnit";
const char *kKeysString = "keys";

// special characters
//
const char kSemiColonChar = ';';
const char kSpaceChar = ' ';
const char kBraceLeftChar = '{';
const char kBraceRightChar = '}';

animBase::animBase ()
//
// Description:
// The constructor.
//
{
resetUnits();
}

animBase::~animBase()
//
// Description:
// The destructor.
//
{
}

void animBase::resetUnits()
//
// Description:
// Reset the units used by this class to the ui units.
//
{
timeUnit = MTime::uiUnit();
linearUnit = MDistance::uiUnit();
angularUnit = MAngle::uiUnit();
}

const char *
animBase::tangentTypeAsWord(MFnAnimCurve::TangentType type)
//
// Description:
// Returns a string with a test based desription of the passed
// MFnAnimCurve::TangentType.
//
{
    switch (type) {
        case MFnAnimCurve::kTangentGlobal:
        return (kWordTangentGlobal);
        case MFnAnimCurve::kTangentFixed:
        return (kWordTangentFixed);
        case MFnAnimCurve::kTangentLinear:
        return (kWordTangentLinear);
        case MFnAnimCurve::kTangentFlat:
        return (kWordTangentFlat);
        case MFnAnimCurve::kTangentSmooth:
        return (kWordTangentSmooth);
        case MFnAnimCurve::kTangentStep:
        return (kWordTangentStep);
        case MFnAnimCurve::kTangentSlow:
        return (kWordTangentSlow);
        case MFnAnimCurve::kTangentFast:
        return (kWordTangentFast);
        case MFnAnimCurve::kTangentClamped:
        return (kWordTangentClamped);
        default:
        break;
    }
    return (kWordTangentGlobal);
}

MFnAnimCurve::TangentType animBase::tangentTypeAsEnum(MString str){

    if(str == kWordTangentFixed)return MFnAnimCurve::kTangentFixed;
    else if(str == kWordTangentLinear)return MFnAnimCurve::kTangentLinear;
    else if(str == kWordTangentFlat)return MFnAnimCurve::kTangentFlat;
    else if(str == kWordTangentSmooth)return MFnAnimCurve::kTangentSmooth;
    else if(str == kWordTangentStep)return MFnAnimCurve::kTangentStep;
    else if(str == kWordTangentClamped)return MFnAnimCurve::kTangentClamped;
    else return MFnAnimCurve::kTangentGlobal;
}

MFnAnimCurve::InfinityType animBase::infinityTypeAsEnum(MString str){

    if(str == kWordLinear)return MFnAnimCurve::kLinear;
    else if(str == kWordConstant)return MFnAnimCurve::kConstant;
    else if(str == kWordCycle)return MFnAnimCurve::kCycle;
    else if(str == kWordCycleRelative)return MFnAnimCurve::kCycleRelative;
    else if(str == kWordOscillate)return MFnAnimCurve::kOscillate;
    else return MFnAnimCurve::kConstant;
}

const char *
animBase::infinityTypeAsWord(MFnAnimCurve::InfinityType type)
//
// Description:
// Returns a string containing the name of the passed
// MFnAnimCurve::InfinityType type.
//
{
    switch (type) {
        case MFnAnimCurve::kConstant:
        return (kWordConstant);
        case MFnAnimCurve::kLinear:
        return (kWordLinear);
        case MFnAnimCurve::kCycle:
        return (kWordCycle);
        case MFnAnimCurve::kCycleRelative:
        return (kWordCycleRelative);
        case MFnAnimCurve::kOscillate:
        return (kWordOscillate);
        default:
        break;
    }
    return (kWordConstant);
}

const char *
animBase::outputTypeAsWord (MFnAnimCurve::AnimCurveType type)
//
// Description:
// Returns a string identifying the output type of the
// passed MFnAnimCurve::AnimCurveType.
//
{
    switch (type) {
        case MFnAnimCurve::kAnimCurveTL:
        case MFnAnimCurve::kAnimCurveUL:
        return (kWordTypeLinear);
        case MFnAnimCurve::kAnimCurveTA:
        case MFnAnimCurve::kAnimCurveUA:
        return (kWordTypeAngular);
        case MFnAnimCurve::kAnimCurveTT:
        case MFnAnimCurve::kAnimCurveUT:
        return (kWordTypeTime);
        case MFnAnimCurve::kAnimCurveTU:
        case MFnAnimCurve::kAnimCurveUU:
        return (kWordTypeUnitless);
        case MFnAnimCurve::kAnimCurveUnknown:
        return (kWordTypeUnitless);
    }
    return (kWordTypeUnknown);
}

const char *
animBase::boolInputTypeAsWord(bool isUnitless)
//
// Description:
// Returns a string based on a bool.
//
{
if (isUnitless) {
        return (kWordTypeUnitless);
    } else {
        return (kWordTypeTime);
    }
}

//-------------------------------------------------------------------------
// Class animReader
//-------------------------------------------------------------------------

animReader::animReader()
//
// Description:
// Class constructor.
//
{
}

animReader::~animReader()
//
// Description:
// Class destructor.
//
{
}

bool animReader::readHeader(ofstream& clip,
double startTime, double endTime,
double startUnitless, double endUnitless
)
    //
    // Description:
    // reads the header for the file. The header contains the clipboard
    // specific data.
    //
    {
    if (!clip) {
        return false;
    }

    clip << kAnimVersion << kSpaceChar << kAnimVersionString << kSemiColonChar << endl;
    clip << kMayaVersion << kSpaceChar << MGlobal::mayaVersion() << kSemiColonChar << endl;

    if (startTime != endTime) {
        MString unit;
        animUnitNames::setToShortName(timeUnit, unit);
        clip << kTimeUnit << kSpaceChar << unit << kSemiColonChar << endl;
        animUnitNames::setToShortName(linearUnit, unit);
        clip << kLinearUnit << kSpaceChar << unit << kSemiColonChar << endl;
        animUnitNames::setToShortName(angularUnit, unit);
        clip << kAngularUnit << kSpaceChar << unit << kSemiColonChar << endl;
        clip << kStartTime << kSpaceChar << startTime << kSemiColonChar << endl;
        clip << kEndTime << kSpaceChar << endTime << kSemiColonChar << endl;
    }

    if (startUnitless != endUnitless) {
        clip << kStartUnitless << kSpaceChar << startUnitless <<
        kSemiColonChar << endl;
        clip << kEndUnitless << kSpaceChar << endUnitless <<
        kSemiColonChar << endl;
    }

    return true;
}

bool animReader::readAnimCurve(ofstream &clip,
const MObject *animCurveObj,
bool verboseUnits /* false */)
//
// Description:
// read out the anim curve from the clipboard item into the
// ofstream. The actual anim curve data is written out.
//
// This method returns true if the read was successful.
//
{
    if (NULL == animCurveObj || animCurveObj->isNull() || !clip) {
        return true;
    }

    MStatus status = MS::kSuccess;
    MFnAnimCurve animCurve(*animCurveObj, &status);
    if (MS::kSuccess != status) {
        cerr << "Error: Could not read the anim curve for Importread." << endl;
        return false;
    }

    clip << kAnimData << kSpaceChar << kBraceLeftChar << endl;

    clip << kTwoSpace << kInputString << kSpaceChar <<
    boolInputTypeAsWord(animCurve.isUnitlessInput()) <<
    kSemiColonChar << endl;

    clip << kTwoSpace << kOutputString << kSpaceChar <<
    outputTypeAsWord(animCurve.animCurveType()) << kSemiColonChar << endl;

    clip << kTwoSpace << kWeightedString << kSpaceChar <<
    (animCurve.isWeighted() ? 1 : 0) << kSemiColonChar << endl;

    // These units default to the units in the header of the file.
    //
    if (verboseUnits) {
        clip << kTwoSpace << kInputUnitString << kSpaceChar;
        if (animCurve.isTimeInput()) {
            MString unitName;
        animUnitNames::setToShortName(timeUnit, unitName);
        clip << unitName;
        } else {
            // The anim curve has unitless input.
            //
            clip << kUnitlessString;
        }
        clip << kSemiColonChar << endl;

        clip << kTwoSpace << kOutputUnitString << kSpaceChar;
    }

    double conversion = 1.0;
    MString unitName;
    switch (animCurve.animCurveType()) {
        case MFnAnimCurve::kAnimCurveTA:
        case MFnAnimCurve::kAnimCurveUA:
        animUnitNames::setToShortName(angularUnit, unitName);
        if (verboseUnits) clip << unitName;
        {
        MAngle angle(1.0);
            conversion = angle.as(angularUnit);
        }
        break;
        case MFnAnimCurve::kAnimCurveTL:
        case MFnAnimCurve::kAnimCurveUL:
        animUnitNames::setToShortName(linearUnit, unitName);
        if (verboseUnits) clip << unitName;
        {
            MDistance distance(1.0);
            conversion = distance.as(linearUnit);
        }
        break;
        case MFnAnimCurve::kAnimCurveTT:
        case MFnAnimCurve::kAnimCurveUT:
        animUnitNames::setToShortName(timeUnit, unitName);
        if (verboseUnits) clip << unitName;
            break;
        default:
        if (verboseUnits) clip << kUnitlessString;
            break;
        }
        if (verboseUnits) clip << kSemiColonChar << endl;

    if (verboseUnits) {
        MString angleUnitName;
        animUnitNames::setToShortName(angularUnit, angleUnitName);
        clip << kTwoSpace << kTanAngleUnitString <<
        kSpaceChar << angleUnitName << kSemiColonChar << endl;
    }

    clip << kTwoSpace << kPreInfinityString << kSpaceChar <<
    infinityTypeAsWord(animCurve.preInfinityType()) <<
    kSemiColonChar << endl;

    clip << kTwoSpace << kPostInfinityString << kSpaceChar <<
    infinityTypeAsWord(animCurve.postInfinityType()) <<
    kSemiColonChar << endl;

    clip << kTwoSpace << kKeysString << kSpaceChar << kBraceLeftChar << endl;

    // And then read out each keyframe
    //
    unsigned numKeys = animCurve.numKeyframes();
    for (unsigned i = 0; i < numKeys; i++) {
        clip << kTwoSpace << kTwoSpace;
        if (animCurve.isUnitlessInput()) {
            clip << animCurve.unitlessInput(i);
        }
        else {
            clip << animCurve.time(i).value();
        }

        clip << kSpaceChar << (conversion*animCurve.value(i));

        clip << kSpaceChar << tangentTypeAsWord(animCurve.inTangentType(i));
        clip << kSpaceChar << tangentTypeAsWord(animCurve.outTangentType(i));

        clip << kSpaceChar << (animCurve.tangentsLocked(i) ? 1 : 0);
        clip << kSpaceChar << (animCurve.weightsLocked(i) ? 1 : 0);
        clip << kSpaceChar << (animCurve.isBreakdown(i) ? 1 : 0);

        if (animCurve.inTangentType(i) == MFnAnimCurve::kTangentFixed) {
            MAngle angle;
            double weight;
            animCurve.getTangent(i, angle, weight, true);

            clip << kSpaceChar << angle.as(angularUnit);
            clip << kSpaceChar << weight;
        }
        if (animCurve.outTangentType(i) == MFnAnimCurve::kTangentFixed) {
            MAngle angle;
            double weight;
            animCurve.getTangent(i, angle, weight, false);

            clip << kSpaceChar << angle.as(angularUnit);
            clip << kSpaceChar << weight;
        }

        clip << kSemiColonChar << endl;
    }
    clip << kTwoSpace << kBraceRightChar << endl;

    clip << kBraceRightChar << endl;

    return true;
}
