DROP TRIGGER IF EXISTS backlog_notify ON auth.airp;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.airp
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.destination;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.destination
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.hub;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.hub
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth."number";
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth."number"
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.outcome;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.outcome
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.prefixlist;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.prefixlist
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.prefixlist_prefix;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.prefixlist_prefix
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.release_reason;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.release_reason
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.route_case;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.route_case
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.route_table;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.route_table
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.route_table_route;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.route_table_route
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.routing_report_data;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.routing_report_data
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_group;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_group
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_group_item;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_group_item
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_number_preprocessing;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_number_preprocessing
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_priority;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_priority
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_rule;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_rule
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();

-------------------------------------------------------------------------------
DROP TRIGGER IF EXISTS backlog_notify ON auth.trunk_trunk_rule;
CREATE TRIGGER backlog_notify
  AFTER INSERT OR UPDATE OR DELETE
  ON auth.trunk_trunk_rule
  FOR EACH ROW
  EXECUTE PROCEDURE backlog.notify_table_update();
