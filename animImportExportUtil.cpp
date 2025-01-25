//-
// ==========================================================================
// Copyright (C) 1995 - 2005 Alias Systems Corp. and/or its licensors.  All
// rights reserved.
//
// The coded instructions, statements, computer programs, and/or related
// material (collectively the "Data") in these files are provided by Alias
// Systems Corp. ("Alias") and/or its licensors for the exclusive use of the
// Customer (as defined in the Alias Software License Agreement that
// accompanies this Alias software). Such Customer has the right to use,
// modify, and incorporate the Data into other products and to distribute such
// products for use by end-users.
//
// THE DATA IS PROVIDED "AS IS".  ALIAS HEREBY DISCLAIMS ALL WARRANTIES
// RELATING TO THE DATA, INCLUDING, WITHOUT LIMITATION, ANY AND ALL EXPRESS OR
// IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE. IN NO EVENT SHALL ALIAS BE LIABLE FOR ANY DAMAGES
// WHATSOEVER, WHETHER DIRECT, INDIRECT, SPECIAL, OR PUNITIVE, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, OR IN EQUITY,
// ARISING OUT OF ACCESS TO, USE OF, OR RELIANCE UPON THE DATA.
// ==========================================================================
//+

//
// File Name: animImportUtil.h
//
// Description: an animation Import utility which illustrates how to
// use the MAnimUtil animation helper class, as well as how to Import
// animation using the Maya .anim format
//
//

// *****************************************************************************

// INCLUDED HEADER FILES

#include <maya/MFStream.h>
#include <stdlib.h>
#include <string.h>

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnSet.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MEulerRotation.h>
#include <maya/MAngle.h>

#include <maya/MFnAnimCurve.h>
#include <maya/MAnimUtil.h>

#include "animImportUtil.h"
#include "animFileImport.h"

#if defined (OSMac_)
using namespace std;
extern "C" int strcasecmp (const char *, const char *);
#endif
// *****************************************************************************

// HELPER METHODS

// *****************************************************************************

#ifndef min
static inline double
min (double a, double b)
{
   return (a < b ? a : b);
}
#endif

#ifndef max
static inline double
max (double a, double b)
{
   return (a > b ? a : b);
}
#endif

// *****************************************************************************

// PUBLIC MEMBER FUNCTIONS

TanimImportUtil::TanimImportUtil()
{
}

TanimImportUtil::~TanimImportUtil()
{
}

