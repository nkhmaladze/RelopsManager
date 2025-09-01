#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <algorithm>
#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>
#include <UnitTest++/TestRunner.h>

#define private public

#include "swatdb_exceptions.h"
#include "swatdb.h"
#include "diskmgr.h"
#include "bufmgr.h"
#include "page.h"
#include "catalog.h"
#include "file.h"
#include "filemgr.h"
#include "relopsmgr.h"
#include "record.h"
#include "schema.h"
#include "heapfile.h"
#include "hashindexfile.h"
#include "relationfile.h"
#include "data.h"
#include "heapfilescanner.h"
#include "operation.h"
#include "select.h"
#include "filescan.h"
#include "join.h"
#include "blockNLJ.h"
#include "indexNLJ.h"
#include "indexscan.h"
#include "tupleNLJ.h"
#include "hashjoin.h"
#include "parallelHashJoin.h"

#include "testerconf.h"

/*
 * 
 * Unit tests for RelopsMgr Class.
 */

/*
 * TestFixture Class for initializing and cleaning up objects. Any test called
 * with this class as TEST_FIXTURE has access to any public and protected data
 * members and methods of the object as if they were local variables and
 * helper functions of test functions. The constructor is called at the start
 * of the test to initialize the data members and destructor is called at the
 * end of the test to deallocate/free memory associated with the objects.
 * It is possible to declare another custom class and associate with tests via
 * TEST_FIXTURE. It is also possible to add more data members and functions.
 * Students are encouraged to do so as they see fit so lon as they are careful
 * not to cause any naming conflicts with other tests.
 */
class TestFixture{

  public:
    /*
     * Public data members that tests have access to as if they are local
     * variables.
     */
    SwatDB *swatdb;
    FileId file_id;
    FileId res_id;
    std::string file_name;
    std::vector<FieldEntry> field_list;
    Schema *schema;
    HeapFile *file;
    std::vector<std::vector<std::string>> data;
    FileId depts_file_id, courses_file_id, profs_file_id, undergrads_file_id,
           masters_file_id, phds_file_id, taking_file_id;
    std::string prof_index_file_name, name_index_file_name, gpa_index_file_name;
    


    TestFixture(){

      /*
       * Initializes all variables needed for testing. All DBMS objects are
       * created and initialized. A file, named "testrel.rel" is added through
       * the buf_mgr. Initializes a simple student schema and adds 5 records to 
       * the initial file. Creates a list of record_ids that were added to the
       * file. 
       */

      this->swatdb = new SwatDB(testdb_file, testdb_dir.c_str());
      this->swatdb->setSaveDB(testdb_save_file);
      depts_file_id = this->swatdb->getCatalog()->getFileId("departments");
      courses_file_id = this->swatdb->getCatalog()->getFileId("courses");
      profs_file_id = this->swatdb->getCatalog()->getFileId("profs");
      undergrads_file_id = 
        this->swatdb->getCatalog()->getFileId("undergrad_students");
      masters_file_id = 
        this->swatdb->getCatalog()->getFileId("masters_students");
      phds_file_id = this->swatdb->getCatalog()->getFileId("phd_students");
      taking_file_id = this->swatdb->getCatalog()->getFileId("is_taking");
      prof_index_file_name = "courses_prof_index";
      name_index_file_name = "undergrads_name_index";
      gpa_index_file_name = "phds_gpa_index";

    }

    /*
     * Clean up and deallocates all objects initilalized by the constructor.
     * The unix file is explicitly removed.
     */
    ~TestFixture(){

      delete swatdb;
    }

};


/**
 *
 */
SUITE(FileScanOneFieldEqual) {


  /**
   * FileScan Select Test on one string field
   */
  TEST_FIXTURE(TestFixture, stringtest){

    char course_name[6] = {'L','o','g','i','c','\0'};
    std::vector<FieldId> fields = {1};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {course_name};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        courses_file_id, fields, comps,values);
    std::cout << 
      "File Scan String Select Test - SELECT * FROM courses WHERE course_name"
      << " = Logic" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 1);

  }

  /**
   * FileScan Select Test on one string field on a bigger relation
   */
  TEST_FIXTURE(TestFixture, stringtestbig){

    char course_name[6] = {'H','e','n','r','y','\0'};
    std::vector<FieldId> fields = {1};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {&course_name};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        undergrads_file_id, fields, comps,values);
    std::cout << 
      "File Scan Int Select Test - SELECT * FROM masters WHERE name = Henry" 
      << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 3);
  }

  /**
   * FileScan Select Test on one int field
   */
  TEST_FIXTURE(TestFixture, inttest){

    int cs_dept_id = 2;
    std::vector<FieldId> fields = {3};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {&cs_dept_id};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        profs_file_id, fields, comps,values);
    std::cout << 
      "File Scan Int Select Test - SELECT * FROM professors WHERE dept_id = 2" 
      << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 3);
  }
  

  /**
   * FileScan Select Test on one float field
   */
  TEST_FIXTURE(TestFixture, floattest){

    float gpa = 3.9;
    std::vector<FieldId> fields = {4};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {&gpa};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
        undergrads_file_id, fields, comps,values);
    std::cout << 
      "File Scan Float Select Test - SELECT * FROM undergrads WHERE gpa = 3.9" 
      << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 2000);
  }

}

