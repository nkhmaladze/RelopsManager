#include <string>
#include <vector>
#include <iostream>
#include "swatdb_types.h"
#include "filescan.h"
#include "data.h"
#include "key.h"
#include "record.h"
#include "heapfile.h"
#include "heapfilescanner.h"
#include "searchkeyformat.h"
#include "catalog.h"


    
/**
 * @brief Constructor for FileScan select operation.
 *
 * @param rel_id. File id of the relation file. 
 * @param result_id. File id of the result file. 
 * @param fields. Vector of field ids for the select operation. 
 * @param comps. Vector of Comps for the select operation.
 * @param values. Vector of Void * for the select operation. 
 */
FileScan::FileScan(FileId rel_id, FileId result_id, 
    std::vector<FieldId> fields, std::vector<Comp> comps, 
    std::vector<void *> values, Catalog *catalog) : Select(rel_id, result_id, 
    fields, comps, values, catalog) {

}


/**
 * @brief Destructor for FileScan select operation.
 *
 * @post File state for the relation and result file are cleaned up
 */
FileScan::~FileScan(){
}

/**                                                                         
 * @brief performs the file scan select operation. 
 *                                                                          
 * @pre Valid files and parameters have been passed to the contructor.      
 * @post Result file has been populated with records that meet the criteria 
 *    of the operation.                                                     
 */   
void FileScan::runOperation() {
  // get heapfile from file state
  HeapFile* file = (HeapFile *)this->file_state.file;
  // initialize scanner
  HeapFileScanner* scanner = new HeapFileScanner(file);
  Record *record = this->file_state.rec;
  
  // scan through the record and get record id
  while( true ){
    RecordId rid = scanner->getNext(record);
    if( rid == INVALID_RECORD_ID ) break;
    bool passes = true;
    // check if match the select conditions
    for( size_t i = 0; i < fields.size(); i ++ ){
      FieldId fieldId = fields[i];
      Comp comp = comps[i];
      void* val = values[i];
      if( !record->compareFieldToValue( fieldId, val, comp ) ){
        passes = false;
        break;
      }
    }
    // if passes insert Record
    if( passes ){
      ((HeapFile *)this->result_state.file)->insertRecord( *record );
    }
  }

  delete scanner;
}