MStatus
TanimImportUtil::reader (
   const MFileObject &file,
   const MString &options,
   FileAccessMode mode
)
{
   MStatus stat;
   animBase animb;
   MString $exportFile;
   float tan1;
   float tan2;
   float weight1;
   float weight2;
   MString weighted;
   MString preI;
   MString postI;
   MString attr;
   MString node;
   MString line;
   MString curAttr;
   MStringArray buffer;
   MStringArray buffer2;
   MStringArray buffer3;
   MTime time ;
   double keyvalue = 0;
   MString inType = "";
   MString outType = "";
   int tanLock = 0;
   int weightLock = 0;
   int breakDown = 0;
   int keyIndex=0;
   MStringArray filteredNames;
   MStringArray test2;
   MSelectionList sl;
   MObject obj;

   int size;
   int endit = 0;
   int weightState=0;
   // Create the Import file
   //
   //ofstream animFile (file.fullName().asChar());
   //MGlobal::displayInfo("readin");
   //cout<<"test"<<endl;
   ifstream animFile (file.fullName().asChar());

   string linetemp;
   //MString line("abc def ghi jkl");
   char sp=' ';
   //line.split(sp,buffer);
   //for(unsigned int i=0;i<buffer.length();i++)MGlobal::displayInfo(buffer[i]);
   //ifstream myfile ("example.txt");
   if (animFile.is_open())
   {
      while (! animFile.eof() )
      {
            getline (animFile,linetemp);
            line.clear();
            line=linetemp.c_str();
            //MGlobal::displayInfo(line);
            //------------------------------------------------------------------------------------------------------------------------------------
            //$line = fgetline($fileID);
            weightState = 0;
            //if (size($line) > 0)
            if(line.length())
            {

               if( line.substring(0,3)=="anim" || line.substring(0,5)=="static")
               {

                  //size = tokenize (line," ",buffer);
                  buffer.clear();
                  line.split(sp,buffer);
                  size=buffer.length();
                  //$buffer[5] = `substitute "\n" $buffer[5] ""`;
                  if(size == 7 || size == 6)
                  {
                     //$filteredNames = dk_filter_nodes ( $line, $paths, $doReplace, $search, $replace, $prefix, $prefixTopNodes, $useChannels );
                     filteredNames.setLength(2);
                     filteredNames[0].clear();
                     filteredNames[1].clear();
                     //Set Attr
                     filteredNames[0]=buffer[1];
                     //Set Node
                     filteredNames[1]=buffer[3];
                     //$curAttr = $filteredNames[0] + "." + $filteredNames[1];
                     curAttr=filteredNames[0];
                     //$node = $filteredNames[0];
                     node=filteredNames[1];
                     sl.clear();
                     stat=MGlobal::getSelectionListByName(node,sl);
                     sl.getDependNode(0, obj);
                     MFnDependencyNode dep(obj);
                     MPlug plug=dep.findPlug(curAttr);
                     endit=0;

                     if(line.substring(0,5)=="static")
                     {
                        MGlobal::displayInfo("static....");
                        //string $connected[] = `listConnections -d 0 $curAttrLong`;
                        MPlugArray tempArray;
                        dep.getConnections(tempArray);

                        if(buffer[5].isInt())plug.setValue(buffer[5].asInt());
                        else if (buffer[5].isShort())plug.setValue(buffer[5].asShort());
                        else if (buffer[5].isDouble())plug.setValue(buffer[5].asDouble());
                        else if (buffer[5].isFloat())plug.setValue(buffer[5].asFloat());
                        else plug.setValue(buffer[5].asDouble());
                        //}
                        //else{
                        //print ("Warning: Attribute is locked - " + $curAttr + "\n");
                        // MGlobal::displayInfo("Warning Attribute is locked -"+plug.name());
                        //}
                        //getline (animFile,linetemp);
                        //line=linetemp.c_str();
                     }

                     //if (`substring $line 1 5` == "anim ")
                     if( line.substring(0,3)=="anim" )
                     {
                        MFnAnimCurve animCurvetemp;
                        MObject animNode=animCurvetemp.create(plug);
                        MFnAnimCurve animCurve(animNode);
                        MGlobal::displayInfo(plug.name());
                        MGlobal::displayInfo("anim....");
                        while (endit == 0)
                        {
                           //$line = fgetline($fileID);
                           getline (animFile,linetemp);
                           line=linetemp.c_str();
                           //if(`substring $line 3 11` == "weighted ")
                           if(line.substring(2,9)=="weighted")
                           {
                              //if(`substring $line 12 12` == "1")
                              if(line.substring(11,11)=="1")
                              {
                                 //$weighted = "keyTangent -edit -weightedTangents true " + $curAttr;
                                 weightState = 1;
                              }
                              else
                              weightState = 0;
                           }

                           //if(`substring $line 3 7` == "preIn")
                           if(line.substring(2,6)=="preIn")
                           {
                              //tokenize ($line," ",$buffer);
                              buffer.clear();
                              line.split(sp,buffer);
                              //tokenize ($buffer[1],";",$buffer);
                              buffer[1].split(';',buffer);
                              preI = buffer[0];
                           }

                           //if(`substring $line 3 8` == "postIn")
                           if(line.substring(2,7)=="postIn")
                           {
                              //tokenize ($line," ",$buffer);
                              buffer.clear();
                              line.split(sp,buffer);
                              //tokenize ($buffer[1],";",$buffer);
                              line.split(';',buffer);
                              postI = buffer[0];
                           }

                           //if(`substring $line 3 8` == "keys {")
                           if(line.substring(2,7)=="keys {")
                           {
                              MGlobal::displayInfo("keys....");
                              //$line = fgetline($fileID);
                              getline (animFile,linetemp);
                              line=linetemp.c_str();
                              //while (`substring $line 3 3` != "}")
                              while(line.substring(2,2)!="}")
                              {
                                 MGlobal::displayInfo(line);
                                    //int $argNum = tokenize ($line," ",$buffer);
                                 buffer.clear();
                                 line.split(sp,buffer);
                                 MGlobal::displayInfo("Start Extract");
                                 for(unsigned int i=0;i<buffer.length();i++)MGlobal::displayInfo(buffer[i]);
                                 MGlobal::displayInfo("End Extract");
                                 int argNum=buffer.length();
                                 time = buffer[0].asFloat();

                                 if(animCurve.animCurveType()==MFnAnimCurve::kAnimCurveTA ){
                                    MAngle ang(buffer[1].asDouble(),MAngle::kDegrees );
                                    ang.setUnit(MAngle::kDegrees);
                                    keyvalue=ang.asRadians();
                                 }
                                 else keyvalue=buffer[1].asDouble();
                                 //keyvalue=ang.asDegrees();
                                    inType = buffer[2];
                                    outType = buffer[3];
                                    tanLock = buffer[4].asInt();
                                    weightLock = buffer[5].asInt();
                                    breakDown = 0;
                                    //MString tempTime;
                                 //tempTime.set(time);
                                 //MGlobal::displayInfo(tempTime);
                                 if (argNum == 7)
                                 {
                                    //tokenize ($buffer[6],";",$buffer2);
                                    buffer2.clear();
                                    buffer[6].split(';',buffer2);

                                    breakDown = buffer2[0].asInt();
                                 }
                                 // additional args for in/out Angle, in/out Weight on fixed tangents
                                 else
                                 {
                                    breakDown = buffer[6].asInt();
                                    // only one tangent direction is 'fixed'
                                 if (argNum > 7)
                                 {
                                       tan1 = buffer[7].asFloat();
                                       // both in and out are 'fixed'
                                    if (argNum > 9)
                                    {
                                       buffer2.clear();
                                       weight1=buffer[8].asFloat();
                                       tan2 = buffer[9].asFloat();
                                       //tokenize ($buffer[10],";",$buffer2);
                                       buffer[10].split(';',buffer2);
                                       weight2 = buffer2[0].asFloat();
                                    }
                                    else{
                                       //tokenize ($buffer[8],";",$buffer2);
                                       buffer2.clear();
                                       buffer[8].split(';',buffer2);
                                       weight1 = buffer2[0].asFloat();
                                    }
                                 }
                              }
                              // set keyframe
                              //string $cmd = ("setKeyframe -time " + $time + " -value " + $value +" -bd " + $breakDown + " " + $curAttr);
                              //eval $cmd;
                              MString tempValue;
                              tempValue.set(keyvalue);
                              MGlobal::displayInfo("value="+tempValue);
                              MEulerRotation rot(keyvalue,keyvalue,keyvalue);
                              keyIndex=animCurve.addKey(time,rot.x,animb.tangentTypeAsEnum(inType),animb.tangentTypeAsEnum(outType));
                              //set breakdown
                              animCurve.setIsBreakdown(keyIndex,breakDown);
                              //keyTangent -lock $tanLock -t $time $curAttr;
                              //animCurve.setTangentsLocked(keyIndex,tanLock);
                              if (weightState == 1)
                              {
                                 //eval $weighted;
                                 //keyTangent -t $time -weightLock $weightLock $curAttr;
                                 //animCurve.setWeightsLocked(keyIndex,weightLock);
                                 animCurve.setIsWeighted(1);
                              }
                              else animCurve.setIsWeighted(0);;

                              if(inType != "fixed" && outType != "fixed"){
                              //keyTangent -e -a -t $time -itt $inType -ott $outType $curAttr;
                              }
                              if(inType == "fixed" && outType != "fixed"){
                                    //keyTangent -e -a -t $time -inAngle $tan1 -inWeight $weight1 -itt $inType -ott $outType $curAttr;
                                    animCurve.setWeightsLocked(keyIndex,0);
                                    animCurve.setTangentsLocked(keyIndex,0);
                                    MAngle angle1(tan1,MAngle::kDegrees);
                                    animCurve.setTangent(keyIndex,angle1,weight1,1);
                                    MString tmp;
                                    tmp.set(tan1);
                                    MGlobal::displayInfo("inTangent="+tmp);
                                    tmp.set(weight1);
                                    MGlobal::displayInfo("weight="+tmp);
                                    //animCurve.setWeight(keyIndex,(double)weight1,1);
                              }
                              if(inType != "fixed" && outType == "fixed"){
                                 //keyTangent -e -a -t $time -outAngle $tan1 -outWeight $weight1 -itt $inType -ott $outType $curAttr;
                                 animCurve.setWeightsLocked(keyIndex,0);
                                 animCurve.setTangentsLocked(keyIndex,0);
                                 MAngle angle1(tan1,MAngle::kDegrees);
                                 animCurve.setTangent(keyIndex,angle1,weight1,0);
                                 MString tmp;
                                 tmp.set(tan2);
                                 MGlobal::displayInfo("outTangent="+tmp);
                                 tmp.set(weight2);
                                 MGlobal::displayInfo("weight="+tmp);
                                 //animCurve.setWeight(keyIndex,(double)weight2,0);
                              }
                              if(inType == "fixed" && outType == "fixed"){
                                 //keyTangent -e -a -t $time -inAngle $tan1 -inWeight $weight1 -outAngle $tan2 -outWeight $weight2 -itt $inType -ott $outType $curAttr;
                                 animCurve.setWeightsLocked(keyIndex,0);
                                 animCurve.setTangentsLocked(keyIndex,0);
                                 MString tmp;
                                 tmp.set(tan1);
                                 MGlobal::displayInfo("tangent1="+tmp);
                                 tmp.set(weight1);
                                 MGlobal::displayInfo("weight1="+tmp);
                                 tmp.set(tan2);
                                 MGlobal::displayInfo("tangent2="+tmp);
                                 tmp.set(weight2);
                                 MGlobal::displayInfo("weight2="+tmp);
                                 MAngle angle1(tan1,MAngle::kDegrees);
                                 animCurve.setTangent(keyIndex,angle1,weight1,1);
                                 MAngle angle2(tan2,MAngle::kDegrees);
                                 animCurve.setTangent(keyIndex,angle2,weight2,-0);
                                 //animCurve.setWeight(keyIndex,(double)weight1,1);
                                 //animCurve.setWeight(keyIndex,(double)weight2,0);
                              }
                              //$line = fgetline($fileID);
                              getline (animFile,linetemp);
                              line.clear();
                              line=linetemp.c_str();
                           }//while(line.substring(2,2)!="}")
                           //setInfinity -poi $postI -pri $preI $curAttr;
                           animCurve.setPreInfinityType(animb.infinityTypeAsEnum(inType));
                           animCurve.setPostInfinityType(animb.infinityTypeAsEnum(outType));
                           endit = 1;
                           }//if(line.substring(2,7)=="keys {")
                        }//while (endit == 0)
                  }
               }//if(size == 7 || size == 6)
            }//if( line.substring(0,3)=="anim" || line.substring(0,5)=="static")
         }//if(line.length())
            //------------------------------------------------------------------------------------------------------------------------------------
         cout << line << endl;
      }//while (! animFile.eof() )
      animFile.close();
   }//if (animFile.is_open())
   else cout << "Unable to open file";

   return (MS::kSuccess);
}

