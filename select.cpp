#include <string>
#include <vector>
#include <iostream>
#include "swatdb_types.h"
#include "swatdb_exceptions.h"
#include "schema.h"
#include "select.h"
#include "operation.h"
#include "catalog.h"


/**
 * @brief Constructor for Select operation. Both FileScan and IndexScan use
 *    this constructor.
 *
 * @param rel_id. FileId of the relation file. 
 * @param result_id. FileId of the result file.  F
 * @param fields. Vector of FieldIds for the select operation.
 *                Note: the fieldId values are positions/indexes of the fields
 * @param comps. Vector of Comps for the select operation.
 * @param catalog. pointer to the catalog of swatdb
 *
 * @throw MismatchingFieldsRelOpsManager if the fields parameter has 
 *    incorrect fields or if there are mismatching numbers of fields, 
 *    values, or comps
 *
 * @pre file_name is the name of a valid file.
 * @post private variables have been set accordingly.
 */
Select::Select(FileId rel_id, FileId result_id, std::vector<FieldId> fields, 
       std::vector<Comp> comps, std::vector<void *> values, Catalog *catalog) : 
       Operation(result_id, catalog) {

  this->_initState(rel_id, fields, &file_state);
  this->fields = fields;
  this->comps = comps;
  this->values = values;

  // size checking
  if(this->fields.size() != this->comps.size()){
      this->_delState(&this->file_state);
      throw MismatchingFieldsRelOpsManager();
    }
  if(this->fields.size() != this->values.size()){
      this->_delState(&this->file_state);
      throw MismatchingFieldsRelOpsManager();
    }
  if(this->comps.size() != this->values.size()){
      this->_delState(&this->file_state);
      throw MismatchingFieldsRelOpsManager();
    }

  Schema* s = this->file_state.schema;
  uint32_t size = s->field_list.size();
  
  // check if fields exist
  for( FieldId fid : fields ){
    if( fid >= size ){
      throw MismatchingFieldsRelOpsManager();
    }
  }
}

/**
 * @brief Destructor for the Select Operation.
 */
Select::~Select() {
  this->_delState(&this->file_state);
}