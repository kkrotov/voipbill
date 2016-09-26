#!/usr/bin/python
import time
import sys
import traceback
import MySQLdb
import psycopg2

from Daemon import Daemon

from syncstat_config import config

tbase = 'nispd'
pid_file = '/var/run/syncstat.pid'


def fix_date(d):
    if   d == '9999-00-00 00:00:00':
        d =  '4000-01-01'
    elif d == '0000-00-00 00:00:00':
        d =  '4000-01-01'
    elif d == '0000-00-00':
        d =  '4000-01-01'
    elif d == 'NULL':
        d =  '4000-01-01'
    elif d == '2005-00-00':
        d =  '2005-01-01'
    elif d == None:
        d = '4000-01-01'
    return d


class Sync(Daemon):
    def init(self):
        self.db_stat = MySQLdb.connect(host=config['stat']['host'], user=config['stat']['user'], passwd=config['stat']['password'], db=config['stat']['db'])
        self.db_stat.autocommit(True)
        
        self.db = psycopg2.connect(host=config['bill']['host'], user=config['bill']['user'], password=config['bill']['password'], database=config['bill']['db'])
        self.db.set_isolation_level(0)
        
        cur = self.db_stat.cursor()
        cur.execute("SET TRANSACTION ISOLATION LEVEL READ COMMITTED")
        cur.execute("SET NAMES koi8r")
        
    def do_sync(self):
        need = True
        while need:
            need = False
            
            n = self.do_sync_clients(100)
            if n > 0:
                print '+', n, 'client'
                if n == 100:
                    need = True

            n = self.do_sync_organization(100)
            if n > 0:
                print '+', n, 'organization'
                if n == 100:
                    need = True
        
            n = self.do_sync_usage_voip(100)
            if n > 0:
                print '+', n, 'usage_voip'
                if n == 100:
                    need = True

            n = self.do_sync_usage_voip_package(100)
            if n > 0:
                print '+', n, 'usage_voip_package'
                if n == 100:
                    need = True

            n = self.do_sync_usage_trunk(100)
            if n > 0:
                print '+', n, 'usage_trunk'
                if n == 100:
                    need = True

            n = self.do_sync_usage_trunk_settings(100)
            if n > 0:
                print '+', n, 'usage_trunk_settings'
                if n == 100:
                    need = True

            n = self.do_sync_tarif(100)
            if n > 0:
                print '+', n, 'tarif'
                if n == 100:
                    need = True

            n = self.do_sync_log_tarif(100)
            if n > 0:
                print '+', n, 'log_tarif'
                if n == 100:
                    need = True

            n = self.do_sync_prefixlist(100)
            if n > 0:
                print '+', n, 'prefixlist'
                if n == 100:
                    need = True

            n = self.do_sync_destination_prefixlists(100)
            if n > 0:
                print '+', n, 'destination_prefixlists'
                if n == 100:
                    need = True

            n = self.do_sync_tariff_package(100)
            if n > 0:
                print '+', n, 'tariff_package'
                if n == 100:
                    need = True

            n = self.do_sync_currency_rate(100)
            if n > 0:
                print '+', n, 'currency_rate'
                if n == 100:
                    need = True


    def do_sync_clients(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, 
                                    c.voip_credit_limit, c.voip_credit_limit_day, c.voip_limit_mn_day, 
                                    case c.voip_disabled when true then 't' else 'f' end, 
                                    if(c.balance > 1000000, 1000000, if(c.balance < -1000000, -1000000, c.balance)) as balance, 
                                    c.credit, 
                                    c.last_account_date, 
                                    c.last_payed_voip_month, 
                                    cc.organization_id,
                                    case c.price_include_vat when true then 't' else 'f' end,
                                    case c.is_blocked when true then 't' else 'f' end,
                                    c.timezone_offset,
                                    case c.anti_fraud_disabled when true then 't' else 'f' end,
                                    c.account_version
                                from z_sync_postgres z left join clients c on z.tid=c.id
                                left join client_contract cc on cc.id=c.contract_id
                                where z.tbase='"""+tbase+"' and z.tname='clients' limit "+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9],r[10],r[11],r[12],r[13],r[14],r[15]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')
        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.clients(id, voip_limit_month, voip_limit_day, voip_limit_mn_day, voip_disabled, balance, credit,amount_date,last_payed_month,organization_id,price_include_vat,is_blocked,timezone_offset,anti_fraud_disabled,account_version)VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.clients where id=%s", todel)

        cur.execute('COMMIT')

        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='clients' and tid=%s and rnd=%s", tofix)

        return len(tofix)

    def do_sync_organization(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""    select z.rnd, z.tid, o.organization_id, o.actual_from, o.actual_to, o.vat_rate
                                from z_sync_postgres z left join organization o on z.tid=o.id
                                where z.tbase='"""+tbase+"' and z.tname='organization' limit "+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3],r[4],r[5]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')
        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.organization(id, organization_id, actual_from, actual_to, tax_rate)VALUES(%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.organization where id=%s", todel)

        cur.execute('COMMIT')

        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='organization' and tid=%s and rnd=%s", tofix)

        return len(tofix)

    def do_sync_usage_voip(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, cc.id, c.activation_dt, c.expire_dt, c.E164, c.no_of_lines, c.region, vn.number_tech, vn.operator_account_id
                                from z_sync_postgres z
                                left join usage_voip c on z.tid=c.id
                                left join clients cc on c.client=cc.client
                                left join voip_numbers vn on vn.number = c.E164
                                where z.tbase='"""+tbase+"""' and z.tname='usage_voip'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],fix_date(r[3]),fix_date(r[4]),r[5],r[6],r[7],r[8],r[9]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.service_number(id, client_account_id, activation_dt, expire_dt, did, lines_count, server_id, tech_number, tech_number_operator_id)VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.service_number where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='usage_voip' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)
        
    def do_sync_usage_voip_package(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.activation_dt, c.expire_dt, c.tariff_id, c.usage_voip_id, c.usage_trunk_id
                                from z_sync_postgres z
                                left join usage_voip_package c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='usage_voip_package'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],fix_date(r[2]),fix_date(r[3]),r[4],r[5],r[6]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.service_package(id, activation_dt, expire_dt, tariff_package_id, service_number_id, service_trunk_id)VALUES(%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.service_package where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='usage_voip_package' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)
        
    def do_sync_usage_trunk(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.client_account_id, c.activation_dt, c.expire_dt, c.trunk_id, c.connection_point_id, c.orig_enabled, c.term_enabled, c.orig_min_payment, c.term_min_payment, c.operator_id
                                from z_sync_postgres z
                                left join usage_trunk c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='usage_trunk'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],fix_date(r[3]),fix_date(r[4]),r[5],r[6],r[7],r[8],r[9],r[10],r[11]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.service_trunk(id, client_account_id, activation_dt, expire_dt, trunk_id, server_id, orig_enabled, term_enabled, orig_min_payment, term_min_payment, operator_id)VALUES(%s,%s,%s,%s,%s,%s,'%s','%s',%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.service_trunk where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='usage_trunk' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)

    def do_sync_usage_trunk_settings(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""    select z.rnd, z.tid, c.usage_id, c.type, c.order, c.src_number_id, c.dst_number_id, c.pricelist_id, c.minimum_minutes, c.minimum_cost, c.minimum_margin, c.minimum_margin_type
                                from z_sync_postgres z
                                left join usage_trunk_settings c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='usage_trunk_settings'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],fix_date(r[3]),fix_date(r[4]),r[5],r[6],r[7],r[8],r[9],r[10],r[11]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.service_trunk_settings(id, trunk_id, \"type\", \"order\", src_number_id, dst_number_id, pricelist_id, minimum_minutes, minimum_cost, minimum_margin, minimum_margin_type)VALUES(%s,%s,'%s','%s',%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.service_trunk_settings where id=%s", todel)

        cur.execute('COMMIT')

        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='usage_trunk_settings' and tid=%s and rnd=%s", tofix)

        return len(tofix)

        
    def do_sync_tarif(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.name, 
                                    case c.currency_id when 'RUR' then 1 when 'RUB' then 1 when 'USD' then 2 else 3 end, 
                                    c.status, c.free_local_min, 
                                    case c.freemin_for_number when true then 't' else 'f' end, 
                                    c.pricelist_id, 
                                    case c.paid_redirect when true then 't' else 'f' end,
                                    case c.tariffication_by_minutes when true then 't' else 'f' end,
                                    case c.tariffication_full_first_minute when true then 't' else 'f' end,
                                    case c.tariffication_free_first_seconds when true then 't' else 'f' end
                                from z_sync_postgres z left join tarifs_voip c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='tarifs_voip' 
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2].decode('koi8-r'),r[3],r[4],r[5],r[6],r[7],r[8],r[9],r[10],r[11]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')
        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.tariff(id, name, currency_id, status,freemin,freemin_for_number,pricelist_id,paid_redirect,tariffication_by_minutes,tariffication_full_first_minute,tariffication_free_first_seconds)VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.tariff where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='tarifs_voip' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)

    def do_sync_log_tarif(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.id_service, c.ts, c.date_activation, c.id_tarif, c.id_tarif_local_mob, c.id_tarif_russia, c.id_tarif_russia_mob, c.id_tarif_intern, c.id_tarif_sng 
                                from z_sync_postgres z 
                                left join log_tarif c on z.tid=c.id 
                                where z.tbase='"""+tbase+"""' and z.tname='log_tarif' 
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],fix_date(r[3]),fix_date(r[4]),r[5],r[6],r[7],r[8],r[9],r[10]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')
        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.tariff_change_log (id, usage_id, ts, date_activation, tarif_id_local, tarif_id_local_mob, tarif_id_russia, tarif_id_russia_mob, tarif_id_intern, tarif_id_sng)VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.tariff_change_log where id=%s", todel)

        cur.execute('COMMIT')

        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='log_tarif' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)

    def do_sync_prefixlist(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.type_id, sub_type, prefixes, country_id, region_id, city_id, case exclude_operators when true then 't' else 'f' end, operators
                                from z_sync_postgres z
                                left join voip_prefixlist c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='prefixlist'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.stat_prefixlist(id, type_id, sub_type, prefixes, country_id, region_id, city_id, exclude_operators, operators)VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.stat_prefixlist where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='prefixlist' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)
        
    def do_sync_destination_prefixlists(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.destination_id, c.prefixlist_id
                                from z_sync_postgres z
                                left join voip_destination_prefixes c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='dest_prefixes'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.stat_destination_prefixlists(id, destination_id, prefixlist_id)VALUES(%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.stat_destination_prefixlists where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='dest_prefixes' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)
        
    def do_sync_tariff_package(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.destination_id, c.minutes_count, c.pricelist_id
                                from z_sync_postgres z
                                left join tarifs_voip_package c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='tariff_package'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3],r[4]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.tariff_package(id, destination_id, prepaid_minutes, pricelist_id)VALUES(%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.tariff_package where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='tariff_package' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)
        
    def do_sync_currency_rate(self, partsize):
        cur_stat = self.db_stat.cursor()
        cur_stat.execute("""        select z.rnd, z.tid, c.currency, c.date, c.rate
                                from z_sync_postgres z
                                left join currency_rate c on z.tid=c.id
                                where z.tbase='"""+tbase+"""' and z.tname='currency_rate'
                                limit """+str(partsize))
        todel = []
        toins = []
        tofix = []
        for r in cur_stat.fetchall():
            tofix.append( (r[1],r[0]) )
            if r[2] == None:
                todel.append( (r[1],) )
            else:
                toins.append( (r[1],r[2],r[3],r[4]) )

        if len(tofix) == 0:
            return 0

        cur = self.db.cursor();
        cur.execute('BEGIN')

        if len(toins) > 0:
            cur.executemany("INSERT INTO billing.currency_rate(id, currency, date, rate)VALUES(%s,%s,%s,%s)", toins)
        if len(todel) > 0:
            cur.executemany("delete from billing.currency_rate where id=%s", todel)

        cur.execute('COMMIT')
        
        cur_stat.executemany("delete from z_sync_postgres where tbase='"+tbase+"' and tname='currency_rate' and tid=%s and rnd=%s", tofix)
        
        return len(tofix)

    def is_need_sync(self):
        cur = self.db_stat.cursor()
        cur.execute(" select tbase from z_sync_postgres where tbase='"+tbase+"' limit 1")
        return cur.rowcount > 0
        
    def run(self):
        while True:
            try:
                self.init()
                while True:
                    if self.is_need_sync():
                        self.do_sync()
                    time.sleep(1)
            except KeyboardInterrupt:
                break
            except:
                print "error:", traceback.format_exc()
            time.sleep(10)


if __name__ == '__main__':
    d = Sync(pid_file)
    d.execd()


