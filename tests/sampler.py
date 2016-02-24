#!/bin/python
# -*- coding: utf-8 -*-

import psycopg2
import pgpubsub
import time
import datetime
import os
import sys
import imp
import urllib2

print 'Запущен скрипт генератора тестовых звонков'
print '  Алгоритм работы:'
print '    - Ожидает, когда все демоны заберут данные в регионы (если надо)'
print '    - Вставляет звонки в регионы'
print '    - Ожидает, когда все звонки из всех регионов придут в центр'

TEST_CLIENT_ID = 35420

# Номера, с которыми тестировать звонки. Обновлять копипастой из Confluence.
# Парсер ниже преобразует их в список. Потом эти номера будут вынесены в отдельный файл.
DIAL_NUMBERS = '''МН (фикс/моб)	 
Germany Proper	4961516273145
Kazakhstan Mobile K Cell	77017110982
Uzbekistan Tashkent	998712388752
Kazakhstan Mobile K Cell	77015038723
Moldova Mobile Orange	37369212293
Italy mobile Omnitel (Vodafone)	393404545638
Moldova Mobile Orange	37369636357
Moldova Mobile Orange	37369507325
Germany Proper	4971189462233
РФ (фикс)	 
Russia Zone2 Republic Tatarstan, Naberezhnye Chelny	78435249958
Russia Zone1 Kemerovo region, Kemerovo	73842587548
Russia Zone1 Kemerovo region, Kemerovo	73842570043
Russia Zone1 Republic Chuvashia, Cheboksary/ Novocheboksarsk	78352416357
Russia Zone1 Tver region, Tver	74822362705
Russia Zone1 Tver region, Tver	74822571126
Russia Zone1 Yaroslavl region, Yaroslavl	74852260006
Russia Zone2 Republic Tatarstan, Naberezhnye Chelny	78435706883
Russia Zone2 Republic Tatarstan, Naberezhnye Chelny	78432344613
Russia Zone2 Vladimir region	74924367293
Russia Zone2 Zabaiykalskiy region, Chita	73022327766
Russia Zone2 Zabaiykalskiy region, Chita	73022236255
Russia Zone3 Khabarovsk region, Khabarovsk	74212402456
Russia Zone3 Khabarovsk region, Khabarovsk	74212402331
Наши города (моб)	 
Москва и Московская область (mob)	79859774625
Москва и Московская область (mob)	79645015032
Москва и Московская область (mob)	79261161718
Санкт - Петербург и Ленинградская область (mob)	79213929332
Санкт - Петербург и Ленинградская область (mob)	79819744229
Санкт - Петербург и Ленинградская область (mob)	79643806711
Ростовская область (mob)	79889912219
Ростовская область (mob)	79281101042
Ростовская область (mob)	79612739229
Новосибирская область (mob)	79139249240
Новосибирская область (mob)	79612204444
Новосибирская область (mob)	79232233199
Краснодарский край (mob)	79054942999
Краснодарский край (mob)	79180025480
Краснодарский край (mob)	79282012083
Самарская область (mob)	79608201747
Самарская область (mob)	79084126916
Самарская область (mob)	79379966166
Свердловская область (mob)	79220226705
Свердловская область (mob)	79193632901
Свердловская область (mob)	79995600100
Нижегородская область (mob)	79103882729
Нижегородская область (mob)	79200340201
Нижегородская область (mob)	79036000033
Приморский край (mob)	79147040920
Приморский край (mob)	79247302424
Приморский край (mob)	79662741569
Республика Татарстан (mob)	79872962664
Республика Татарстан (mob)	79053172929
Республика Татарстан (mob)	79270424242
Наши города (фикс)	 
Самарская область. Самара	78463363439
Самарская область. Самара	78469919848
Москва	74957851212
Москва	74957863434
Санкт - Петербург	78129460868
Санкт - Петербург	78126355249
Краснодарский край. Краснодар	78612511434
Краснодарский край. Краснодар	78612040013
Республика Татарстан. Казань	78432749858
Республика Татарстан. Казань	78435159942
Новосибирская область. Новосибирск	73833833000
Новосибирская область. Новосибирск	73833120160
Ростовская область. Ростов - на - Дону	78633090017
Ростовская область. Ростов - на - Дону	78633090031
Свердловская область. Екатеринбург	73433586341
Свердловская область. Екатеринбург	73432151754
Нижегородская область. Нижний Новгород	78314649381
Нижегородская область. Нижний Новгород	78314174574
Приморский край. Владивосток	74232060203
Приморский край. Владивосток	74232080075
РФ (моб)	 
Хабаровский Край (mob)	79144260034
Смоленская область (mob)	79156350468
Республика Мордовия (mob)	79271944561
Пермский край (mob)	79222411325
Тверская область (mob)	79607133044
Ивановская область (mob)	79632156670
Челябинская область (mob)	79227201370
Республика Мордовия (mob)	79375177444
Калужская область (mob)	79038169829
Рязанская область (mob)	79056917863
Амурская область (mob)	79098139590'''

