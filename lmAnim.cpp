
//-------------------------------------------------------------------------
//    lmAnim
//-------------------------------------------------------------------------
//
// lmAnim "show" "shot"
// RETURN
//  -hero (hro)             an array with the namespace and path of all hero animations
//  -pending (pnd)          an array with the namespace and path of all pending hero animations
//  -approvedVersion (apr)  version of the approved animation
//  -frames (frm)           animation frame range
//  -heroFrame (hfr)        hero frame
// EXECUTE
//  -promote (prm)          promote pending animation to hero animation
//

#define animApprovedFlag "-av"
#define animApprovedLongFlag "-approvedVersion"
#define animFramesFlag "-f"
#define animFramesLongFlag "-frames"
#define animHeroFlag "-h"
#define animHeroLongFlag "-hero"
#define animPendingFlag "-p"
#define animPendingLongFlag "-pending"
#define animPromoteFlag "-pr"
#define animPromoteLongFlag "-promote"
#define animCanPromoteFlag "-cpr"
#define animCanPromoteLongFlag "-canPromote"
#define animHeroFrameFlag "-hf"
#define animHeroFrameLongFlag "-heroFrame"

class lmAnim : public MPxCommand
{

public:

    MStatus         doIt( const MArgList& args );           

    static void*    creator();
    static          MSyntax newSyntax();

};

MSyntax lmAnim::newSyntax(){

    MSyntax syntax;
        syntax.addArg(MSyntax::kString);
        syntax.addArg(MSyntax::kString);
        syntax.addFlag(animHeroFlag,animHeroLongFlag);
        syntax.addFlag(animApprovedFlag,animApprovedLongFlag);
        syntax.addFlag(animPendingFlag,animPendingLongFlag);
        syntax.addFlag(animPromoteFlag,animPromoteLongFlag);
        syntax.addFlag(animCanPromoteFlag,animCanPromoteLongFlag);
        syntax.addFlag(animFramesFlag,animFramesLongFlag,MSyntax::kString);
        syntax.addFlag(animHeroFrameFlag,animHeroFrameLongFlag,MSyntax::kString);
       
        syntax.enableQuery(true);
    
    return syntax;
}

