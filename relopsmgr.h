#ifndef  _SWATDB_RELOPSMGR_H_
#define  _SWATDB_RELOPSMGR_H_



/**
 * \file
 */

#include <string>
#include <vector>
#include <mutex>
#include "swatdb_types.h"

class FileManager;
class Catalog;
class Schema;
class File;
class RelationFile;
class HeapFile;
class HeapPage;
class Page;
class HeapFileScanner;
class Record;
class Data;
class Key;

extern std::string relopsdir;

  // NOTE:  Do not modify this definition

/**
 * SwatDB RelOpsManager Class.
 * The interface to relational operators layer of the system:
 * manages relational operators.
 */
class RelOpsManager {

  public:

    /**
     * @brief Creates SwatDB Relops layer, the interface to the relational 
     *    operations functionality of the DB.
     *
     *
     * @pre Input parameter objects (file_mgr, buf_mgr, catalog) are 
     *    initialized and valid.
     *
     * @param file_mgr Pointer to the SwatDB FileManager.
     * @param buf_mgr Pointer to the SwatDB BufferManager.
     * @param catalog Pointer to the SwatDB Catalog.
     * @param result_path if not NULL, sets the file path for operator results
     */
    RelOpsManager(FileManager *file_mgr, BufferManager *buf_mgr, 
        Catalog *catalog, const char *result_path);

    /**
     * @brief Runs the Project operation 
     *
     * @pre Input parameters rel_id is a valid relation id to be projected on
     *
     * @param rel_id. FileId corresponding to relation fileid 
     *    being projected on
     * @param fields. Vector of fieldids corresponding to fields 
     *    being projected on`
     *
     * @throw MismatchingFieldsRelOpsManager if the fields parameter has
     *    invalid field Ids or is empty
     *
     * @return HeapFile * of the result file with results of the project.
     */
    HeapFile *project(FileId rel_id, std::vector<FieldId> fields);

    /**
     * @brief Runs the Select operation using the type of select given by the
     *    argument stype.
     *
     * @pre Input parameters rel_id is a valid relation id to be selected on,
     *    and fields and values have the same types.
     *
     * @param stype.  SelectType indicating type of select (filescan, index)
     * @param rel_id. FileId corresponding to relation fileid being selected on
     * @param fields. Vector of fieldids corresponding to fields being 
     *                selected on
     * @param comps.  Vector of Comp corresponding to comparators for selection 
     * @param values. Vector of void * corresponding to values for selection
     * @param index_id. FileId with default value INVALID_FILE_ID corresponding 
     *                  to index file id if indexscan is being used. 
     *
     * @return HeapFile * of the result file with results of the select.
     */
    HeapFile *select(SelectType stype, 
                     FileId rel_id, std::vector<FieldId> fields, 
                     std::vector<Comp> comps, std::vector<void *> values, 
                     FileId index_id = INVALID_FILE_ID);


    /**
     * @brief Runs the Join operation using the type of join given by the 
     *    argument jtype. This overloaded function only runs TupleNLJ and 
     *    BlockNLJ, the others are implemented in the other overloaded versions
     *
     * @pre Input parameters o_fid and i_fid are valid relation ids and fields
     *    and values align. 
     *
     * @param jtype. JoinType indicating type of join (tuplenlj, blocknlj, etc)
     * @param o_fid. FileId of the outer relation of the join. 
     * @param i_fid. FileId of the inner relation of the join. 
     * @param outer_field_ids. Vector of field ids for the outer relation.
     * @param inner_field_ids. Vector of field ids for the inner relation.
     * @param block_size. Uint32 indicating block size if block nlj is run. 
     *    This parameter is also used as the inner_index_id if the jtype is
     *    indexNLJ.
     */ 
    HeapFile *join(JoinType jtype, FileId o_fid, FileId i_fid, 
                   std::vector<FieldId> outer_field_ids, 
                   std::vector<FieldId> inner_field_ids,  
                   std::uint32_t block_size = 0);

 
    /**
     * @brief Runs the Join operation using the type of join given by the 
     *    argument jtype. This overloaded function only runs HashJoin and 
     *    ParallelHashJoin, the others are implemented in the other 
     *    overloaded versions
     *
     * @pre Input parameters o_fid and i_fid are valid relation ids and fields
     *    and values align. 
     *
     * @param jtype. JoinType indicating type of join (tuplenlj, blocknlj, etc)
     * @param o_fid. FileId of the outer relation of the join. 
     * @param i_fid. FileId of the inner relation of the join. 
     * @param outer_field_ids. Vector of field ids for the outer relation.
     * @param inner_field_ids. Vector of field ids for the inner relation.
     * @param num_buckets. Number of buckets used for hash joins..
     * @param num_threads. Number of threads for parallel hash join.
     */ 
    HeapFile *join(JoinType jtype, FileId o_fid, FileId i_fid,
                   std::uint32_t num_buckets, 
                   std::vector<FieldId> outer_field_ids, 
                   std::vector<FieldId> inner_field_ids, 
                   std::uint32_t num_threads = 0);

    /**
     * @brief Checks if two files have identical contents (every record in
     *    file1 also exists in file2 with the exact same value). This is 
     *    done by running a join on all columns and returning true if the
     *    resulting file has the same number of records as the original
     *    files. Note: this will only work if neither files have any 
     *    duplicates. 
     *
     * @pre Files have the same schema and no duplicate records. 
     *
     * @param file1_id. FileId of first file to be compared. 
     * @param file2_id. FileId of second file to be compared.
     *
     * @throw InvalidSchemaHeapFile if schemas do not match. 
     *
     * @return True if files are identical, False otherwise. 
     */
    bool checkFilesEqual(FileId file1_id, FileId file2_id);

    
   private:
    /*
     * Pointer to the file manager layer of the DBMS
     */
    FileManager *file_mgr;

    /*
     * Pointer to the buffer manager layer of the DBMS
     */
    BufferManager *buf_mgr;
    
    /*
     * Pointer to the Catalog of the DBMS
     */
    Catalog *catalog;

    /**
     * Stores the current result number
     */
    int result_num;

    /**
     * Creates a result file with the schema of the joined relations
     */
    FileId _createJoinRes(FileId outer_fid, FileId inner_fid, 
        std::vector<FieldId> o_fields, std::vector<FieldId> i_fields);
     
    /**
     * Creates a result file with the schema of rel_schema, but only the fields
     *    in the fields parameter. Used for the project operation. 
     *    * @throw MismatchingFieldsRelOpsManager if the fields parameter has
     *    invalid field Ids or is empty
     */
    FileId _createProjectRes(Schema *rel_schema, 
                                            std::vector<FieldId> fields);

    /**
     * Creates a result file with the schema given using result_num, 
     *    then increments result_num
     */
    FileId _createResultFile(Schema *schema);

};

#endif
