/* Copyright (C) 2008-2009 Sun Microsystems, Inc

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <my_pthread.h>
#include <my_global.h>
#include <sql_priv.h>

#include <string.h>                             // strncpy
#include <pfs_instr_class.h>
#include <pfs_instr.h>
#include <pfs_global.h>
#include <tap.h>

void test_no_registration()
{
  int rc;
  PFS_sync_key key;
  PFS_thread_key thread_key;
  PFS_file_key file_key;
  PFS_mutex_class *mutex;
  PFS_rwlock_class *rwlock;
  PFS_cond_class *cond;
  PFS_thread_class *thread;
  PFS_file_class *file;
  PFS_table_share *table;

  rc= init_sync_class(0, 0, 0);
  ok(rc == 0, "zero init (sync)");
  rc= init_thread_class(0);
  ok(rc == 0, "zero init (thread)");
  rc= init_file_class(0);
  ok(rc == 0, "zero init (file)");
  rc= init_table_share(0);
  ok(rc == 0, "zero init (table)");

  key= register_mutex_class("FOO", 3, 0);
  ok(key == 0, "no mutex registered");
  key= register_mutex_class("BAR", 3, 0);
  ok(key == 0, "no mutex registered");
  key= register_mutex_class("FOO", 3, 0);
  ok(key == 0, "no mutex registered");

  key= register_rwlock_class("FOO", 3, 0);
  ok(key == 0, "no rwlock registered");
  key= register_rwlock_class("BAR", 3, 0);
  ok(key == 0, "no rwlock registered");
  key= register_rwlock_class("FOO", 3, 0);
  ok(key == 0, "no rwlock registered");

  key= register_cond_class("FOO", 3, 0);
  ok(key == 0, "no cond registered");
  key= register_cond_class("BAR", 3, 0);
  ok(key == 0, "no cond registered");
  key= register_cond_class("FOO", 3, 0);
  ok(key == 0, "no cond registered");

  thread_key= register_thread_class("FOO", 3, 0);
  ok(thread_key == 0, "no thread registered");
  thread_key= register_thread_class("BAR", 3, 0);
  ok(thread_key == 0, "no thread registered");
  thread_key= register_thread_class("FOO", 3, 0);
  ok(thread_key == 0, "no thread registered");

  file_key= register_file_class("FOO", 3, 0);
  ok(file_key == 0, "no file registered");
  file_key= register_file_class("BAR", 3, 0);
  ok(file_key == 0, "no file registered");
  file_key= register_file_class("FOO", 3, 0);
  ok(file_key == 0, "no file registered");

  PFS_thread fake_thread;
  fake_thread.m_table_share_hash_pins= NULL;

  table= find_or_create_table_share(& fake_thread, "foo_db", 6, "foo_table", 9);
  ok(table == NULL, "not created");
  table= find_or_create_table_share(& fake_thread, "bar_db", 6, "bar_table", 9);
  ok(table == NULL, "not created");
  table= find_or_create_table_share(& fake_thread, "foo_db", 6, "foo_table", 9);
  ok(table == NULL, "not created");

  mutex= find_mutex_class(0);
  ok(mutex == NULL, "no mutex key 0");
  mutex= find_mutex_class(1);
  ok(mutex == NULL, "no mutex key 1");
  mutex= find_mutex_class(9999);
  ok(mutex == NULL, "no mutex key 9999");

  rwlock= find_rwlock_class(0);
  ok(rwlock == NULL, "no rwlock key 0");
  rwlock= find_rwlock_class(1);
  ok(rwlock == NULL, "no rwlock key 1");
  rwlock= find_rwlock_class(9999);
  ok(rwlock == NULL, "no rwlock key 9999");

  cond= find_cond_class(0);
  ok(cond == NULL, "no cond key 0");
  cond= find_cond_class(1);
  ok(cond == NULL, "no cond key 1");
  cond= find_cond_class(9999);
  ok(cond == NULL, "no cond key 9999");

  thread= find_thread_class(0);
  ok(thread == NULL, "no thread key 0");
  thread= find_thread_class(1);
  ok(thread == NULL, "no thread key 1");
  thread= find_thread_class(9999);
  ok(thread == NULL, "no thread key 9999");

  file= find_file_class(0);
  ok(file == NULL, "no file key 0");
  file= find_file_class(1);
  ok(file == NULL, "no file key 1");
  file= find_file_class(9999);
  ok(file == NULL, "no file key 9999");

  cleanup_sync_class();
  cleanup_thread_class();
  cleanup_file_class();
  cleanup_table_share();
}

void test_mutex_registration()
{
  int rc;
  PFS_sync_key key;
  PFS_mutex_class *mutex;

  rc= init_sync_class(5, 0, 0);
  ok(rc == 0, "room for 5 mutex");

  key= register_mutex_class("FOO", 3, 0);
  ok(key == 1, "foo registered");
  key= register_mutex_class("BAR", 3, 0);
  ok(key == 2, "bar registered");
  key= register_mutex_class("FOO", 3, 0);
  ok(key == 1, "foo re registered");
  key= register_mutex_class("M-3", 3, 0);
  ok(key == 3, "M-3 registered");
  key= register_mutex_class("M-4", 3, 0);
  ok(key == 4, "M-4 registered");
  key= register_mutex_class("M-5", 3, 0);
  ok(key == 5, "M-5 registered");
  ok(mutex_class_lost == 0, "lost nothing");
  key= register_mutex_class("M-6", 3, 0);
  ok(key == 0, "M-6 not registered");
  ok(mutex_class_lost == 1, "lost 1 mutex");
  key= register_mutex_class("M-7", 3, 0);
  ok(key == 0, "M-7 not registered");
  ok(mutex_class_lost == 2, "lost 2 mutex");
  key= register_mutex_class("M-3", 3, 0);
  ok(key == 3, "M-3 re registered");
  ok(mutex_class_lost == 2, "lost 2 mutex");
  key= register_mutex_class("M-5", 3, 0);
  ok(key == 5, "M-5 re registered");
  ok(mutex_class_lost == 2, "lost 2 mutex");

  mutex= find_mutex_class(0);
  ok(mutex == NULL, "no key 0");
  mutex= find_mutex_class(3);
  ok(mutex != NULL, "found key 3");
  ok(strncmp(mutex->m_name, "M-3", 3) == 0, "key 3 is M-3");
  ok(mutex->m_name_length == 3, "name length 3");
  mutex= find_mutex_class(9999);
  ok(mutex == NULL, "no key 9999");

  cleanup_sync_class();
}

void test_rwlock_registration()
{
  int rc;
  PFS_sync_key key;
  PFS_rwlock_class *rwlock;

  rc= init_sync_class(0, 5, 0);
  ok(rc == 0, "room for 5 rwlock");

  key= register_rwlock_class("FOO", 3, 0);
  ok(key == 1, "foo registered");
  key= register_rwlock_class("BAR", 3, 0);
  ok(key == 2, "bar registered");
  key= register_rwlock_class("FOO", 3, 0);
  ok(key == 1, "foo re registered");
  key= register_rwlock_class("RW-3", 4, 0);
  ok(key == 3, "RW-3 registered");
  key= register_rwlock_class("RW-4", 4, 0);
  ok(key == 4, "RW-4 registered");
  key= register_rwlock_class("RW-5", 4, 0);
  ok(key == 5, "RW-5 registered");
  key= register_rwlock_class("RW-6", 4, 0);
  ok(key == 0, "RW-6 not registered");
  key= register_rwlock_class("RW-7", 4, 0);
  ok(key == 0, "RW-7 not registered");
  key= register_rwlock_class("RW-3", 4, 0);
  ok(key == 3, "RW-3 re registered");
  key= register_rwlock_class("RW-5", 4, 0);
  ok(key == 5, "RW-5 re registered");

  rwlock= find_rwlock_class(0);
  ok(rwlock == NULL, "no key 0");
  rwlock= find_rwlock_class(3);
  ok(rwlock != NULL, "found key 3");
  ok(strncmp(rwlock->m_name, "RW-3", 4) == 0, "key 3 is RW-3");
  ok(rwlock->m_name_length == 4, "name length 4");
  rwlock= find_rwlock_class(9999);
  ok(rwlock == NULL, "no key 9999");

  cleanup_sync_class();
}

void test_cond_registration()
{
  int rc;
  PFS_sync_key key;
  PFS_cond_class *cond;

  rc= init_sync_class(0, 0, 5);
  ok(rc == 0, "room for 5 cond");

  key= register_cond_class("FOO", 3, 0);
  ok(key == 1, "foo registered");
  key= register_cond_class("BAR", 3, 0);
  ok(key == 2, "bar registered");
  key= register_cond_class("FOO", 3, 0);
  ok(key == 1, "foo re registered");
  key= register_cond_class("C-3", 3, 0);
  ok(key == 3, "C-3 registered");
  key= register_cond_class("C-4", 3, 0);
  ok(key == 4, "C-4 registered");
  key= register_cond_class("C-5", 3, 0);
  ok(key == 5, "C-5 registered");
  key= register_cond_class("C-6", 3, 0);
  ok(key == 0, "C-6 not registered");
  key= register_cond_class("C-7", 3, 0);
  ok(key == 0, "C-7 not registered");
  key= register_cond_class("C-3", 3, 0);
  ok(key == 3, "C-3 re registered");
  key= register_cond_class("C-5", 3, 0);
  ok(key == 5, "C-5 re registered");

  cond= find_cond_class(0);
  ok(cond == NULL, "no key 0");
  cond= find_cond_class(3);
  ok(cond != NULL, "found key 3");
  ok(strncmp(cond->m_name, "C-3", 3) == 0, "key 3 is C-3");
  ok(cond->m_name_length == 3, "name length 3");
  cond= find_cond_class(9999);
  ok(cond == NULL, "no key 9999");

  cleanup_sync_class();
}

void test_thread_registration()
{
  int rc;
  PFS_thread_key key;
  PFS_thread_class *thread;

  rc= init_thread_class(5);
  ok(rc == 0, "room for 5 thread");

  key= register_thread_class("FOO", 3, 0);
  ok(key == 1, "foo registered");
  key= register_thread_class("BAR", 3, 0);
  ok(key == 2, "bar registered");
  key= register_thread_class("FOO", 3, 0);
  ok(key == 1, "foo re registered");
  key= register_thread_class("Thread-3", 8, 0);
  ok(key == 3, "Thread-3 registered");
  key= register_thread_class("Thread-4", 8, 0);
  ok(key == 4, "Thread-4 registered");
  key= register_thread_class("Thread-5", 8, 0);
  ok(key == 5, "Thread-5 registered");
  key= register_thread_class("Thread-6", 8, 0);
  ok(key == 0, "Thread-6 not registered");
  key= register_thread_class("Thread-7", 8, 0);
  ok(key == 0, "Thread-7 not registered");
  key= register_thread_class("Thread-3", 8, 0);
  ok(key == 3, "Thread-3 re registered");
  key= register_thread_class("Thread-5", 8, 0);
  ok(key == 5, "Thread-5 re registered");

  thread= find_thread_class(0);
  ok(thread == NULL, "no key 0");
  thread= find_thread_class(3);
  ok(thread != NULL, "found key 3");
  ok(strncmp(thread->m_name, "Thread-3", 8) == 0, "key 3 is Thread-3");
  ok(thread->m_name_length == 8, "name length 8");
  thread= find_thread_class(9999);
  ok(thread == NULL, "no key 9999");

  cleanup_thread_class();
}

void test_file_registration()
{
  int rc;
  PFS_file_key key;
  PFS_file_class *file;

  rc= init_file_class(5);
  ok(rc == 0, "room for 5 file");

  key= register_file_class("FOO", 3, 0);
  ok(key == 1, "foo registered");
  key= register_file_class("BAR", 3, 0);
  ok(key == 2, "bar registered");
  key= register_file_class("FOO", 3, 0);
  ok(key == 1, "foo re registered");
  key= register_file_class("File-3", 6, 0);
  ok(key == 3, "File-3 registered");
  key= register_file_class("File-4", 6, 0);
  ok(key == 4, "File-4 registered");
  key= register_file_class("File-5", 6, 0);
  ok(key == 5, "File-5 registered");
  key= register_file_class("File-6", 6, 0);
  ok(key == 0, "File-6 not registered");
  key= register_file_class("File-7", 6, 0);
  ok(key == 0, "File-7 not registered");
  key= register_file_class("File-3", 6, 0);
  ok(key == 3, "File-3 re registered");
  key= register_file_class("File-5", 6, 0);
  ok(key == 5, "File-5 re registered");

  file= find_file_class(0);
  ok(file == NULL, "no key 0");
  file= find_file_class(3);
  ok(file != NULL, "found key 3");
  ok(strncmp(file->m_name, "File-3", 6) == 0, "key 3 is File-3");
  ok(file->m_name_length == 6, "name length 6");
  file= find_file_class(9999);
  ok(file == NULL, "no key 9999");

  cleanup_file_class();
}

void test_table_registration()
{
  PFS_table_share *table_share;
  PFS_table_share *table_share_2;

  PFS_thread fake_thread;
  fake_thread.m_table_share_hash_pins= NULL;

  table_share_lost= 0;
  table_share= find_or_create_table_share(& fake_thread, "db1", 3, "t1", 2);
  ok(table_share == NULL, "not created");
  ok(table_share_lost == 1, "lost the table");

  table_share_lost= 0;
  init_table_share(5);
  init_table_share_hash();

  table_share= find_or_create_table_share(& fake_thread, "db1", 3, "t1", 2);
  ok(table_share != NULL, "created db1.t1");
  ok(table_share_lost == 0, "not lost");

  table_share_2= find_or_create_table_share(& fake_thread, "db1", 3, "t1", 2);
  ok(table_share_2 != NULL, "found db1.t1");
  ok(table_share_lost == 0, "not lost");
  ok(table_share == table_share_2, "same table");

  table_share_2= find_or_create_table_share(& fake_thread, "db1", 3, "t2", 2);
  ok(table_share_2 != NULL, "created db1.t2");
  ok(table_share_lost == 0, "not lost");

  table_share_2= find_or_create_table_share(& fake_thread, "db2", 3, "t1", 2);
  ok(table_share_2 != NULL, "created db2.t1");
  ok(table_share_lost == 0, "not lost");

  table_share_2= find_or_create_table_share(& fake_thread, "db2", 3, "t2", 2);
  ok(table_share_2 != NULL, "created db2.t2");
  ok(table_share_lost == 0, "not lost");

  table_share_2= find_or_create_table_share(& fake_thread, "db3", 3, "t3", 2);
  ok(table_share_2 != NULL, "created db3.t3");
  ok(table_share_lost == 0, "not lost");

  table_share_2= find_or_create_table_share(& fake_thread, "db4", 3, "t4", 2);
  ok(table_share_2 == NULL, "lost db4.t4");
  ok(table_share_lost == 1, "lost");

  table_share_lost= 0;
  table_share_2= find_or_create_table_share(& fake_thread, "db1", 3, "t2", 2);
  ok(table_share_2 != NULL, "found db1.t2");
  ok(table_share_lost == 0, "not lost");
  ok(strncmp(table_share_2->m_schema_name, "db1", 3) == 0 , "schema db1");
  ok(table_share_2->m_schema_name_length == 3, "length 3");
  ok(strncmp(table_share_2->m_table_name, "t2", 2) == 0 , "table t2");
  ok(table_share_2->m_table_name_length == 2, "length 2");

  cleanup_table_share_hash();
  cleanup_table_share();
}

void set_wait_stat(PFS_single_stat_chain *stat)
{
  stat->m_count= 12;
  stat->m_min= 5;
  stat->m_max= 120;
  stat->m_sum= 999;
}

bool is_empty_stat(PFS_single_stat_chain *stat)
{
  if (stat->m_count != 0)
    return false;
  if (stat->m_min != (ulonglong) -1)
    return false;
  if (stat->m_max != 0)
    return false;
  if (stat->m_sum != 0)
    return false;
  return true;
}

void test_instruments_reset()
{
  int rc;
  PFS_sync_key key;
  PFS_file_key file_key;
  PFS_mutex_class *mutex_1;
  PFS_mutex_class *mutex_2;
  PFS_mutex_class *mutex_3;
  PFS_rwlock_class *rwlock_1;
  PFS_rwlock_class *rwlock_2;
  PFS_rwlock_class *rwlock_3;
  PFS_cond_class *cond_1;
  PFS_cond_class *cond_2;
  PFS_cond_class *cond_3;
  PFS_file_class *file_1;
  PFS_file_class *file_2;
  PFS_file_class *file_3;

  rc= init_sync_class(3, 3, 3);
  ok(rc == 0, "init (sync)");
  rc= init_thread_class(3);
  ok(rc == 0, "init (thread)");
  rc= init_file_class(3);
  ok(rc == 0, "init (file)");

  key= register_mutex_class("M-1", 3, 0);
  ok(key == 1, "mutex registered");
  key= register_mutex_class("M-2", 3, 0);
  ok(key == 2, "mutex registered");
  key= register_mutex_class("M-3", 3, 0);
  ok(key == 3, "mutex registered");

  key= register_rwlock_class("RW-1", 4, 0);
  ok(key == 1, "rwlock registered");
  key= register_rwlock_class("RW-2", 4, 0);
  ok(key == 2, "rwlock registered");
  key= register_rwlock_class("RW-3", 4, 0);
  ok(key == 3, "rwlock registered");

  key= register_cond_class("C-1", 3, 0);
  ok(key == 1, "cond registered");
  key= register_cond_class("C-2", 3, 0);
  ok(key == 2, "cond registered");
  key= register_cond_class("C-3", 3, 0);
  ok(key == 3, "cond registered");

  file_key= register_file_class("F-1", 3, 0);
  ok(file_key == 1, "file registered");
  file_key= register_file_class("F-2", 3, 0);
  ok(file_key == 2, "file registered");
  file_key= register_file_class("F-3", 3, 0);
  ok(file_key == 3, "file registered");

  mutex_1= find_mutex_class(1);
  ok(mutex_1 != NULL, "mutex key 1");
  mutex_2= find_mutex_class(2);
  ok(mutex_2 != NULL, "mutex key 2");
  mutex_3= find_mutex_class(3);
  ok(mutex_3 != NULL, "mutex key 3");

  rwlock_1= find_rwlock_class(1);
  ok(rwlock_1 != NULL, "rwlock key 1");
  rwlock_2= find_rwlock_class(2);
  ok(rwlock_2 != NULL, "rwlock key 2");
  rwlock_3= find_rwlock_class(3);
  ok(rwlock_3 != NULL, "rwlock key 3");

  cond_1= find_cond_class(1);
  ok(cond_1 != NULL, "cond key 1");
  cond_2= find_cond_class(2);
  ok(cond_2 != NULL, "cond key 2");
  cond_3= find_cond_class(3);
  ok(cond_3 != NULL, "cond key 3");

  file_1= find_file_class(1);
  ok(file_1 != NULL, "file key 1");
  file_2= find_file_class(2);
  ok(file_2 != NULL, "file key 2");
  file_3= find_file_class(3);
  ok(file_3 != NULL, "file key 3");

  set_wait_stat(& mutex_1->m_wait_stat);
  set_wait_stat(& mutex_2->m_wait_stat);
  set_wait_stat(& mutex_3->m_wait_stat);
  set_wait_stat(& rwlock_1->m_wait_stat);
  set_wait_stat(& rwlock_2->m_wait_stat);
  set_wait_stat(& rwlock_3->m_wait_stat);
  set_wait_stat(& cond_1->m_wait_stat);
  set_wait_stat(& cond_2->m_wait_stat);
  set_wait_stat(& cond_3->m_wait_stat);
  set_wait_stat(& file_1->m_wait_stat);
  set_wait_stat(& file_2->m_wait_stat);
  set_wait_stat(& file_3->m_wait_stat);

  ok(! is_empty_stat(& mutex_1->m_wait_stat), "mutex_1 stat is populated");
  ok(! is_empty_stat(& mutex_2->m_wait_stat), "mutex_2 stat is populated");
  ok(! is_empty_stat(& mutex_3->m_wait_stat), "mutex_3 stat is populated");
  ok(! is_empty_stat(& rwlock_1->m_wait_stat), "rwlock_1 stat is populated");
  ok(! is_empty_stat(& rwlock_2->m_wait_stat), "rwlock_2 stat is populated");
  ok(! is_empty_stat(& rwlock_3->m_wait_stat), "rwlock_3 stat is populated");
  ok(! is_empty_stat(& cond_1->m_wait_stat), "cond_1 stat is populated");
  ok(! is_empty_stat(& cond_2->m_wait_stat), "cond_2 stat is populated");
  ok(! is_empty_stat(& cond_3->m_wait_stat), "cond_3 stat is populated");
  ok(! is_empty_stat(& file_1->m_wait_stat), "file_1 stat is populated");
  ok(! is_empty_stat(& file_2->m_wait_stat), "file_2 stat is populated");
  ok(! is_empty_stat(& file_3->m_wait_stat), "file_3 stat is populated");

  reset_instrument_class_waits();

  ok(is_empty_stat(& mutex_1->m_wait_stat), "mutex_1 stat is cleared");
  ok(is_empty_stat(& mutex_2->m_wait_stat), "mutex_2 stat is cleared");
  ok(is_empty_stat(& mutex_3->m_wait_stat), "mutex_3 stat is cleared");
  ok(is_empty_stat(& rwlock_1->m_wait_stat), "rwlock_1 stat is cleared");
  ok(is_empty_stat(& rwlock_2->m_wait_stat), "rwlock_2 stat is cleared");
  ok(is_empty_stat(& rwlock_3->m_wait_stat), "rwlock_3 stat is cleared");
  ok(is_empty_stat(& cond_1->m_wait_stat), "cond_1 stat is cleared");
  ok(is_empty_stat(& cond_2->m_wait_stat), "cond_2 stat is cleared");
  ok(is_empty_stat(& cond_3->m_wait_stat), "cond_3 stat is cleared");
  ok(is_empty_stat(& file_1->m_wait_stat), "file_1 stat is cleared");
  ok(is_empty_stat(& file_2->m_wait_stat), "file_2 stat is cleared");
  ok(is_empty_stat(& file_3->m_wait_stat), "file_3 stat is cleared");

  cleanup_sync_class();
  cleanup_file_class();
}

void do_all_tests()
{
  test_no_registration();
  test_mutex_registration();
  test_rwlock_registration();
  test_cond_registration();
  test_thread_registration();
  test_file_registration();
  test_table_registration();
  test_instruments_reset();
}

int main(int, char **)
{
  plan(196);
  MY_INIT("pfs_instr_info-t");
  do_all_tests();
  return 0;
}