MStatus lmAnim::doIt( const MArgList& args )
{
    MArgDatabase argData(syntax(),args); 

    MString show, shot;
    // command arguments:
    //  option 1: lmAnim -flag show shot
    //  option 2: lmAnim -flag shot
    //  option 3: lmAnim -flag
    //
    if (argData.getCommandArgument(0,show) == MS::kFailure) {
        lmFilepath file ( MFileIO::currentFile() );
        if ( file.isValid() ) {
            show = file.project();
            shot = file.shot();
        }
        else {
            MGlobal::displayError("lmAnim: Error parsing arguments. Please provide a show and a shot to operate on.");
            return MS::kFailure;
        }
    }
    if ( !shot.length() && argData.getCommandArgument(1,shot) == MS::kFailure){
        // we arrived here because only one command argument was provided. this was placed in the show variable,
        // however, for ease of use, the single command arg variant uses just the shot
        lmFilepath file ( MFileIO::currentFile() );
        if ( file.isValid() ) {
            shot = show;
            show = file.project();
        }
        else {
            MGlobal::displayError("lmAnim: Error parsing arguments. Please provide a show and a shot to operate on.");
            return MS::kFailure;
        }
    }
                  
    // connect to db
    Connection conn(false);
    conn.connect(DB_NAME, DB_ADDRESS, DB_LOGIN, DB_PASS);
    if(!conn.connected()) {
        MGlobal::displayError("Database connection failed: " + MString(conn.error()));
        return MS::kFailure;
    }               
    cout << "connected" << endl;
  
    Query query = conn.query();
    Result result;
   
 
    //----------------------------------------------------------------------------
    // approved animation version
    if(argData.isFlagSet(animApprovedFlag))
    {                
        query << "SELECT ver_anim_luma FROM shots " <<
                "WHERE shot_name = '" << shot << "' " <<
                "AND id_project = '" << show << "'";
               
        cout << query.preview() << endl;        
        result = query.store();
       
        if(result.size())
            setResult((MString)result.at(0).at(0));
        else
            setResult("");
        return MS::kSuccess;
    }
    //----------------------------------------------------------------------------
    // hero animation
    if(argData.isFlagSet(animHeroFlag))
    {       
        query <<  "SELECT id_file, SUBSTRING(descriptor,2) "
            "FROM files, shots "
            "WHERE files.id_shot = shots.id_shot "
            "AND id_project = '" << show << "' "
            "AND shot_name = '" << shot << "' "
            "AND files.id_element = 'anim' "
            "AND version = 'hero' "
            "AND subdir = '_hero' "
            "ORDER BY descriptor";
               
        cout << query.preview() << endl;
        result = query.store();
       
        // store in array
        // namespace1, path1, namespace2, path2, ...
        MStringArray files;
        files.clear();
       
        Row row;
        Row::size_type i;
        for(i=0; row=result.at(i); i++)
        {
            //cout << row.at(0) << endl;
            lmFilepath file( conn, row.at(0));
            //cout << file << endl;
            if (file.isValid())
            {
                files.append( (MString)row.at(1) );
                files.append( file.fullpath() );
            }
            // else should we remove the entry?
        }
        setResult(files);
        return MS::kSuccess;
    }
    //----------------------------------------------------------------------------
    // pending animation
    if(argData.isFlagSet(animPendingFlag))
    {       
        query <<  "SELECT id_file, SUBSTRING(descriptor,2) "
            "FROM files, shots "
            "WHERE files.id_shot = shots.id_shot "
            "AND id_project = '" << show << "' "
            "AND shot_name = '" << shot << "' "
            "AND files.id_element = 'anim' "
            "AND version = 'hero' "
            "AND subdir = '_hero/pending' "
            "ORDER BY descriptor";
               
        cout << query.preview() << endl;
        result = query.store();
       
        // store in array
        // namespace1, path1, namespace2, path2, ...
        MStringArray files;
        Row row;
        Row::size_type i;
        for(i=0; row=result.at(i); i++)
        {
            //cout << row.at(0) << endl;
            lmFilepath file( conn, row.at(0));
            //cout << file << endl;
            if (file.isValid())
            {
                files.append( (MString)row.at(1) );
                files.append( file.fullpath() );
            }
            // else should we remove the entry?
        }
        setResult(files);
        return MS::kSuccess;
    }  
    //----------------------------------------------------------------------------
    // promote pending animation
    if(argData.isFlagSet(animPromoteFlag))
    {                      

        // get file, descriptor, and timestamp of pending animation file
        query <<  "SELECT id_file, descriptor, timestamp " <<
            "FROM files, shots " <<
            "WHERE files.id_shot = shots.id_shot " <<
            "AND id_project = '" << show << "' " <<
            "AND shot_name = '" << shot << "' " <<
            "AND files.id_element = 'anim' " <<
            "AND version = 'hero' " <<
            "AND subdir = '_hero/pending' " <<
            "ORDER BY descriptor,subdir DESC";
               
        cout << query.preview() << endl;
        Result pending = query.store();
        query.reset();       
       
        Row::size_type i;
        for(i=0; i < pending.size(); i++)
        {   
            // copy pending file over to hero

            lmFilepath pendFile( conn, (int)pending.at(i).at(0) );
            // make a duplicate of the pending file, but set the subdirectory to hero, instead of hero/pending
            lmFilepath apprFile = pendFile;
            apprFile.setSubdirs( "_hero");
           
            cout << pendFile.isValid() << endl;
            cout << pendFile.fullpath().asChar() << endl;
            cout << apprFile.fullpath().asChar() << endl;
            cout << (int)pending.at(i).at(0) << endl;
           
            // rename
            struct stat St;
            if (rename( pendFile.fullpath().asChar(), apprFile.fullpath().asChar() ) != 0 ) {
                MGlobal::displayError( "lmAnim: cannot move file from " + pendFile.fullpath() + " to " + apprFile.fullpath() );
                return MS::kFailure;
            }
           
            /*
            MString cache = pendFile.descriptor() + "_cache";
            // remove first period
            //cache = cache.substring(1, cache.length()-1);
              
            pendFile.setSubdirs( "_hero/pending/anim" + cache);
            apprFile.setSubdirs( "_hero/anim" + cache);
           
            cout << pendFile.path() << endl;
            cout << apprFile.path() << endl;
           
            // check if pending cache directory exists 
            if ( stat( pendFile.path().asChar(), &St ) == 0 ) {
                // pending cache dir exists
                // make approved cache dir
                if (mkdir (apprFile.path().asChar(), S_IRWXU|S_IRWXG ) != 0) {
                    MGlobal::displayError( "Error creating cache directory " + apprFile.path() );
                    return MS::kFailure;
                }
                // move cache
                if (rename( pendFile.path().asChar(), apprFile.path().asChar() ) != 0) {
                    MGlobal::displayError( "Error moving file " + pendFile.path() + " to " + apprFile.path() );
                    return MS::kFailure;
                }
            }
            */
           
           
            // find the id_file of the approved hero animaiton
            query <<  "SELECT id_file " <<
                "FROM files, shots " <<
                "WHERE files.id_shot = shots.id_shot " <<
                "AND id_project = '" << show << "' " <<
                "AND shot_name = '" << shot << "' " <<
                "AND files.id_element = 'anim' " <<
                "AND version = 'hero' " <<
                "AND descriptor = " << pending.at(i).at(1) << " " <<
                "AND subdir = '_hero' ";
                   
            cout << query.preview() << endl;
            Result apprvd = query.store();
            query.reset();
           
            // the hero table creates a relationship between two files, the source and the hero.
            // in the case of animation, this relationship is made for both the pending and approved hero files.
            // the goal here is to update the animation from pending to approved without changing the id_hero index
            // of the hero row, since it may have been used elsewhere.  also, we cannot simply delete the old hero file
            // and modify the pending file to be the new hero, bc deleting the old hero will cascade delete all the hisotry.
            // to do this update without the bad side effects, we update the existing hero entry
            // to have the properties of the pending entry, and then delete the pending file
            if (apprvd.size())
            {            
                // The hero table links a source file with a hero file, and also records the date and exporter.
                // In the case of animation, the table links hero files as well as pending hero files to their respective source files.
                // If the hero file already exists in the files database, then we simply need to insert its id into the hero export row
                // of the pending hero export.  The hero file will inherit the date and exporter of the pending hero
 
                // in the case of successive exports of the same version ( A01 -> A01 -> A01 ), we keep only the latest export in the history.  however,
                // if we revert to a previous version ( A01 -> B02 -> A01 ), then we want to reflect this in the history.
               
               
                // so, first we determine the source of the latest animation hero
                query << "SELECT id_file_source FROM heroes "
                "WHERE id_file_hero = '" << apprvd.at(0).at(0) << "' "
                "ORDER BY heroes.timestamp DESC LIMIT 1";
                cout << query.preview() << endl;       
                result=query.store();
                query.reset();
               
                if (result.size())
                {
                    // if the source and hero of the promoted anim are the same as a previous export and that export is the latest export
                    // (as determined above), we need to get rid of corresponding approved hero row,
                    // the query after this will will replace it, bringing with it its new timestamp and exporter
                    query << "DELETE heroes FROM heroes, heroes AS pend "
                    "WHERE heroes.id_file_hero = '" << apprvd.at(0).at(0) << "' "
                    "AND pend.id_file_hero = '" << pending.at(i).at(0) << "' "
                    "AND heroes.id_file_source=pend.id_file_source "
                    "AND heroes.id_file_source = '" << result.at(0).at(0) << "'";
                    cout << query.preview() << endl;       
                    query.execute();
                    query.reset();
                }
               
               
                // switch hero source file from pending to approved
                query << "UPDATE heroes SET id_file_hero = '" << apprvd.at(0).at(0) << "', timestamp=timestamp "
                "WHERE id_file_hero = '" << pending.at(i).at(0) << "'";
                cout << query.preview() << endl;       
                query.execute();
                query.reset();
               
                // update the timestamp of the old approved hero file to that of the new pending file
                query << "UPDATE files SET timestamp = " << pending.at(i).at(2) << " "
                "WHERE id_file = '" << apprvd.at(0).at(0) << "'";
                cout << query.preview() << endl;          
                query.execute();
                query.reset();
               
                // remove the pending file entry, bc we have now successufully transferred its properties to the
                // existing hero entry
                query << "DELETE FROM files WHERE id_file = '" << pending.at(i).at(0) << "'";
                cout << query.preview() << endl;         
                query.execute();
                query.reset();
               
               
               /*
                 // FIRST, we need to find out if an entry already exists in the heroes db for this hero/source combination
                query << "SELECT heroes.id_hero,heroes2.timestamp FROM heroes "
                    "LEFT JOIN heroes AS heroes2 ON heroes.id_file_source = heroes2.id_file_source "
                    "AND heroes.timestamp < heroes2.timestamp "
                    "WHERE heroes.id_file_hero = '" << apprvd.at(0).at(0) << "'";
                cout << query.preview() << endl;       
                Result result = query.store();
                query.reset();
               
                /////////////////////////////
                // subqueries: these two will be done as one when we upgrade
                // actually, come to think of it, the second update will just become an INSERT ON DUPLICATE KEY UPDATE
                // and we can ditch all the rest of this
                query << "SELECT id_hero_source,timestamp from heroes WHERE id_file_hero = " << pending.at(i).at(0);
                cout << query.preview() << endl;       
                Result result = query.store();
                query.reset();
               
                query << "SELECT id_hero from heroes WHERE id_file_source = " << result.at(0).at(0) << " "
                    "AND id_file_hero = " << apprvd.at(0).at(0);
                cout << query.preview() << endl;       
                Result result = query.store();
                query.reset();
                /////////////////////////////////////////
               
                if (result.size() && !result.at(0).at(1).is_null()) // this query should always return a result, but the timestamp will be null if there are no matches
                {
                    // if it does exist, then we'll get the timestamp and simply update our exising hero row.  when the pending
                    // anim file is deleted (below), the pending hero row will be deleted along with it due to cascading
                    query << "UPDATE heroes SET timestamp='" << result.at(0).at(1) << "' "
                    "WHERE id_hero = '" << result.at(0).at(0) << "'";
                    cout << query.preview() << endl;       
                    query.execute();
                    query.reset();
                }
                else
                {
                    // associate existing hero file with the source file that produced the new pending animation, maintaining
                    // the timestamp set when the pending hero was exported 
                    query << "UPDATE heroes SET id_file_hero = '" << apprvd.at(0).at(0) << "', timestamp=timestamp " <<
                    "WHERE id_file_hero = '" << pending.at(i).at(0) << "'";
                    cout << query.preview() << endl;       
                    query.execute();
                    query.reset();
                }
 
                // update the timestamp of the old approved hero file to that of the new pending file
                query << "UPDATE files SET timestamp = " << pending.at(i).at(2) << " " <<
                "WHERE id_file = '" << apprvd.at(0).at(0) << "'";
                cout << query.preview() << endl;          
                query.execute();
                query.reset();
               
                // remove the pending file entry, bc we have now successufully transferred its properties to the
                // existing hero entry
                query << "DELETE FROM files WHERE id_file = '" << pending.at(i).at(0) << "'";
                cout << query.preview() << endl;         
                query.execute();
                query.reset();
                */
            }
           
            // there is no existing animation hero file, change the file from pending to regular hero.
            // the id_file will stay the same, so the hero table will remain accurate
            else
            {
                // change the pending file entry to be a normal approved animation
                query << "UPDATE files SET subdir = '_hero', timestamp=timestamp " <<
                "WHERE id_file = ' " << pending.at(i).at(0) << "'";
                cout << query.preview() << endl;         
                query.execute();
                query.reset();
            }
            cout << (int)pending.at(i).at(0) << endl;
        }
        return MS::kSuccess;
    } 
    //----------------------------------------------------------------------------
    // frame range animation
    if(argData.isFlagSet(animFramesFlag))
    {
        if(argData.isQuery())
        {
            query << "SELECT anim_fr_range FROM shots "
            "WHERE shot_name = '" << shot << "' "
            "AND id_project = '" << show << "' ";
           
            cout << query.preview() << endl;
            result = query.store();
            query.reset();
           
            if(result.size())
                setResult((MString)result.at(0).at(0));
            else
                setResult("");
        }
        else
        {
            MString frames;
            if (argData.getFlagArgument(animFramesFlag,0,frames) == MS::kFailure)
                return MS::kFailure;
            query << "UPDATE shots SET anim_fr_range = '" << frames << "'";     
            cout << query.preview() << endl;
            query.execute();
            query.reset();
        }
        return MS::kSuccess;
    }
    //----------------------------------------------------------------------------
    // hero frame
    if(argData.isFlagSet(animHeroFrameFlag))
    {
        if(argData.isQuery())
        {
            cout << "query" << endl;
            query << "SELECT hero_frame FROM shots "
            "WHERE shot_name = '" << shot << "' "
            "AND id_project = '" << show << "' ";
           
            cout << query.preview() << endl;
            result = query.store();
            query.reset();
           
            if(result.size())
                setResult((MString)result.at(0).at(0));
            else
                setResult("");
        }
        else
        {
            MString frames;
            if (argData.getFlagArgument(animHeroFrameFlag,0,frames) == MS::kFailure)
                return MS::kFailure;
            query << "UPDATE shots SET hero_frame = '" << frames << "'";     
            cout << query.preview() << endl;
            query.execute();
            query.reset();
        }
        return MS::kSuccess;
    }
    //----------------------------------------------------------------------------
    // promotion element
    if(argData.isFlagSet(animCanPromoteFlag))
    {
        // first we need to determine if the user can promote pending animation from this shot
        // there is a column in the shots table which tells us which element is the promoter.
        // if it is NULL than any element can promote
       
        // this is weak protection because it is based on the file currently open, so a user could
        // promote animation of a different shot that he would normally be locked out of.  it is
        // up to the mel scripts to prevent this problem, and they currently only allow promotion of
        // animation for the currently open shot
        lmFilepath file ( MFileIO::currentFile() );
        if (!file.isValid()) {
            MGlobal::displayError("lmAnim: Cannot promote animation from an improperly named scene file.");
            return MS::kFailure;
        }
       
        query << "SELECT LOWER(id_elem_anim_promote) FROM shots "
            "WHERE shot_name='" << shot << "' "
            "AND id_project='" << show << "' ";
            //"AND (id_elem_anim_promote='" << file.element() << "' OR id_elem_anim_promote is NULL)";
        cout << query.preview() << endl;
        result = query.store();
        query.reset(); 
       
        // bail if thre is no result, or if the promotion element doesn't match our current element and it is also not null (null means any can promote)
        if (!result.size() )
        {
            MGlobal::displayError("lmAnim: Cannot find shot.");
            return MS::kFailure;
        }
       
        if ( (string)result.at(0).at(0)==(string)file.element().asChar() || result.at(0).at(0).is_null() )
        {
            setResult(1);
            return MS::kSuccess;
        }
       
        MGlobal::displayWarning("lmAnim: This shot's animation can only be promoted by " + (MString)result.at(0).at(0) + ".");
        setResult(0);
        return MS::kSuccess;
       
    }
   
    MGlobal::displayError("lmAnim: Unsupported Flag");
    return MS::kFailure;
           
}

void* lmAnim::creator() {
    return new lmAnim;
}

