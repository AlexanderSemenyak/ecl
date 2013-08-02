/*
   Copyright (C) 2012  Statoil ASA, Norway. 
    
   The file 'ert_util_PATH_test.c' is part of ERT - Ensemble based Reservoir Tool. 
    
   ERT is free software: you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by 
   the Free Software Foundation, either version 3 of the License, or 
   (at your option) any later version. 
    
   ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.   
    
   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
   for more details. 
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <ert/util/test_util.h>
#include <ert/util/test_work_area.h>


void create_test_area(const char * test_name , bool store) {
  char * pre_cwd = util_alloc_cwd();
  test_work_area_type * work_area = test_work_area_alloc( test_name , store);
  char * work_path = util_alloc_string_copy( test_work_area_get_cwd( work_area ));
  
  test_assert_true( util_is_directory( work_path ));
  test_work_area_free( work_area );
  test_assert_bool_equal( store , util_entry_exists( work_path ));
  
  {
    char * post_cwd = util_alloc_cwd();
    test_assert_string_equal( pre_cwd , post_cwd );
    free( post_cwd );
  }
  free( pre_cwd );
  free( work_path );
}


void test_install_file_exists(const char * filename ) {
  char * abs_input_path = util_alloc_abs_path( filename );
  test_work_area_type * work_area = test_work_area_alloc( "FILE-TEST" , false);
  
  test_work_area_install_file( work_area , filename );
  test_assert_true( util_files_equal( abs_input_path , filename ));
  test_work_area_free( work_area );
  free( abs_input_path );
}


void test_copy_directory(const char * rel_path) {
  test_work_area_type * work_area = test_work_area_alloc( "FILE-TEST" , false);
  test_work_area_copy_directory( work_area , rel_path );
  test_assert_true( util_is_directory( rel_path ));
  test_work_area_free( work_area );
}


void test_input() {
  test_work_area_type * work_area = test_work_area_alloc( NULL , false );
  test_assert_NULL( work_area );
}


void test_create_with_prefix() {
  test_work_area_type * work_area = test_work_area_alloc( "AREA" , false );
  {
    uid_t uid = getuid();
    struct passwd * pw = getpwuid( uid );
    test_work_area_type * sub_area = test_work_area_alloc_with_prefix("sub" , "subName" , true);
    char * sub_path = util_alloc_sprintf( "%s/%s/%s/%s" , test_work_area_get_cwd( work_area ) , "sub" , pw->pw_name  , "subName");
    
    test_assert_true( util_is_directory( sub_path ));
    free( sub_path );
    test_work_area_free( sub_area );
  }
  test_work_area_free( work_area );
}

int main(int argc , char ** argv) {
  const char * rel_path_file = argv[1];
  const char * abs_path_file = argv[2];
  const char * rel_directory = argv[3];

  create_test_area("STORE-TEST" , true );
  create_test_area("DEL-TEST" , false);
  test_install_file_exists( rel_path_file );
  test_install_file_exists( abs_path_file );
  test_copy_directory( rel_directory );
  test_input();
  test_create_with_prefix();
  
  exit(0);
}