DIAL_NUMBERS = [num for num in [line.split()[-1] for line in DIAL_NUMBERS.split('\n')] if num.isdigit()]

REGION_TRUNKS = '''99	mcn_msk_ast16_99	MGTS_Loc	BEE_Loc
98	mcn_spb_ast6_98	smg_RTK_Loc	smg_Beeline_Loc
97	mcn_krd_ast6_97	smg_RTK_Loc	smg_Beeline_Loc
96	mcn_smr_ast6_96	smg_RTK_Loc		MTT
95	mcn_ekb_ast6_95	smg_RTK_Loc	smg_Beeline_Loc
94	mcn_nsk_ast6_94	smg_RTK_Loc	smg_Beeline_Loc
93	mcn_kzn_ast6_93	smg_Tattelecom_Loc	smg_Beeline_Loc
89	mcn_vlv_ast6_89	smg_RTK_Loc	smg_Beeline_Loc
88	mcn_nnv_ast6_88	smg_RTK_Loc	smg_Beeline_Loc
87	mcn_rnd_ast6_87	smg_RTK_Loc	smg_Beeline_Loc
81	HU_Bdp_Ast34	HU_Bdp_Gts	HU_Bdp_Gts_INT'''

REGION_TRUNKS = [line.split() for line in REGION_TRUNKS.split('\n')]

# Загружаем список регионов
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
regionsList = imp.load_source('regions-list', os.path.join(__location__, 'regions-list')).REGIONS_LIST.split()


# Подписываемся на вставки звонков в центральную БД

conn = psycopg2.connect(database='nispd_test', user='postgres')
cur = conn.cursor()


nowTime = datetime.datetime.now()
nowTimeStr = datetime.datetime.strftime(nowTime, '%Y-%m-%d %H:%M:%S')
nowTimeDbPartition = datetime.datetime.strftime(nowTime, '%Y%m')


# Вытаскиваем актуальный список DID'ов тестового клиента

# Ключ - регион. Значение - список DID'ов этого региона
regionDids = {}

cur.execute('''
  SET search_path = billing, pg_catalog;
  SELECT server_id, did FROM billing.service_number WHERE client_account_id = %(clientId)s
  AND activation_dt <= '%(nowTime)s' AND '%(nowTime)s' < expire_dt
  ORDER BY server_id;
''' % {'clientId': TEST_CLIENT_ID, 'nowTime': nowTimeStr})

rows = cur.fetchall()
for region_id, did in rows:
  if region_id not in regionDids :
    regionDids[region_id] = []

  regionDids[region_id].append(did)


# Определяем, какие номера из списка - наши, и определяем их регион

ourDialNumbers = {}

cur.execute('''
  SET search_path = billing, pg_catalog;
  SELECT server_id, did FROM billing.service_number WHERE did IN (%(numbers)s)
  ORDER BY server_id;
''' % {'numbers': ','.join(["'%s'" % num for num in DIAL_NUMBERS]), 'nowTime': nowTimeStr})

rows = cur.fetchall()
for region_id, did in rows:
  ourDialNumbers[did] = region_id

print ourDialNumbers


# Вытаскиваем список номеров, используемых для исходящих звонков для линий без номера

outboundDidForNoNum = {}

cur.execute('''
  SET search_path = public, pg_catalog;
  SELECT id, calling_station_id_for_line_without_number FROM public.server''')

rows = cur.fetchall()
for region_id, number in rows:
  outboundDidForNoNum[region_id] = number


# Вытаскиваем актуальный список префиксов для определения местных транков