/**
 *
 */
SUITE(FileScanOneFieldRange) {

  /**
   * FileScan Select Test on one int field using range comparison 
   */ 
  TEST_FIXTURE(TestFixture, inttest){

    int cs_dept_id = 3;
    std::vector<FieldId> fields = {3};
    std::vector<Comp> comps = {GREATER_EQUAL};
    std::vector<void *> values = {&cs_dept_id};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
        profs_file_id, fields, comps,values);
    std::cout << 
      "File Scan Int Range Select Test - SELECT * FROM professors"
      << " WHERE dept_id >= 3" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());

    CHECK_EQUAL(result->getNumRecords(), 6);
  }


  /**
   * FileScan Select Test on one int field using range comparison
   * on a big relation
   */
  TEST_FIXTURE(TestFixture, inttestbig){

    int stud_id = 5;
    std::vector<FieldId> fields = {0};
    std::vector<Comp> comps = {LESS_EQUAL};
    std::vector<void *> values = {&stud_id};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        taking_file_id, fields, comps,values);

    std::cout << 
      "File Scan Int Select Test - SELECT * FROM is_taking WHERE stud_id "
      " <= 5" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);

  }


  /**
   * FileScan Select Test on one float field using range comparison 
   */
  TEST_FIXTURE(TestFixture, floattest){

    float gpa = 2.3;
    std::vector<FieldId> fields = {4};
    std::vector<Comp> comps = {LESS_EQUAL};
    std::vector<void *> values = {&gpa};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        undergrads_file_id, fields, comps,values);

    std::cout << 
      "File Scan Float Range Select Test - SELECT * FROM undergrads"
      << " WHERE gpa <= 2.3" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 6000);
  }

}

/**
 *
 */
SUITE(FileScanMultiField) {
  
  /**
   *
   */
  TEST_FIXTURE(TestFixture, small) {
    
    float gpa = 3.5;
    char stud_name[6] = {'H','e','n','r','y','\0'};
    std::vector<FieldId> fields = {4, 1}; 
    std::vector<Comp> comps = {LESS_EQUAL, EQUAL};
    std::vector<void *> values = {&gpa, &stud_name};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        undergrads_file_id, fields, comps,values);

    std::cout << 
      "File Scan Multi Field Select Test - SELECT * FROM undergrads"
      << " WHERE gpa <= 3.5 and name = Henry" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 2);
  }

}

/**
 * Tests functionality of relational operators manager index select
 */
SUITE(IndexSelectTests) {


  /**
   * Tests select functionality on an int comparison and prints the results
   */
  TEST_FIXTURE(TestFixture, basicIndexSelectInt){

    
    std::int32_t prof_id = 2;
    std::vector<FieldId> fields = {3};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {&prof_id};

    FileId ind_id = this->swatdb->getCatalog()->getFileId(
                        this->prof_index_file_name);

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(IndexT, 
                        courses_file_id, fields, comps, values, ind_id);


    std::cout << 
      "Index Scan Int Select Test - SELECT * FROM professors WHERE"
      << " prof_id = 2" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 2);
  }


  /**
   * Tests select functionality on a float comparison and prints the results
   */
  TEST_FIXTURE(TestFixture, basicIndexSelectFloat){

    float gpa = 4.0;
    int major_id = 3;
    std::vector<FieldId> fields = {3, 4};
    std::vector<Comp> comps = {EQUAL, EQUAL};
    std::vector<void *> values = {&major_id, &gpa};

    FileId ind_id = this->swatdb->getCatalog()->getFileId(
                        this->gpa_index_file_name);

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(IndexT, 
                        phds_file_id, fields, comps, values, ind_id);

    std::cout << "Index Scan Float Select Test - SELECT * FROM "
      << " phds WHERE gpa = 4.0 and major_dept_id = 3" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 4);
  }

   TEST_FIXTURE(TestFixture, basicIndexSelectString){

    char name[5] = {'J','a','c','k','\0'};
    std::vector<FieldId> fields = {1};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {name};

    FileId ind_id = this->swatdb->getCatalog()->getFileId(
        this->name_index_file_name);

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(IndexT, 
                        undergrads_file_id, fields, comps, values, ind_id);

    std::cout << "Index Scan String Select Test - SELECT * FROM "
      << "undergrads WHERE name = Jack" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 3);
  }

}

