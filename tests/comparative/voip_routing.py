#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

import sys
reload(sys)
sys.setdefaultencoding('utf8')

class TestComparativeVoipRouting(unittest2.TestCase):

    # Выбираем обсчитанные звонки для двух итераций и сравниваем их между собой

    # Для получения сравниваемых диапазонов id,
    # берём две записи: с самым большим id и с самым маленьким id.
    # Делим каждую из них на миллион, формируем два сравниваемых диапазона.
    # Сравнение делаем по соответствующим записям.

    def loadCalls(self):

        conn = psycopg2.connect(database='nispd_test', user='postgres')
        cur = conn.cursor()

        cur.execute('''
      SET search_path = tests, pg_catalog;

      SELECT curr.region_id, curr.src_number, curr.dst_number,
             curr.route_case, curr.debug,
             prev.route_case, prev.debug
      FROM      voiprouting_tests curr

      LEFT JOIN voiprouting_tests prev

      ON     (prev.src_number = curr.src_number
        AND prev.dst_number = curr.dst_number
        AND prev.region_id = curr.region_id

        AND prev.sampler_id < (SELECT sampler_id / 1000000 FROM voiprouting_tests ORDER BY sampler_id DESC LIMIT 1) * 1000000)

      WHERE
        curr.sampler_id >=    (SELECT sampler_id / 1000000 FROM voiprouting_tests ORDER BY sampler_id DESC LIMIT 1) * 1000000

      ORDER BY curr.region_id, curr.src_number, curr.dst_number;
    ''')

        rows = cur.fetchall()
        conn.close()
        return rows

    def checkCalls(self, rows):
        recordsOk = True
        hasRecords = False

        for region, src_number, dst_number, curr_rc, curr_full, prev_rc, prev_full in rows :
            hasRecords = True
            if prev_rc != curr_rc :
                # Баги того, что маршрутизация поменялась.
                recordsOk = False
                print region, src_number, dst_number, curr_rc, curr_full, prev_rc, prev_full

            if prev_rc == 'RESULT|RELEASE REASON|NO_ROUTE_TO_DESTINATION' :
                # Добавляем ошибки маршрутизации
                print region, src_number, dst_number, curr_rc, curr_full, prev_rc, prev_full

        if not hasRecords :
            print 'ERROR: VoIP Routing test not happened at all.\n'


        self.assertTrue(recordsOk and hasRecords)

    def runTest(self):
        calls = self.loadCalls()
        self.checkCalls(calls)


if __name__ == '__main__':
    unittest2.main()
