#ifndef _SWATDB_FILESCAN_H_
#define  _SWATDB_FILESCAN_H_

/**
 * \file
 */

#include <string>
#include <vector>
#include "swatdb_types.h"
#include "select.h"

class FileManager;
class Catalog;
class Schema;
class File;
class RelationFile;
class HeapFile;
class HeapFileScanner;
class Record;
class Data;
class Key;
class SearchKeyFormat;

/**
 * Select is an abstract class that lays the foundation for select operations
 */
class FileScan : public Select {

  public:
    
    /**
     * @brief Constructor for FileScan select operation.
     *
     * @param rel_id. File id for the relation file. 
     * @param result_id. File id for the result file
     * @param fields. Vector of field ids for the select operation. 
     * @param comps. Vector of Comps for the select operation.
     * @param values. Vector of Void * for the select operation. 
     */
    FileScan(FileId rel_id, FileId result_id, std::vector<FieldId> fields, 
        std::vector<Comp> comps, std::vector<void *> values, Catalog *catalog);

    /**
     * @brief Destructor for FileScan. Deletes dynamic member variables. 
     */
    ~FileScan();

    /**                                                                         
     * @brief Runs the filescan operation. All tuples of the relations 
     *    will be looped over and checked against the value and comparison
     *    conditions. For each tuple, if it passes these conditions, it 
     *    will be added to the result file.
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with records that meet the criteria 
     *    of the operation.                                                     
     */   
    void runOperation(); 

  private:
    

};

#endif