localPrefix = {
  # 99: ['7495'],
  # 98: ['7812', '7813']
}

cur.execute('''
  SET search_path = billing, pg_catalog;
  SELECT id, region_id FROM billing.instance_settings WHERE active = 't';''')

rows = cur.fetchall()
for region_id, prefixes in rows:
  localPrefix[region_id] = prefixes

# Навешиваем триггеры для нотификаций завершения синхронизации
# "центр->регион" до генерации тестовых звонков и "регион->центр" - после

cur.execute('''
  SET search_path = calls_raw, pg_catalog;

  CREATE OR REPLACE FUNCTION notify_raw_calls_received()
    RETURNS trigger AS $$
  DECLARE
  BEGIN
    PERFORM pg_notify(
      'rawcallsreceived',
      row_to_json(NEW)::text);
    RETURN NEW;
  END;
  $$ LANGUAGE plpgsql;

  DROP TRIGGER IF EXISTS notify_raw_calls_received_tr ON calls_raw_%(dbPartTime)s;

  CREATE TRIGGER notify_raw_calls_received_tr
    AFTER INSERT ON calls_raw_%(dbPartTime)s
    FOR EACH ROW
    EXECUTE PROCEDURE notify_raw_calls_received();

  SET search_path = event, pg_catalog;

  CREATE OR REPLACE FUNCTION notify_queue_delete()
    RETURNS trigger AS $$
  DECLARE
  BEGIN
    PERFORM pg_notify('queuedeleted'::text, ''::text);
    RETURN OLD;
  END;
  $$ LANGUAGE plpgsql;

  DROP TRIGGER IF EXISTS notify_queue_delete_tr ON queue;

  CREATE TRIGGER notify_queue_delete_tr
    AFTER DELETE ON queue
    FOR EACH ROW
    EXECUTE PROCEDURE notify_queue_delete();
''' % {'dbPartTime': nowTimeDbPartition})

conn.commit()


# Убираем ограничения, так как "звоним" на огромные суммы
cur.execute('''
  SET search_path = billing, pg_catalog;
  UPDATE billing.clients SET voip_limit_month = 0, voip_limit_day = 0, credit = 100000000
  WHERE id = %(clientId)d''' % {'clientId': TEST_CLIENT_ID})
conn.commit()


# Ждём завершения синхронизации "центр->регион"

cur.execute('''
  SET search_path = event, pg_catalog;
  SELECT count(server_id) FROM queue WHERE server_id IN (%(regions)s);
''' % {'regions': ','.join(regionsList)})
rows = cur.fetchall()
if rows[0][0] > 0 :
  print 'Ждём, когда все регионы заполнят свои базы, забрав данные из центральной'
  onsync = pgpubsub.connect(user='postgres', database='nispd_test')
  onsync.listen('queuedeleted')

  for e in onsync.events(select_timeout=5*60, yield_timeouts=True):
    if e is None:
      print 'ERROR: Синхронизация из центра в регионы неудачна'
      break
    else:
      sys.stdout.write('.')
      cur.execute('''
        SET search_path = event, pg_catalog;
        SELECT count(server_id) FROM queue WHERE server_id IN (%(regions)s);
      ''' % {'regions': ','.join(regionsList)})
      rows = cur.fetchall()
      print rows[0][0]
      if rows[0][0] == 0 :
        print 'Успешно дождались синхронизации данных из центра в регионы'
        break

  onsync.unlisten('queuedeleted')


# Создаём таблицы для хранения тестовых данных
cur.execute('''
  CREATE SCHEMA IF NOT EXISTS tests;
  CREATE TABLE IF NOT EXISTS tests.voiprouting_tests (
    sampler_id BIGINT NOT NULL, -- ID прогона тестов
    region_id INT NOT NULL,
    src_number TEXT NOT NULL,
    dst_number TEXT NOT NULL,
    route_case TEXT, -- RouteCase, если есть
    debug TEXT       -- Полный ответ демона
  )
''')
conn.commit()


# Пустая queue ещё не значит, что демон биллинга готов к вставке звонков.
# Что весьма печально. TODO: это надо исправить, после исправления sleep выпилить.
time.sleep(120)

