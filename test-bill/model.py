#!/usr/bin/python
# -*- coding: utf-8 -*-

from sql_access import *
from config import *

class NispdDatabase(object):
    def __init__(self, connstr):
        self.db = DbWrap(connstr)

        # tables cleared by clear() method
        self.dirtyTables = {}
        self.dirtyTablesStackNumber = 0

    def clear(self):
        query = ''
        dellist = {}
        for table, stacknum in self.dirtyTables.iteritems():
            dellist[stacknum] = table

        for n in reversed(range(self.dirtyTablesStackNumber)):
            query = query + 'DELETE FROM ' + dellist[n] + ';'

        dst = self.db.cursor()
        dst.execute(query)
        self.dirtyTablesStackNumber = 0
        self.dirtyTables = {}

    def markDirtyTable(self, name):
        if name not in self.dirtyTables:
            self.dirtyTables[name] = self.dirtyTablesStackNumber
            self.dirtyTablesStackNumber += 1

    def insertGeoPrefix(self, prefix, areaCodePrefix, mob, dest, geo_id = None, geo_operator_id = None):
        '''
        Rossvyaz' mapping:
            prefix - phone no. part to classify:
                - local, when prefix matches an instance_settings.city_prefix list item;
                - regional, when 'region' field matches an instance_settings.region list item;
                - Russia, Worldwide, SNG otherwise (specified by 'dest' field).
            geo_id - primary key corresponding to nispd.geo.geo.id (Rossvyaz' loaded area name).
            geo_operator_id - primary key corresponding to nispd.geo.operator.id (Rossvyaz' loaded operator name).
            dest - classifier: 1 - Russia, 2 - Worldwide, 3 - SNG.
            mob - mobile phone flag.
            areaCodePrefix - region prefix to classify as regional for corresponding instances.
        '''
        if not geo_id:
            geo_id = prefix

        dst = self.db.cursor()
        dst.execute("""INSERT INTO geo.prefix (prefix, geo_id, mob, dest, region, operator_id) VALUES (%s, %s, %s, %s, %s, %s);
                       SELECT billing.mark_sync('geo_prefix',0);""",
            (prefix, geo_id, mob, dest, areaCodePrefix, geo_operator_id) )
        self.markDirtyTable('geo.prefix')

    def insertVoipOperatorGroup(self):
        dst = self.db.cursor()
        dst.execute("""INSERT INTO voip.operator_group(id, name) VALUES (1, 'prime')""" )
        self.markDirtyTable('voip.operator_group')

    def insertInstanceSettings(self):
        '''
        id - filial code.
        region_id - regional prefixes for branch city (area code prefixes).
        city_prefix - local prefixes for branch city.
        '''
        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.instance_settings(id, region_id, city_prefix) VALUES (99, '{7495,7495496}', '{7495,7498,7499}')""" )
        self.markDirtyTable('billing.instance_settings')

    def insertVoipOperator(self, branchCityCode, operator_id, term_in_cost,
                           pricelist_id, local_network_pricelist_id, client_7800_pricelist_id, operator_7800_pricelist_id):
        dst = self.db.cursor()
        dst.execute("""INSERT INTO voip.operator(
                        region, id, short_name, name, group_id, term_in_cost, term_out_cost, 
                        term_out_local_cost, default_pricelist_id, rossvyaz_operator_id, 
                        pricelist_id, local_network_id, local_network_pricelist_id, client_7800_pricelist_id, 
                        operator_7800_pricelist_id, minimum_payment, stat_client_card_id)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)""",
            (branchCityCode, operator_id, 'op_'+str(branchCityCode)+'_'+str(operator_id),
             'op_'+str(branchCityCode)+'_'+str(operator_id), 1, term_in_cost, 0.0, 0.0, None, None, 
                        pricelist_id, None, local_network_pricelist_id, client_7800_pricelist_id, 
                        operator_7800_pricelist_id, None, None) )
        self.markDirtyTable('voip.operator')

    def insertVoipPricelist(self, branchCityCode, operator_id, name, tariffication_by_minutes, tariffication_full_first_minute,
                            pricelist_type, initiate_mgmn_cost, initiate_zona_cost):
        insertedPricelistId = 0    
        dst = self.db.cursor()
        dst.execute("""INSERT INTO voip.pricelist(region, operator_id, name, currency_id, correct,
                       tariffication_by_minutes, tariffication_full_first_minute, type, initiate_mgmn_cost, initiate_zona_cost)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s) RETURNING id""",
            (branchCityCode, operator_id, name, 1, 0.0,
                       tariffication_by_minutes, tariffication_full_first_minute, pricelist_type, initiate_mgmn_cost, initiate_zona_cost) )
        insertedPricelistId = dst.fetchone()[0]
        self.markDirtyTable('voip.pricelist')

        return insertedPricelistId

    def insertNetworkPrefix(self, prefix, instance_id, operator_id, network_type_id):
        '''
        Local city operator's prices:
            prefix - corresponding prefix for price.
            instance_id - filial for which the prefix is local.
            operator_id - local operator id.
            network_type_id - classifier from network_type (corresponding to 'ndef' field in billing.defs).
        '''

        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.network_prefix
            (prefix, instance_id, operator_id, date_from, deleted, date_to, network_type_id) VALUES (%s, %s, %s, %s, %s, %s, %s);
            SELECT billing.mark_sync('network_prefix',0);""",
            (prefix, instance_id, operator_id, '2013-01-01', False, None, network_type_id) )
        self.markDirtyTable('billing.network_prefix')

    def insertPrice(self, defcode, pricelist_id, price):
        ndef = long(defcode)

        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.defs(defcode, ndef, pricelist_id, date_from, deleted, date_to, price)
                       VALUES (%s, %s, %s, %s, %s, %s, %s); SELECT billing.mark_sync('defs',0);""",
            (defcode, ndef, pricelist_id, '2013-01-01', False, None, price) )
        self.markDirtyTable('billing.defs')

    def insertTarif(self, tarif_id, freemin, freemin_for_number, pricelist_id, 
                    paid_redirect, tariffication_by_minutes, tariffication_full_first_minute, 
                    tariffication_free_first_seconds):
        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.tarif(id, name, currency_id, status, freemin,
                        freemin_for_number, pricelist_id, paid_redirect, tariffication_by_minutes,
                        tariffication_full_first_minute, tariffication_free_first_seconds)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)""",
            (tarif_id, 'Tarif_' + str(tarif_id), 1, 'public', freemin, freemin_for_number, pricelist_id, 
                paid_redirect, tariffication_by_minutes, tariffication_full_first_minute, 
                tariffication_free_first_seconds) )
        self.markDirtyTable('billing.tarif')

    def insertClient(self, client_id, voip_limit_month, voip_limit_day, voip_disabled, credit, balance, amount_date, last_payed_month):
        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.clients(
                        id, voip_limit_month, voip_limit_day, voip_disabled, credit, 
                        balance, amount_date, last_payed_month)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s)""",
            (client_id, voip_limit_month, voip_limit_day, voip_disabled, credit, balance, amount_date, last_payed_month) )
        self.markDirtyTable('billing.clients')

    def insertClientPhoneWithTarif(self, change_id, branchCityCode, client_id, usage_id, phone_num, no_of_lines,
                tarif_id_local, tarif_id_local_mob, tarif_id_russia, tarif_id_russia_mob, tarif_id_intern, tarif_id_sng):
        dst = self.db.cursor()
        dst.execute("""INSERT INTO billing.usage(id, client_id, phone_num, actual_from, actual_to, no_of_lines, region)
                        VALUES (%s, %s, %s, %s, %s, %s, %s)""",
            (usage_id, client_id, phone_num, '2013-01-01', '2029-01-01', no_of_lines, branchCityCode) )
        self.markDirtyTable('billing.usage')

        dst.execute("""INSERT INTO billing.tarif_change_log(
            id, usage_id, ts, date_activation, tarif_id_local, tarif_id_local_mob, 
            tarif_id_russia, tarif_id_russia_mob, tarif_id_intern, tarif_id_sng)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)""",
            (change_id, usage_id, '1960-01-01 00:00:00', '1960-01-01',
            tarif_id_local, tarif_id_local_mob, tarif_id_russia, tarif_id_russia_mob, tarif_id_intern, tarif_id_sng) )
        self.markDirtyTable('billing.tarif_change_log')

    def getCall(self, callrecord):
        class CallRecord(object):
            def __init__(self, cursor, callrecord):
                
                cursor.execute("""SELECT "time", direction_out, usage_num, phone_num, len, usage_id, 
                                   pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, 
                                   month, day, amount, amount_op, client_id, region, geo_id, srv_region_id, 
                                   pricelist_op_id, price, price_op, len_mcn, prefix_mcn, prefix_op, 
                                   prefix_geo, len_op, geo_operator_id, redirect_num
                              FROM calls.calls WHERE id=%s""", (callrecord,) )
                if cursor.rowcount:
                    (self.start_time, self.direction_out, self.usage_num, self.phone_num, self.call_len, self.usage_id, 
                        self.pricelist_mcn_id, self.operator_id, self.free_min_groups_id, self.dest, self.mob, 
                        self.month, self.day, self.amount, self.amount_op, self.client_id, self.nativeBranchCityCode, self.geo_id, self.servedByBranchCityCode, 
                        self.pricelist_op_id, self.price, self.price_op, self.len_mcn, self.prefix_mcn, self.prefix_op, 
                        self.prefix_geo, self.len_op, self.geo_operator_id, self.redirect_num) = cursor.fetchone()

                    self.callrecord = callrecord
                else:
                    raise Exception("Record doesn't exist")

            def __repr__(self):
                return str((self.callrecord, self.start_time, self.direction_out, self.usage_num, self.phone_num, self.call_len, self.usage_id, 
                        self.pricelist_mcn_id, self.operator_id, self.free_min_groups_id, self.dest, self.mob, 
                        self.month, self.day, self.amount, self.amount_op, self.client_id, self.nativeBranchCityCode, self.geo_id, self.servedByBranchCityCode,
                        self.pricelist_op_id, self.price, self.price_op, self.len_mcn, self.prefix_mcn, self.prefix_op, 
                        self.prefix_geo, self.len_op, self.geo_operator_id, self.redirect_num))

        return CallRecord(self.db.cursor(), callrecord)

