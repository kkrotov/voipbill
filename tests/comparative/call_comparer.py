#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

import sys
reload(sys)
sys.setdefaultencoding('utf8')


class TestAuth(unittest2.TestCase):

    def loadData(self):

        conn = psycopg2.connect(database='nispd_test', user='postgres')
        cur = conn.cursor()

        cur.execute('''
      --SET search_path=tests;
      SELECT curr.id, prev.id, curr.date, prev.date, curr.answer, prev.answer

      FROM   tests.call_tests curr

      LEFT JOIN tests.call_tests prev

      ON (curr.id = prev.id AND curr.date != prev.date AND curr.date > prev.date)
      WHERE curr.answer != prev.answer;
    ''')

        rows = cur.fetchall()
        conn.close()
        return rows

    def checkData(self, rows):
        msg = ''
        noErrors = True
        for curr_id, prev_id, curr_date, prev_date, curr_answer, prev_answer in rows:
            msg += 'ERROR!\n'
            msg += 'prev_answer: \''
            msg += str(prev_answer)
            msg += '\'\ncurr_answer: \''
            msg += str (curr_answer)
            msg += ('\'\nid: \'')
            msg += str(prev_id)
            msg += ('\'\n============================\n')
            noErrors = False

        self.assertTrue (noErrors, msg=msg)

    def test_this(self):
        data = self.loadData()
        self.checkData(data)

# Результаты сравнения пишем в Jenkins, чтобы было видно в отчёте

if __name__ == '__main__':
    unittest2.main(testRunner=xmlrunner.XMLTestRunner(output='.'))
