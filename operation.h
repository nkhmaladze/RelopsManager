#ifndef _SWATDB_OPERATION_H_
#define _SWATDB_OPERATION_H_


/**
 * \file
 */

#include <string>
#include <vector>
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

  // NOTE:  Do not modify this struct

/**
 * Struct for storing file state associated with a relational operation.
 * One is created for the result file, and one (or more) are created for the
 * source files and indices. 
 */
struct fileState {
  /**
   * A pointer to the File   
   */
  File *file;
  /**
   * The file's FileId 
   */
  FileId fid;
  /**
   * Schema corresponding to file 
   */
  Schema *schema;
  /**
   * Temporary record space that can be used while scanning through file 
   */
  Record *rec;
  /**
   * RecordId for the temporary record
   */
  RecordId rid;
  /**
   * Temporary space for the search key for select and join comparisons  
   */
  Key *key;
};
/**
 * SwatDB Operation Class.
 * The interface to indivual relational operators operations the system:
 */


  // NOTE:  Do not modify this class definition

class Operation {

  public:
  
    /**
     * @brief Constructor for the Operation class. Because Operation is an
     * abstract class, an object cannot be created, but this constructor is
     * used by derived classes. 
     *
     * @pre result_id is the id of a valid newly created result file.
     *
     * @param result_name std::string of a newly created result file
     */
    Operation(FileId result_id, Catalog *catalog);

    /**
     * @brief Destructor for the Operation class. Cleans up dynamic memory in
     *    result state.
     */
    virtual ~Operation();

    /**
     * @brief Runs the operation. Other than initial setup in the 
     *    initialization, all the work of each operation is done in this 
     *    function. It is a pure virtual method, and is 
     *    implemented in inherited classes. All child classes must implement
     *    this method.
     *
     * @pre Valid files and parameters have been passed to the contructor. 
     * @post Result file has been populated with records that meet the criteria
     *    of the operation. 
     */
    virtual void runOperation() = 0;


  protected:
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
    void _initState(FileId file_id, 
    std::vector<FieldId> fields, fileState *state); 


    /**
    * @brief Deletes objects created in relop structs 
    *
    * @post Dynamic objects are deleted  
    * 
    * @param fileState struct to be deleted
    */
    void _delState(fileState *file_state); 


    /*
     * fileState struct for the result file
     */
    fileState result_state;


    /*
     * Catalog for SwatDB
     */
    Catalog *catalog;

};

#endif
