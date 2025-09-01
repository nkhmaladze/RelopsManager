#ifndef  _SWATDB_PROJECT_H_
#define  _SWATDB_PROJECT_H_

/**
 * \file
 */

#include <string>
#include <vector>
#include "swatdb_types.h"
#include "operation.h"

class Catalog;
class Schema;
class File;
class RelationFile;
class HeapFile;
class HeapFileScanner;
class Record;
class Data;
class Key;

/**
 * Project is a derived class of operation that implements the 
 * project operation, which removes some number of columns from a
 * relation. 
 */
class Project : public Operation {

  public:

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
    Project(FileId rel_id, FileId result_id, std::vector<FieldId> fields,
           Catalog *catalog);

    /**
     * @brief Destructor for the project Operation. 
     */
    ~Project();

    /**                                                                         
     * @brief Runs the operation.                                              
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with all the records of the 
     *    original relation, with the new schema only consisting of fields 
     *    that were specified by passing them into the constructor. 
     */   
    void runOperation(); 

  protected:
    
    /**
     * Fields that will be kept following the execution of the 
     * project operation.  
     */
    std::vector<FieldId> fields;

    /*
     * fileState struct for the file project on 
     */
    fileState file_state;
    
};


#endif
