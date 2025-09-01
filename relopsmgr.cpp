#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <mutex>
#include "filemgr.h"
#include "catalog.h"
#include "swatdb_types.h"
#include "file.h"
#include "relopsmgr.h"
#include "swatdb_exceptions.h"
#include "heapfile.h"
#include "hashindexfile.h"
#include "heapfilescanner.h"
#include "blockheapfilescanner.h"
#include "hashindexscanner.h"
#include "record.h"
#include "relationfile.h"
#include "heapfile.h"
#include "heappage.h"
#include "heappagescanner.h"
#include "bufmgr.h"
#include "data.h"
#include "schema.h"
#include "key.h"
#include "searchkeyformat.h"
#include "operation.h"
#include "select.h"
#include "filescan.h"
#include "indexscan.h"
#include "join.h"
#include "tupleNLJ.h"
#include "indexNLJ.h"
#include "blockNLJ.h"
#include "hashjoin.h"
#include "parallelHashJoin.h"
#include "project.h"
#include "testingconfig.h"
#include "relopsmgr.h"

/**
 * SwatDB RelOpsManager Class.
 * The interface to the relational operators of the system:
 * manages relational operations on files.
 * This file contains the non-operation specific methods of RelOpsManager
 */


/**
 * @brief Creates SwatDB Relops layer, the interface to the relational 
 *    operations functionality of the DB.
 *
 *
 * @pre Input parameter object (file_mgr) are valid.
 *
 * @param file_mgr Pointer to the SwatDB FileManager.
 * @param catalog Pointer to the SwatDB Catalog.
 */
RelOpsManager::RelOpsManager(FileManager *file_mgr, BufferManager *buf_mgr, 
                             Catalog *catalog, const char *result_path){
  
  this->file_mgr = file_mgr;
  this->buf_mgr = buf_mgr;
  this->catalog = catalog;
  this->result_num = 0;
  if(result_path != NULL) { 
    testdb_path = result_path;
  }
  // printf("Debug: relation results will be stored in %s\n", testdb_path.c_str());
}

/**
 * @brief Checks if two files have identical contents (every record in
 *    file1 also exists in file2 with the exact same value). This function
 *    can be used to check the correctness of an operation test. The check is
 *    done by running a join on all columns and returning true if the
 *    resulting file has the same number of records as the original
 *    files. Note: this will only work if never files have any 
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
bool RelOpsManager::checkFilesEqual(FileId file1_id, FileId file2_id) {
  
  HeapFile *file1, *file2;
  
  file1 = (HeapFile *)this->file_mgr->getFile(file1_id);
  file2 = (HeapFile *)this->file_mgr->getFile(file2_id);

  if(file1->getNumRecs() != file2->getNumRecs()){
    return false;
  }
  
  Schema *file1_schema = this->catalog->getSchema(file1_id);
  Schema *file2_schema = this->catalog->getSchema(file2_id);
  if(file1_schema->field_list.size() != file2_schema->field_list.size()) { 
    return false;
  }
  std::vector<FieldId> field_list1 = {};
  std::vector<FieldId> field_list2 = {};
  for(long unsigned int i = 0; i < file1_schema->field_list.size(); i++) {
    field_list1.push_back(i); 
    field_list2.push_back(i); 
  }
  
  HeapFile *result = this->join(BlockNLJT, file1_id, file2_id,
                                field_list1, field_list2, 5);
  //the number of records in the result should be exactly equal to 
  // the number of records in both the input files if and only if
  // the two input files are exactly equal, since we are
  // joining on all columns
  if(file1->getNumRecs() != result->getNumRecs()){
    return false;
  }
  return true;
}


/**
 * Creates a result file with the schema given using result_num,
 * then increments result_num
 *
 * Helper function for code that creates result files for storing
 * the results of select, project, and join operations.
 *
 * @param schema: the schema for the file to create
 * @return fileId of result file created
 */
FileId RelOpsManager::_createResultFile(Schema *schema) {

  std::string filename = std::to_string(this->result_num) + "result";
  FileId res_id = this->file_mgr->createRelation(filename, schema, HeapFileT,
                              testdb_path + filename + ".rel", false);
  this->result_num++;
  return res_id;

}