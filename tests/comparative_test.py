#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

import sys
reload(sys)
sys.setdefaultencoding('utf8')


class TestComparativeResults(unittest2.TestCase):

  def test_comparer(self):

    # Выбираем обсчитанные звонки для двух итераций и сравниваем их между собой

    # Для получения сравниваемых диапазонов id,
    # берём две записи: с самым большим id и с самым маленьким id.
    # Делим каждую из них на миллион, формируем два сравниваемых диапазона.
    # Сравнение делаем по соответствующим записям.

    conn = psycopg2.connect(database='nispd_test', user='postgres')
    cur = conn.cursor()

    cur.execute('''
      SET search_path = calls_raw, pg_catalog;

      SELECT curr.cdr_id, prev.cdr_id, curr.server_id, curr.src_number, curr.dst_number, curr.orig,
        curr.billed_time, curr.rate, curr.destination_id,
        curr.pricelist_id, curr.prefix, curr.geo_id, curr.geo_operator_id, curr.mob, curr.geo_mob,
        curr.our, curr.operator_id,
        prev.cost, curr.cost,
        curr.cost - prev.cost  cost_diff,

        regions.name  region_name,
        destination.name  type_name,
        prev.rate    price_should,
        curr.rate    price_is,
        trunk.trunk_name  trunk_name,
        pricelist.name pricelist_name,
        curr.trunk_id

      FROM      calls_raw curr

      LEFT JOIN calls_raw prev

      ON     (prev.src_number = curr.src_number
        AND prev.dst_number = curr.dst_number
        AND prev.server_id = curr.server_id
        AND prev.orig = curr.orig
        
        AND prev.cdr_id < (SELECT cdr_id / 1000000 FROM calls_raw ORDER BY cdr_id DESC LIMIT 1) * 1000000)

      LEFT JOIN auth.trunk trunk
      ON (curr.trunk_id = trunk.id)
      LEFT JOIN auth.destination destination
      ON (curr.destination_id = destination.id)
      LEFT JOIN billing.instance_settings regions
      ON (curr.server_id = regions.id)
      LEFT JOIN voip.pricelist pricelist
      ON (curr.pricelist_id = pricelist.id)
              
      WHERE
        curr.cdr_id >=    (SELECT cdr_id / 1000000 FROM calls_raw ORDER BY cdr_id DESC LIMIT 1) * 1000000

      ORDER BY curr.id;
    ''')

    rows = cur.fetchall()

    costOk = True
    recordsOk = True
    hasRecords = False

    errorlog = ''

    reportTable = '<table style="width:100%%%%">%(header)s%(reportRows)s</table>'
    reportRowFormat = '<tr>\
        <th>%(Region)s</th>\
        <th>%(A)s</th>\
        <th>%(B)s</th>\
        <th>%(Trunk)s</th>\
        <th>%(Our)s</th>\
        <th>%(Orig/Term)s</th>\
        <th>%(Pricelist)s</th>\
        <th>%(Type)s</th>\
        <th>%(Prefix)s</th>\
        <th>%(Mobile)s</th>\
        <th>%(Price Should)s</th>\
        <th>%(Price Is)s</th>\
        <th>%(Cost Should)s</th>\
        <th>%(Cost Is)s</th>\
        <th>%(Cost Diff)s</th>\
      </tr>'

    reportHeader = reportRowFormat.replace('%(', '').replace(')s', '')
    reportTable = reportTable % {'header': reportHeader, 'reportRows': '%(reportRows)s'}

    reportRows = ''

    for curr_id, prev_id, region, src_number, dst_number, orig, billed_time, rate, dest_id, pricelist_id, \
        prefix, geo_id, geo_op_id, mob, geo_mob, our, op_id, cost_should, cost_is, cost_diff, \
        region_name, type_name, \
        price_should, price_is, trunk_name, pricelist_name, trunk_id in rows :

      # Состояние для одной записи.
      currOk = True

      hasRecords = True

      if curr_id is None or prev_id is None :
        errorlog += 'ERROR: Not all calls are synchronized for region ' + str(region) + '\n'
        recordsOk = False

      if cost_diff is not None and cost_diff > 0 :
        errorlog += 'ERROR: A call cost differs by %(cost_diff)f for region %(region)d with %(A)s %(orig)s %(B)s\n' % {
          'cost_diff': cost_diff, 'region': region, 'A': src_number, 'orig': '->' if orig else '<-', 'B': dst_number}
        costOk = False
        currOk = False

      if cost_diff is None or cost_is is None or abs(cost_is) < 0.0000001 :
        # Пропускаем нашу часть 7800-х - они бесплатные, разумеется.
        if str(dst_number).startswith('7800') and not orig and our:
          continue

        errorlog += 'ERROR: A call cost is absent or zero for region %(region)d with %(A)s %(orig)s %(B)s. Cost should be %(cost_should)s, pricelist %(pricelist_id)s\n' % {
          'region': region, 'A': src_number, 'orig': '->' if orig else '<-', 'B': dst_number,
          'cost_should': str(cost_should) if cost_should is not None else 'NULL',
          'pricelist_id': str(pricelist_id) if pricelist_id is not None else 'NULL'}
        costOk = False
        currOk = False

      if not currOk :
        reportRows += reportRowFormat % {
            'Region': region_name + ' (' + str(region) + ')',
            'A': str(src_number),
            'B': str(dst_number),
            'Trunk': trunk_name + ' (' + str(trunk_id) + ')',
            'Our': 'Yes' if our else 'No',
            'Orig/Term': 'Orig' if orig else 'Term',
            'Pricelist': (pricelist_name + ' (' + str(pricelist_id) + ')') if pricelist_id is not None else 'NULL',
            'Type': (type_name + ' (' + str(dest_id) + ')') if dest_id is not None else 'NULL',
            'Prefix': str(prefix) if prefix is not None else 'NULL',
            'Mobile': 'Mobile' if mob else 'Fixed',
            'Price Should': str(price_should) if price_should is not None else 'NULL',
            'Price Is': str(price_is) if price_is is not None else 'NULL',
            'Cost Should': str(cost_should) if cost_should is not None else 'NULL',
            'Cost Is': str(cost_is) if cost_is is not None else 'NULL',
            'Cost Diff': str(cost_diff) if cost_diff is not None else 'NULL'
          }

    if not hasRecords :
      errorlog += 'ERROR: Calls sync was not happened at all.\n'

    conn.close()

    msg = '\n\n' + (reportTable % {'reportRows': reportRows}) + '\n\n' + errorlog

    self.assertTrue(costOk and recordsOk and hasRecords, msg=msg)

# Результаты сравнения пишем в Jenkins, чтобы было видно в отчёте

if __name__ == '__main__':
  unittest2.main(testRunner=xmlrunner.XMLTestRunner(output='.'))