void
TanimImportUtil::read (ofstream &animFile, const MDagPath &path)
{
   // Walk through the dag breadth first
   //
   MItDag dagIt (MItDag::kDepthFirst);
   dagIt.reset (path, MItDag::kDepthFirst);
   for (; !dagIt.isDone(); dagIt.next()) {
      MDagPath thisPath;
      if (dagIt.getPath (thisPath) != MS::kSuccess) {
         continue;
      }
      // Find the animated plugs for this object
      //
      MPlugArray animatedPlugs;
      MObject node = thisPath.node();
      MFnDependencyNode fnNode (node);
      MAnimUtil::findAnimatedPlugs (thisPath, animatedPlugs);
      unsigned int numPlugs = animatedPlugs.length();
      if (numPlugs == 0) {
            // If the object is not animated, then read out place holder
            // information
            //
            animFile << "anim " << fnNode.name().asChar() << " " << dagIt.depth() << " " << thisPath.childCount() << " 0;\n";
      }
      else {
         // Otherwise read out each animation curve
         //
         readAnimatedPlugs (animFile, animatedPlugs, fnNode.name(), dagIt.depth(), thisPath.childCount());
      }
   }
}

void
TanimImportUtil::read (ofstream &animFile, const MObject &node)
{
// Watch out for characters and handle them a little differently
//
   if (node.hasFn (MFn::kCharacter)) {
      MObjectArray characterList;
      characterList.append (node);
      MIntArray depths;
      depths.append (0);
      unsigned int current = 0;
      while (current < characterList.length()) {
         const MObject &thisNode = characterList[current];
         int thisDepth = depths[current++];
         // If this node is a character, then check for any immediate
         // subCharacters
         //
         MFnSet fnSet (thisNode);
         // Now find the set members
         //
         MSelectionList members;
         fnSet.getMembers (members, false);
         unsigned int childCount = 0;
         MItSelectionList iter (members, MFn::kCharacter);
         for (; !iter.isDone(); iter.next()) {
            MObject childNode;
            iter.getDependNode (childNode);
            characterList.insert (childNode, current + childCount);
            depths.insert (thisDepth + 1, current + childCount);
            childCount++;
         }
         // Find the animated plugs for this object
         //
         MPlugArray animatedPlugs;
         MAnimUtil::findAnimatedPlugs (thisNode, animatedPlugs);
         unsigned int numPlugs = animatedPlugs.length();
         if (numPlugs == 0) {
            // If the object is not animated, then read out place holder
            // information
            //
            animFile << "anim " << fnSet.name().asChar() << " " << thisDepth << " " << childCount << " 0;\n";
         }
         else {
            // Otherwise read out each animation curve
            //
            readAnimatedPlugs (animFile, animatedPlugs, fnSet.name(), thisDepth, childCount);
         }
      }
      return;
   }
   // Find the animated plugs for this object
   //
   MPlugArray animatedPlugs;
   MFnDependencyNode fnNode (node);
   MAnimUtil::findAnimatedPlugs (node, animatedPlugs);
   unsigned int numPlugs = animatedPlugs.length();
   if (numPlugs != 0) {
   // If the object is animated the read out each animation curve
   //
      readAnimatedPlugs (animFile, animatedPlugs, fnNode.name(), 0, 0);
   }
}

