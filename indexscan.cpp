#include <string>
#include <vector>
#include "swatdb_exceptions.h"
#include "swatdb_types.h"
#include "indexscan.h"
#include "key.h"
#include "data.h"
#include "record.h"
#include "heapfile.h"
#include "catalog.h"
#include "heapfilescanner.h"
#include "searchkeyformat.h"
#include "hashindexscanner.h"
#include "hashindexfile.h"

/**                                                                         
 * @brief Constructor for IndexScan select operation.                       
 *                                                                          
 * @param rel_id. FileId  of the relation file.                              
 * @param index_id. FileId  of the index file.                              
 * @param result_id. FileId  of the result file.                            
 * @param fields. Vector of field ids for the select operation.             
 * @param comps. Vector of Comps for the select operation.                  
 * @param values. Vector of Void * for the select operation.                
 * @param catalog. Catalog * for SwatDB. 
 *
 * @throw InvalidFileIdRelOpsManager if the rel_id does not match the 
 *    relation id for the index_id given. 
 * @throw MismatchingFieldsRelOpsManager if the fields selected on do 
 *    not match the fields the index is on. 
 */                                                                         
IndexScan::IndexScan(FileId rel_id, FileId index_id, FileId result_id, 
                          std::vector<FieldId> fields, std::vector<Comp> comps,
                          std::vector<void *> values, Catalog *catalog) : 
                          Select(rel_id, result_id, fields, comps, values, 
                                 catalog) {


  std::vector<FieldId> indexFields;
  this->index_file = (HashIndexFile*)catalog->getFile(index_id);

  // check if index_file exist
  if (this->index_file == nullptr) {
    throw InvalidFileIdRelOpsManager();
  }
  // check fileid
  FileId expected_rel_id = catalog->getRelationFileId(index_id);
  if (expected_rel_id != rel_id) {
    throw InvalidFileIdRelOpsManager();
  }

  SearchKeyFormat* format = this->index_file->getKeyFormat();
  std::vector<FieldId> ids_fields = format->getFieldList();
  // check if fields match
  if( ids_fields != fields ){
    throw MismatchingFieldsRelOpsManager();
  }
  
}

IndexScan::~IndexScan(){
}
    
/**                                                                             
 * @brief Runs the index scan select operation. 
 *                                                                              
 * @pre Valid files and parameters have been passed to the contructor.          
 * @post Result file has been populated with records that meet the criteria     
 *    of the operation.                                                         
 */        
void IndexScan::runOperation() {
  // create key
  Key *key_val = new Key(MAX_RECORD_SIZE);
  SearchKeyFormat *key_format = index_file->getKeyFormat();
  key_val->setKeyFormat(key_format);
  key_val->setKeyFromValues(this->values);
  // init scanner
  HashIndexScanner scanner(index_file, key_val);

  Record *rec = file_state.rec;
  HeapFile* file = (HeapFile *)this->file_state.file;
  HeapFile* out = (HeapFile *)this->result_state.file;

  RecordId rid;
  while( ( rid = scanner.getNext() ) != INVALID_RECORD_ID ){
    // fetch full record from rid
    file->getRecord(rid, rec);
    // check conditions
    bool passes = true;
    for( size_t i = 0; i < fields.size(); ++i ){
      if( !rec->compareFieldToValue( fields[i], values[i], comps[i] ) ){
        passes = false;
        break;
      }
    }
    // insert record
    if (passes) out->insertRecord(*rec);
  }
  
  delete key_val;
}