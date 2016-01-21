#!/bin/python
# -*- coding: utf-8 -*-

import xmlrunner
import unittest2
import psycopg2

import sys
reload(sys)
sys.setdefaultencoding('utf8')

class TestComparativeResults(unittest2.TestCase):
  def test_voiprouting_comparer(self):

    # Выбираем обсчитанные звонки для двух итераций и сравниваем их между собой

    # Для получения сравниваемых диапазонов id,
    # берём две записи: с самым большим id и с самым маленьким id.
    # Делим каждую из них на миллион, формируем два сравниваемых диапазона.
    # Сравнение делаем по соответствующим записям.

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

    conn.close()

    self.assertTrue(recordsOk and hasRecords)


  def test_billing_comparer(self):

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
        <td>%(Region)s</td>\
        <td>%(A)s</td>\
        <td>%(B)s</td>\
        <td>%(Trunk)s</td>\
        <td>%(Our)s</td>\
        <td>%(Orig/Term)s</td>\
        <td>%(Pricelist)s</td>\
        <td>%(Type)s</td>\
        <td>%(Prefix)s</td>\
        <td>%(Mobile)s</td>\
        <td>%(Price Should)s</td>\
        <td>%(Price Is)s</td>\
        <td>%(Cost Should)s</td>\
        <td>%(Cost Is)s</td>\
        <td>%(Cost Diff)s</td>\
      </tr>'

    reportHeader = reportRowFormat.replace('%(', '').replace(')s', '').replace('td>', 'th>')
    reportTable = reportTable % {'header': reportHeader, 'reportRows': '%(reportRows)s'}

    reportRows = ''

    for curr_id, prev_id, region, src_number, dst_number, orig, billed_time, rate, dest_id, pricelist_id, \
        prefix, geo_id, geo_op_id, mob, geo_mob, our, op_id, cost_should, cost_is, cost_diff, \
        region_name, type_name, \
        price_should, price_is, trunk_name, pricelist_name, trunk_id in rows :

      # Состояние для одной записи.
      currOk = True

      hasRecords = True
      
      # Пока не залит прайс Ростелекома для Владивостока и не обновлён прайс Ростелекома для Новосибирска,
      # эти номера добавляем в "чёрный список" (криво специально, чтобы выпилить):
      if region == 89 and str(src_number) == '74232060498' and str(dst_number) == '74232080075' :
        continue
      if region == 89 and str(src_number) == '74232060490' and str(dst_number) == '74232080075' :
        continue
      if region == 94 and str(src_number) == '73833120493' and str(dst_number) == '73833833000' :
        continue
      if region == 94 and str(src_number) == '73833120496' and str(dst_number) == '73833833000' :
        continue

      if curr_id is None or prev_id is None :
        errorlog += 'ERROR: Not all calls are synchronized for region ' + str(region) + '\n'
        recordsOk = False

      if cost_diff is not None and cost_diff > 0 :
        errorlog += 'ERROR: A call cost differs by %(cost_diff)f for region %(region)d with %(A)s %(orig)s %(B)s\n' % {
          'cost_diff': cost_diff, 'region': region, 'A': src_number, 'orig': '->' if orig else '<-', 'B': dst_number}
        costOk = False
        currOk = False

      if cost_diff is None or cost_is is None or abs(cost_is) < 0.0000001 :
        # Пропускаем терминацию наших звонков на наши транки - кроме 800-х.
        if not str(dst_number).startswith('7800') and not orig and our:
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
