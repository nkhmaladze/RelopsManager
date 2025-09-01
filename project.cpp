#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include "swatdb_types.h"
#include "project.h"
#include "operation.h"
#include "catalog.h"
#include "data.h"
#include "heapfilescanner.h"
#include "record.h"
#include "key.h"
#include "heapfile.h"


/**
 * @brief Constructor for Project operation. 
 *
 * @param rel_id. FileId of the relation file.
 * @param result_id. FileId of the result file. 
 * @param fields. Vector of field ids for the project operation. 
 * @param catalog. pointer to the catalog of SwatDB
 *
 * @pre rel_id is the name of a valid file. 
 * @post private variables have been set accordingly.
 */
Project::Project(FileId rel_id, FileId result_id, std::vector<FieldId> fields,
        Catalog *catalog) : Operation(result_id, catalog) {

  this->_initState(rel_id, fields, &file_state);
  this->fields = fields; 
  
}

/**
 * @brief Destructor for the project Operation. 
 */
Project::~Project(){
  this->_delState(&this->file_state);
}

/**                                                                         
 * @brief Runs the operation.                                              
 *                                                                          
 * @pre Valid files and parameters have been passed to the contructor.      
 * @post Result file has been populated with all the records of the 
 *    original relation, with the new schema only consisting of fields 
 *    that were specified by passing them into the constructor. 
 */   
void Project::runOperation() {
  // initialize scanner
  HeapFile* file = (HeapFile *)this->file_state.file;
  HeapFileScanner* scanner = new HeapFileScanner(file);
  Record *record = this->file_state.rec;  
  Record *dest = this->result_state.rec;

  while( true ){
    RecordId rid = scanner->getNext(record);
    if( rid == INVALID_RECORD_ID ) break;
    record->setRecordFromRecord( this->fields, dest );
    ((HeapFile *)this->result_state.file)->insertRecord( *dest );
  }

  delete scanner;
}