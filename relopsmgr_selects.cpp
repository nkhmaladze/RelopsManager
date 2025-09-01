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
 * This file contains the select interface implementation of RelOpsManager
 */


/**
 * @brief Runs the Select operation using the type of select given by the
 *    argument stype.
 *
 * @pre Input parameters rel_id is a valid relation id to be selected on,
 *    and fields and values have the same types.
 *
 * @param stype. SelectType indicating type of select (filescan, index)
 * @param rel_id. FileId corresponding to relation fileid being selected on
 * @param fields. Vector of fieldids corresponding to fields being selected on
 * @param comps. Vector of Comp corresponding to comparators for selection 
 * @param values. Vector of void * corresponding to values for selection
 * @param index_id. FileId with default value INVALID_FILE_ID corresponding to 
 *    index file id if indexscan is being used. 
 *
 * @return HeapFile * of the result file with results of the select.
 */
HeapFile *RelOpsManager::select(SelectType stype, FileId rel_id, 
                  std::vector<FieldId> fields, std::vector<Comp> comps,
                  std::vector<void *> values, 
                  FileId index_id){

  FileId res_id = this->_createResultFile(this->catalog->getSchema(rel_id));

  switch(stype) {
    case FileScanT: {
      FileScan *fscan = new FileScan(rel_id, res_id, fields, comps, values,
          this->catalog);
      fscan->runOperation();
      delete fscan;
      break;
     }
    case IndexT: {
      IndexScan *iscan = new IndexScan(rel_id, index_id, res_id, 
          fields, comps, values, this->catalog);
      iscan->runOperation();
      delete iscan;
      break;
    }
    default: throw;
  }
  return ((HeapFile *)this->catalog->getFile(res_id));

}