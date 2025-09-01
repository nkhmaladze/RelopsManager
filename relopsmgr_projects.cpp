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

/**
 * SwatDB RelOpsManager Class.
 * The interface to the relational operators of the system:
 * manages relational operations on files.
 * This file contains the project interface of RelOpsManager
 */


/**
 * @brief Runs the Project operation 
 *
 * @pre Input parameters rel_id is a valid relation id to be projected on
 *
 * @param rel_id. FileId corresponding to relation fileid being projected on
 * @param fields. Vector of fieldids corresponding to fields being projected on
 *
 * @return HeapFile * of the result file with results of the project.
 *
 * @throw MismatchingFieldsRelOpsManager if the fields parameter has
 *    invalid field Ids or is empty
 */
HeapFile *RelOpsManager::project(FileId rel_id, std::vector<FieldId> fields) {
  
  FileId res_id;
  Project *project;

  res_id = this->_createProjectRes(this->catalog->getSchema(rel_id), fields);
  project = new Project(rel_id, res_id, fields, this->catalog);
  project->runOperation();

  delete project;

  return ((HeapFile *)this->catalog->getFile(res_id));
}


/**
 * Creates a result file with the schema of rel_schema, but only the fields
 * in the fields parameter. Used for the project operation. 
 *
 * @throw MismatchingFieldsRelOpsManager if the fields parameter has
 *    invalid field Ids or is empty
 */
FileId RelOpsManager::_createProjectRes(Schema *rel_schema, 
    std::vector<FieldId> fields) 
{
 /*
  *  (1) create schema for project result from passed rel_schema and fields
  *  (2) call _createResultFile method of this class to create the result file
  * Note:
  *  * RelOpsManager is a friend class of Schema, thus can directly access 
  *    rel_schema->field_list in this method.
  *  * Each field's fieldID value is its position in this list: field 0, 1, ...
  */

  if( fields.empty() ) throw MismatchingFieldsRelOpsManager();

  for( FieldId fid : fields ){ 
    if( fid >= rel_schema->field_list.size() ) throw MismatchingFieldsRelOpsManager();
  }

  std::vector<FieldEntry> new_field_list;
  std::vector<std::string> new_primary_key;
  
  for( FieldId fid : fields ){
    new_field_list.push_back( rel_schema->field_list[fid] );
  }

  Schema *new_schema = new Schema(new_field_list, new_primary_key);

  FileId ans = this->_createResultFile( new_schema );
  return ans;

  // TODO: change return value
  //return INVALID_FILE_ID;
}