void
TanimImportUtil::readAnimatedPlugs (
   ofstream &animFile,
   const MPlugArray &animatedPlugs,
   const MString &nodeName,
   unsigned int depth,
   unsigned int childCount
)
{
   // Walk through each animated plug and read out the animation curve(s)
   //
   unsigned int numPlugs = animatedPlugs.length();
   for (unsigned int i = 0; i < numPlugs; i++) {
      MPlug plug = animatedPlugs[i];
      MObjectArray animation;
      if (!MAnimUtil::findAnimation (plug, animation)) {
         continue;
      }
      // read out the plugs' anim statement
      //
      animFile << "anim ";
      // build up the full attribute name
      //
      MPlug attrPlug (plug);
      MObject attrObj = attrPlug.attribute();
      MFnAttribute fnAttr (attrObj);
      MString fullAttrName (fnAttr.name());
      attrPlug = attrPlug.parent();
      while (!attrPlug.isNull()) {
         attrObj = attrPlug.attribute();
         MFnAttribute fnAttr (attrObj);
         fullAttrName = fnAttr.name() + "." + fullAttrName;
         attrPlug = attrPlug.parent();
      }
      attrObj = plug.attribute();
      MFnAttribute fnLeafAttr (attrObj);
      animFile << fullAttrName.asChar() << " " << fnLeafAttr.name().asChar() << " " << nodeName.asChar() << " " << depth << " " << childCount << " " << i << ";\n";
      unsigned int numCurves = animation.length();
      // read out each animation curve that animates the plug
      //
      for (unsigned int j = 0; j < numCurves; j++) {
         MObject animCurveNode = animation[j];
         if (!animCurveNode.hasFn (MFn::kAnimCurve)) {
            continue;
         }
         animReader Reader;
         Reader.readAnimCurve (animFile, &animCurveNode);
      }
   }
}

