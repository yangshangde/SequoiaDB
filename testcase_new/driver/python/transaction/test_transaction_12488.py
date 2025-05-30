# @decription: test rollback transation
# @testlink:   seqDB-12488
# @interface:  transaction_begin(self)
#              transaction_rollback(self)
# @author:     liuxiaoxuan 2017-9-09

import unittest
import datetime
from pysequoiadb.error import (SDBTypeError, SDBBaseError, SDBEndOfCursor, SDBError)
from lib import testlib

insert_nums = 100
class TestTransaction12488(testlib.SdbTestBase):
    def test_transaction_12488(self):
        # begin to do transaction
        self.begin_transaction()
        # do create cl
        testlib.drop_cs(self.db, self.cs_name, ignore_not_exist=True)
        self.cs = self.db.create_collection_space(self.cs_name)
        self.cl = self.cs.create_collection(self.cl_name)
        # insert
        self.insert_datas()

        # update
        rule = {'$set': {'b': 'update'}}
        condition = {'a': {'$gt': 11, '$lt': 20}}
        self.update_datas(rule,condition)

        # remove
        condition = {'a': {'$gte': 20}}
        self.remove_datas(condition)

        # check data before rollback transaction
        condition = {'a': {'$gt': 11}}
        expectResult = []
        for i in range(12, 20):
            expectResult.append({"a": i, "b": "update"})
        self.check_result( expectResult,condition)

        # do rollback
        self.rollback_transaction()

        # check rollback result
        expectResult = []
        self.check_result(expectResult)

    def tearDown(self):
      self.db.drop_collection_space(self.cs_name)   

    def begin_transaction(self):
       try:
          self.db.transaction_begin()
       except SDBBaseError as e:
          self.fail('begin transaction fail: ' + str(e))

    def insert_datas(self):
       doc = []
       for i in range(0, insert_nums):
          doc.append({"a": i , "b": "test" + str(i)})
       try:
          flags = 0
          self.cl.bulk_insert(flags, doc)
       except SDBBaseError as e:
          self.fail('insert fail: ' + str(e))

    def update_datas(self,rule,cond):
        try:
            self.cl.update(rule, condition = cond)
        except SDBBaseError as e:
            self.fail('update fail: ' + str(e))

    def remove_datas(self,cond):
        try:
            self.cl.delete(condition = cond)
        except SDBBaseError as e:
            self.fail('remove fail: ' + str(e))

    def rollback_transaction(self):
        try:
            self.db.transaction_rollback()
        except SDBBaseError as e:
            self.fail('rollback transaction fail: ' + str(e))

    def check_result(self,expectRec,cond = None):
       try:
          if cond == None:
              cursor = self.cl.query(order_by = {"_id": 1})
          else:
              cursor = self.cl.query(condition = cond, order_by = {"_id": 1})

          actRec = testlib.get_all_records_noid(cursor)
          # check result
          self.assertListEqualUnordered(expectRec, actRec)			 
       except SDBBaseError as e:
          self.fail('check result fail: ' + str(e))
