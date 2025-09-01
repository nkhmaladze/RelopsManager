#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <algorithm> //for std::find
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
    FileId depts_file_id, studs_file_id;


    TestFixture(){

      /*
       * Initializes all variables needed for testing. All DBMS objects are
       * created and initialized. A file, named "testrel.rel" is added through
       * the buf_mgr. Initializes a simple student schema and adds 5 records to 
       * the initial file. Creates a list of record_ids that were added to the
       * file. 
       */

      this->swatdb = new SwatDB(smalldb_file, testdb_dir.c_str());
      this->swatdb->setSaveDB(smalldb_save_file);
      depts_file_id = this->swatdb->getCatalog()->getFileId("smalldepartments");
      studs_file_id = this->swatdb->getCatalog()->getFileId("smallstudents");

      // uncomment to print out DB info:
      /*
      printf("+++++ Catalog info: ++++++++++++++++\n");
      this->swatdb->cat->printCatInfo();
      std::cout << "++++++ Relation Files ++++++++++++++++\n";
      this->swatdb->getFileMgr()->printFile(depts_file_id);
      this->swatdb->getFileMgr()->printFile(studs_file_id);
      std::cout << "+++++++++++++++++++++++++++++++++++++++\n";
      */
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
SUITE(Select) {
  /**
   * FileScan Select Test on one int field, equal operator
   */
  TEST_FIXTURE(TestFixture, inttest){

    int cs_dept_id = 2;

    // FieldId values are the field's position (index)
    // this is a select on the 3rd field in the relation
    // comparing it for equality (EQUAL) to the value of cs_dept_id 
    // (see the cout stmt below for the SQL version of this)
    std::vector<FieldId> fields = {3};
    std::vector<Comp> comps = {EQUAL};
    std::vector<void *> values = {&cs_dept_id};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        studs_file_id, fields, comps,values);
    std::cout << 
      "File Scan Select Test - SELECT * FROM smallstudents WHERE dept_id = 2" 
      << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 6);
  }

  /**
   * FileScan Select Test on string field using non-equality operator
   */
  TEST_FIXTURE(TestFixture, stringField){

    char stud_name[6] = {'J','o','e','\0', '\0', '\0'};
    std::vector<FieldId> fields = {1};
    std::vector<Comp> comps = {GREATER};
    std::vector<void *> values = {&stud_name};

    HeapFile *result = this->swatdb->getRelOpsMgr()->select(FileScanT, 
                        studs_file_id, fields, comps, values);
    std::cout << 
      "File Scan Select Test - SELECT * FROM smallstudents WHERE name > Joe" 
      << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 11);
  }

}

SUITE(Projects) {
    
  TEST_FIXTURE(TestFixture, project1) {
    std::vector<FieldId> fields = {1,2};
    std::cout << 
      "Project Test - SELECT name, email FROM smallstudents";
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->studs_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);
  } 

  TEST_FIXTURE(TestFixture, project2) {
    std::vector<FieldId> fields = {3};
    std::cout << 
      "Project Test - SELECT major_dept_id FROM smallstudents" ;
    HeapFile *result = 
      this->swatdb->getRelOpsMgr()->project(this->studs_file_id, fields);
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);
  }

}

/*
 * Prints usage
 */
void usage(){
  std::cout << "Usage: ./smalltests -s <suite_name> -h help\n";
  std::cout << "Available Suites: Project, Select" << std::endl;
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