bool
TanimImportUtil::haveReadMethod () const
{
   return (true);
}

MString
TanimImportUtil::defaultExtension () const
{
   return (MString("anim"));
}

MPxFileTranslator::MFileKind
TanimImportUtil::identifyFile (
   const MFileObject &file,
   const char * /*buffer*/,
   short /*size*/
) const
{
   const char *name = file.name().asChar();
   int   nameLength = (int)strlen(name);

   if ((nameLength > 5) && !strcasecmp(name+nameLength-5, ".anim")) {
      return (kIsMyFileType);
   }

   return (kNotMyFileType);
}

/* static */ void *
TanimImportUtil::creator ()
{
   return (new TanimImportUtil);
}

//--------------------------------------------------------------------------------
// Plugin management
//--------------------------------------------------------------------------------

//const char *const animExportOptionScript = "animImportOptions";
//const char *const animExportDefaultOptions = "precision=8;nodeNames=1;verboseUnits=0;whichRange=1;range=0:10;options=keys;hierarchy=none;controlPoints=0;shapes=1;helpPictures=0;useChannelBox=0;copyKeyCmd=";

MStatus
initializePlugin (MObject obj)
{
   MStatus status;
   MFnPlugin plugin (obj, "Alias", "3.0");
   status = plugin.registerFileTranslator ("animImportUtil", "", TanimImportUtil::creator,
   "",
   "", true );
   return (status);
}

MStatus
uninitializePlugin (MObject obj)
{
   MFnPlugin plugin (obj);
   plugin.deregisterFileTranslator ("animImportUtil");
   return (MS::kSuccess);
}

