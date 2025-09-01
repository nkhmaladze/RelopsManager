#ifndef  _SWATDB_SELECT_H_
#define  _SWATDB_SELECT_H_

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
 * Select is an abstract class that lays the foundation for select operations
 */
class Select : public Operation {

  public:

    /**
     * @brief Constructor for Select operation. Both FileScan and IndexScan use
     *    this constructor.
     *
     * @param rel_id. FileId of the relation file.
     * @param result_id. FileId of the result file. 
     * @param fields. Vector of field ids for the select operation. 
     *         Note: the fieldId values are positions/indexes of the fields
     * @param comps. Vector of Comps for the select operation.
     * @param values. Vector of Void * for the select operation. 
     * @param catalog. pointer to the catalog of SwatDB
     *
     * @pre file_name is the name of a valid file. 
     * @post private variables have been set accordingly.
     *
     * @throw MismatchingFieldsRelOpsManager if the fields parameter has
     *    incorrect fields or if there are mismatching numbers of fields,
     *    values, or comps
     */
    Select(FileId rel_id, FileId result_id, std::vector<FieldId> fields,
           std::vector<Comp> comps, std::vector<void *> values, 
           Catalog *catalog);

    /**
     * @brief Destructor for the Select Operation. 
     */
    ~Select();

  protected:
    
    /**
     * The positions of the following three vectors are lined up, so index 0
     * of each are one conjunct of the select, index 1 is the next, and so on.
     */
    /**
     * Fields used in the select operation
     */
    std::vector<FieldId> fields;

    /**
     * comparators used in the select operation
     */
    std::vector<Comp> comps;

    /**
     * Values used in the select operation. Type is void * because this could
     * be a mix of float, int, and char * types. 
     */
    std::vector<void *> values;

    /*
     * fileState struct for the file selected on 
     */
    fileState file_state;
    
};

#endif