# Подписка при синхронизации calls_raw из региона в центр
pubsub = pgpubsub.connect(user='postgres', database='nispd_test')
pubsub.listen('rawcallsreceived')

# Наши транки
myTrunk = {
  # 99: 'mcn_msk_ast17_99',
  # 98: 'mcn_spb_ast6_98'
}

# Местные транки
localTrunk = {
  # 99: 'Beeline_SIP',
  # 98: 'MGTS_Loc'
}

# Не местные транки
mgmnTrunk = {
  # 99: 'Beeline_SIP',
  # 98: 'MGTS_Spd'
}


# Заполняем транки из распарсенного списка
for region_id, myTrunkName, localTrunkName, mgmnTrunkName in REGION_TRUNKS :
  region_id = int(region_id)
  myTrunk[region_id] = myTrunkName
  localTrunk[region_id] = localTrunkName
  mgmnTrunk[region_id] = mgmnTrunkName

def isLocalFor(number, region) :
  prefixes = localPrefix[region]
  for prefix in prefixes :
    if number.startswith(prefix) :
      return True
  return False

def determineAlienTrunk(number, region) :
  if number not in ourDialNumbers :
    return localTrunk[region] if isLocalFor(number, region) else mgmnTrunk[region]
  else :
    # TODO: Процедура неправильная, нужны местные локальные транки с указанным регионом,
    # (выбирать их по trunk.code??? - стрёмно), поэтому пока не используем.
    ourTrunkRegion = ourDialNumbers[number]
    return myTrunk[ourTrunkRegion]


print 'Вставляем тестовые звонки в регионы:'
print regionsList

regConnections = [(psycopg2.connect(database='nispd%s_test' % reg, user='postgres'), int(reg)) for reg in regionsList]

CALLID_START = long('%d000000' % int(round(time.time())))
callId = CALLID_START

# Вставляем тестовые звонки