SUITE(ExceptionTests) {
  
  TEST_FIXTURE(TestFixture, InvalidFieldIdsTest) {
   
    int stud_id = 5;
    std::vector<FieldId> fields = {10};
    std::vector<Comp> comps = {LESS_EQUAL};
    std::vector<void *> values = {&stud_id};

    CHECK_THROW(this->swatdb->getRelOpsMgr()->select(FileScanT,
       taking_file_id, fields, comps,values), MismatchingFieldsRelOpsManager);

  }

  TEST_FIXTURE(TestFixture, MismatchingSizesFileScanTest) {
   
    int stud_id = 5;
    std::vector<FieldId> fields = {0, 1};
    std::vector<Comp> comps = {LESS_EQUAL};
    std::vector<void *> values = {&stud_id};

    CHECK_THROW(this->swatdb->getRelOpsMgr()->select(FileScanT,
       taking_file_id, fields, comps,values), MismatchingFieldsRelOpsManager);

  }

  TEST_FIXTURE(TestFixture, InvalidFileIdIndexScan) {

    char name[5] = {'J','a','c','k','\0'};
    std::vector<FieldId> fields = {1};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {name};

    CHECK_THROW(this->swatdb->getRelOpsMgr()->select(IndexT, 
          this->undergrads_file_id, fields, comps, values, 10000), 
        InvalidFileIdCat);
  }

  TEST_FIXTURE(TestFixture, MismatchingFieldsIndexScan) {
    
    float gpa = 4.0;
    int major_id = 3;
    int stud_id = 20;
    std::vector<FieldId> fields = {0, 3, 4};
    std::vector<Comp> comps = {EQUAL, EQUAL, EQUAL};
    std::vector<void *> values = {&stud_id, &major_id, &gpa};

    FileId ind_id = this->swatdb->getCatalog()->getFileId(
                        this->gpa_index_file_name);

    CHECK_THROW(this->swatdb->getRelOpsMgr()->select(IndexT, phds_file_id, 
          fields, comps, values, ind_id), MismatchingFieldsRelOpsManager);
  }

  TEST_FIXTURE(TestFixture, MismatchingFileIdsIndexScan) {
    
    float gpa = 4.0;
    int major_id = 3;
    std::vector<FieldId> fields = {3, 4};
    std::vector<Comp> comps = {EQUAL, EQUAL};
    std::vector<void *> values = {&major_id, &gpa};

    FileId ind_id = this->swatdb->getCatalog()->getFileId(
                        this->gpa_index_file_name);

    CHECK_THROW(this->swatdb->getRelOpsMgr()->select(IndexT, masters_file_id, 
          fields, comps, values, ind_id), InvalidFileIdRelOpsManager);
  }


}


/*
 * Prints usage
 */
void usage(){
  std::cout << "Usage: ./selecttests -s <suite_name> -h help\n";
  std::cout << "Available Suites: FileScanOneFieldEqual, " 
    << "FileScanOneFieldRange, FileScanMultiField, IndexSelectTests,"
    << "ExceptionTests" << std::endl;
}

/*
 * The main program either run all tests or tests a specific SUITE, given its
 * name via command line argument option 's'. If no argument is given by
 * argument option 's', main runs all tests by default. If invalid argument is
 * given by option 's', 0 test is run
 */
int main(int argc, char** argv){
  const char* suite_name;
  bool test_all = true;
  int c;

  //check for suite_name argument if provided
  while ((c = getopt (argc, argv, "hs:")) != -1){
    switch(c) {
      case 'h': usage();
                exit(1);
      case 's': suite_name = optarg;
                test_all  = false;
                break;
      default: printf("optopt: %c\n", optopt);

    }
  }

  //run all tests
  if (test_all){
    return UnitTest::RunAllTests();
  }

  //run the SUITE of the given suite name
  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);
  return runner.RunTestsIf(UnitTest::Test::GetTestList(), suite_name,
      UnitTest::True(), 0);
}
