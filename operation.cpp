#include <string>
#include <vector>
#include "swatdb_types.h"
#include "filemgr.h"
#include "operation.h"
#include "catalog.h"
#include "record.h"
#include "relationfile.h"
#include "data.h"
#include "schema.h"
#include "key.h"
#include "searchkeyformat.h"

/**
 * @brief Constructor for the Operation class. Because Operation is an abstract 
 *    class, an object cannot be created, but this constructor is used by 
 *    derived classes. 
 *
 * @pre result_id is the id of a valid newly created result file.
 *
 * @param result_name std::string of a newly created result file
 */
Operation::Operation(FileId result_id, Catalog *catalog){  
  this->catalog = catalog;
  this->_initState(result_id, {}, &this->result_state);
}

/**
 * @brief Destructor for the Operation class. Cleans up dynamic memory in
 *    result state.
 */
Operation::~Operation() {
  
  this->_delState(&result_state);
}


/**
 * @brief Performs the file and temporary record setup for relational 
 *    operators.  
 *
 * @pre A file exists that corresponds to the param rel_name
 * @post  Initializes the fileState struct (the state field) for 
 *        all operations.  Including setting the key if a search key 
 *        is needed for the operation.
 * 
 * @param file id for the file state
 * @param fields std::vector<FieldId> that stores the fields with which to 
 *    make the key
 * @param state relopState pointer that will be filled
*/
void Operation::_initState(FileId file_id, 
  std::vector<FieldId> fields, fileState *state){

  state->file = this->catalog->getFile(file_id);
  state->fid = file_id;
  state->schema = this->catalog->getSchema(state->fid);
  state->rec = new Record(state->schema);
  state->rid = INVALID_RECORD_ID;
  state->key = new Key();
  SearchKeyFormat *key_format = new SearchKeyFormat(fields, state->schema);
  Data *key_data = new Data(MAX_RECORD_SIZE);
  state->key->setKeyFormat(key_format);
  state->key->setKeyData(key_data);
}


/**
 * @brief Deletes objects created in relop structs 
 *
 * @post Dynamic objects are deleted  
 * 
 * @param relopState struct to be deleted
 */
void Operation::_delState(fileState *file_state){
  
  if(file_state != nullptr){
    if(file_state->rec != nullptr){
      delete file_state->rec->getRecordData();
      delete file_state->rec; 
      file_state->rec = nullptr;
    }
    if(file_state->key != nullptr) {
      if(file_state->key->getKeyData() != nullptr){
        delete file_state->key->getKeyData(); 
      }
      if(file_state->key->getKeyFormat() != nullptr){
        delete file_state->key->getKeyFormat(); 
      }
      delete file_state->key; 
      file_state->key = nullptr;
  
    }
  }
}