#!/bin/sh

### /usr/pgsql-9.4/bin/psql -U postgres -h 127.0.0.1 nispd_test -c "delete from event.queue;SELECT event.notify_all(99);"

#/usr/pgsql-9.4/bin/psql -U postgres -h 127.0.0.1 nispd_test < 1.sql

### Накатываем дополнительные триггеры на центральную базу для синхронизации таблиц nnp

##/usr/pgsql-9.4/bin/psql -U doleinik_rw -h eridanus.mcn.ru nispd_test < 01-nnp-make-triggers-in-nispd.sql  
##/usr/pgsql-9.4/bin/psql -U doleinik_rw -h eridanus.mcn.ru nispd_test < event.queue.notify.all.sql

### Накатываем дополнительные триггеры на региональную базу для синхронизации таблиц nnp

/usr/pgsql-9.4/bin/psql -U doleinik -h 127.0.0.1 nispd_test < 01-nnp-make-triggers-in-nispd.sql  
/usr/pgsql-9.4/bin/psql -U doleinik -h 127.0.0.1 nispd_test < event.queue.notify.all.sql
/usr/pgsql-9.4/bin/psql -U doleinik -h 127.0.0.1 nispd99_test < 02-nnp_event_notify_local.sql  