for (regConn, region_id) in regConnections :
  curReg = regConn.cursor()

  curReg.execute('''
    DROP SEQUENCE calls_cdr.cdr_id_seq;
    CREATE SEQUENCE calls_cdr.cdr_id_seq
      INCREMENT 1
      MINVALUE 1
      MAXVALUE 9223372036854775807
      START %(callId)d
      CACHE 1;
    ALTER TABLE calls_cdr.cdr_id_seq
      OWNER TO postgres;
    GRANT ALL ON SEQUENCE calls_cdr.cdr_id_seq TO postgres;
    GRANT SELECT ON SEQUENCE calls_cdr.cdr_id_seq TO g_readonly;
  ''' % {'callId': callId})

  regConn.commit()

  if region_id not in regionDids :
    print 'Пропускаем регион %s т.к. для него нет тестовых данных' % region_id
    regConn.close()
    continue

  dids = regionDids[region_id]

  # Звонки делаем исходящие + входящие на 800-й номер. Стоимость остальных будет нулевой, её нужно исключить из тестов.

  for did in dids :
    # print 'For did ' + did
    for number in DIAL_NUMBERS :
      # print 'For number ' + number
      for A, B in [(did, number), (number, did)] :
        if A == B :
          continue

        # Входящие не на 800-й пропускаем - они все бесплатные.
        if B == did and not B.startswith('7800') :
          continue

        # Входящие иностранные на 800-е пропускаем
        if not A.startswith('7') and B.startswith('7800') :
          continue

        # Исходящие звонки с 800-х вообще не должны делаться, для них используются "короткие" номера.
        if A.startswith('7800') :
          continue

        if number in ourDialNumbers :
          # Не генерим звонки между нашими абонентами,
          # т.к. определение нашего транка - либо нетривиально, либо требует решения.
          continue

        if A == did :
          # Заменяем короткий номер:
          if len(A) == 4 :
            A = outboundDidForNoNum[region_id] + '*' + A

          # Для всех исходящих генерим тестирование маршрутизации
          # Результаты пишем (append'ом) в формате "Регион А-номер Б-номер RouteCase"
          # Полный лог ответа демона биллинга тоже пишем, чтобы можно было сравнить на месте
          # fail с предыдущим позитивным ответом.
          # Результаты сравниваются и выводится позитивная информация для тех кейсов,
          # для которых остутствуют или отличаются результаты в новой версии,
          # наряду с негативными ответами демона биллинга.

          routeReply = ''
          route_case = ''

          try :
            requestUrl = 'http://localhost:80%(region_id)s/test/auth?trunk_name=%(myTrunk)s&src_number=%(A)s&dst_number=%(B)s&src_noa=3&dst_noa=3' % {
              'A': A, 'B': B, 'region_id': region_id, 'myTrunk': myTrunk[region_id]
            }
            routeReply = urllib2.urlopen(requestUrl).read()
            route_case = routeReply.split('\n')[-2]
          except Exception as err :
            routeReply = err.reason

          cur.execute('''INSERT INTO tests.voiprouting_tests
            (sampler_id, region_id, src_number, dst_number, route_case, debug) VALUES
            (%(sampler_id)s, %(region_id)s, %(src_number)s, %(dst_number)s, %(route_case)s, %(debug)s)''',
            {
              'sampler_id': CALLID_START, 'region_id': region_id,
              'src_number': A, 'dst_number': B, 'route_case': route_case,
              'debug': str(routeReply)
            })
          conn.commit()

        '''
        При необходимости добавить проверку маршрутизации и выбор актуального маршрута самим биллингом,
        а не по информации из Confluence, нужно выполнять следующий код:

        requestUrl = 'http://localhost:80%(region_id)s/test/auth?trunk_name=%(myTrunk)s&src_number=%(A)s&dst_number=%(B)s&src_noa=3&dst_noa=3' % {'A': A, 'B': B, 'region_id': region_id, 'myTrunk': myTrunk[region_id]}
        
        print urllib2.urlopen(requestUrl).read().split('\n')[-2]

        # Нужно парсить такой блок в случае успеха:

        INFO|TERM SERVICE TRUNKS|
        INFO||PRICE: 11.6938, TRUNK: smg_Beeline_Loc (206), SERVICE TRUNK 6, PRICELIST: 23, PREFIX: 7701
        INFO||PRICE: 11.94, TRUNK: MTT (205), SERVICE TRUNK 39, PRICELIST: 14, PREFIX: 7701
        RESULT|ROUTE CASE|rc_auto_206_205

        или

        INFO|OUTCOME|RC_Beeline (333)
        FOUND|ROUTE CASE|BY ID '239'
        ||(id: 239, name: RC_Beeline, )
        RESULT|ROUTE CASE|RC_Beeline
        '''

        # Для тестов будем брать все транки из роуткейса, чтобы покрыть их тестами.

        # По имени Route Case'а и server_id вытащить его ID из auth.route_case
        # По route_case_id в auth.route_case_trunk взять все trunk_id,
        # Для каждого из них получить из таблицы trunk имена транков.

        if B == did :
          # Входящий звонок
          srcTrunk = determineAlienTrunk(A, region_id)
          dstTrunk = myTrunk[region_id]
        else :
          # Исходящий звонок
          srcTrunk = myTrunk[region_id]
          dstTrunk = determineAlienTrunk(B, region_id)

        statement = '''
          SELECT insert_cdr(%(callId)d::bigint, '85.94.32.233'::inet, '%(A)s', '%(B)s',
          '',
          360030::bigint, '%(nowTime)s', '%(nowTime)s', '%(nowTime)s',
          16::smallint,
          '%(srcTrunk)s', '%(dstTrunk)s',
          3::smallint, 3::smallint,
          '')
          ''' % {'A': A, 'B': B, 'callId': callId, 'srcTrunk': srcTrunk, 'dstTrunk': dstTrunk,
                 'nowTime': nowTimeStr}

        # print statement

        curReg.execute(statement)

        callId += 1

  regConn.commit()
  regConn.close()

conn.close()

print 'Ждём, когда в центральном сервере появятся все обсчитанные звонки. Или таймаута'

# На каждый один звонок приходится две записи:
callId += callId - CALLID_START

for e in pubsub.events(select_timeout=10*60, yield_timeouts=True):
  if e is None:
    print 'ERROR: Синхронизация из регионов в центр неудачна. Недополучено %d звонков.' % (callId - CALLID_START)
    break
  else:
    # Success part: запомнить номер последнего прогона.
    sys.stdout.write('.')

    callId -= 1
    if callId == CALLID_START :
      print 'Успешно дождались синхронизации звонков из региона в центр'
      break

pubsub.unlisten('rawcallsreceived')

