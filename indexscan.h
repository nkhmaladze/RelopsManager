#ifndef  _SWATDB_INDEXSCAN_H_
#define  _SWATDB_INDEXSCAN_H_

                                                                                
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
class HashIndexFile;
                                                                                
/**                                                                             
 * Select is an abstract class that lays the foundation for select operations   
 */                                                                             
class IndexScan : public Select {                                                
                                                                                
  public:                                                                       
    /** 
      * @brief Constructor for IndexScan select operation
      * 
      * @param rel_id. FileId  of the relation file.
      * @param index_id. FileId  of the index file.
      * @param result_id. FileId  of the result file. 
      * @param fields. Vector of field ids for the select operation.      
      * @param comps. Vector of Comps for the select operation.
      * @param values. Vector of Void * for the select operation.
      * @param catalog. Catalog * for SwatDB.      
      */     
    IndexScan(FileId rel_id, FileId index_id, FileId result_id, 
              std::vector<FieldId> fields, 
              std::vector<Comp> comps,
              std::vector<void *> values, Catalog *catalog); 

    /**
     * @brief Destructor for IndexScan. Delete dynamic member variables
     */
    ~IndexScan();
                                                                                
    /**                                                                         
     * @brief Runs the operation. This is a pure virtual method, and is         
     *    implemented in inherited classes. All child classes must implement    
     *    this method.                                                          
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with records that meet the criteria 
     *    of the operation.                                                     
     */                                                                         
    void runOperation();                                                        
                                                                                
  private:                                                                      
                                                                                
    /**
     * HashIndex * for the index file being selected on. 
     */
    HashIndexFile *index_file;
                                                                                
};     

#endif
