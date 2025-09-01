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

SUITE(Projects) {

  TEST_FIXTURE(TestFixture, test1a) {

    std::vector<FieldId> fields = {1};
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->depts_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 4);
    
  }

  TEST_FIXTURE(TestFixture, test1b) {

    std::vector<FieldId> fields = {0, 1};
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->courses_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);
    
  }

  TEST_FIXTURE(TestFixture, test1c) {
    
    std::vector<FieldId> fields = {1, 4};
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->undergrads_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 50000);

  }

  TEST_FIXTURE(TestFixture, test1d) {

    std::vector<FieldId> fields = {1, 3};
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->profs_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 12);

  }
  
  TEST_FIXTURE(TestFixture, test1e) {

    std::vector<FieldId> fields = {0,1,2,3,4};
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->phds_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 400);

  }

}

SUITE(ExceptionTests) {
  TEST_FIXTURE(TestFixture, noFields) {
    CHECK_THROW(this->swatdb->getRelOpsMgr()->project(this->depts_file_id, {}),
        MismatchingFieldsRelOpsManager);
  }
}



/*
 * Prints usage
 */
void usage(){
  std::cout << "Usage: .projecttests -s <suite_name> -h help\n";
  std::cout << "Available Suites: " << "Projects, ExceptionTests " << std::endl;
}

/*
 * The main program either run all tests or tests a specific SUITE, given its
 * name via command line argument option 's'. 
 * If no argument is given by argument option 's', main runs all tests by
 * default.  
 * If invalid argument is given by option 's', 0 test is run
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
