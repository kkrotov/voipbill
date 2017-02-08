create type stat.report_filetypes as enum ('pdf', 'csv', 'html');

create type stat.report_statuses as enum ('in_queue', 'in_progress', 'complete');

CREATE TABLE stat.report_orders
(
  id smallserial NOT NULL,
  client_id integer NOT NULL,
  query character varying NOT NULL,
  order_datetime timestamp without time zone NOT NULL,
  file_complete_datetime timestamp without time zone,
  type stat.report_filetypes NOT NULL DEFAULT 'pdf'::stat.report_filetypes,
  status stat.report_statuses NOT NULL DEFAULT 'in_queue'::stat.report_statuses,
  file_path character varying,
  CONSTRAINT report_orders_pkey PRIMARY KEY (id)
)
WITH (
OIDS=FALSE
);

ALTER TABLE "stat"."report_orders"
ADD COLUMN "start_connect_time" timestamp,
ADD COLUMN "end_connect_time" timestamp;

GRANT SELECT ON SEQUENCE stat.report_orders_id_seq TO GROUP g_readonly;
GRANT SELECT ON TABLE stat.report_orders TO GROUP g_readonly;
GRANT INSERT ON TABLE stat.report_orders TO GROUP g_stat;
GRANT USAGE ON SEQUENCE stat.report_orders_id_seq TO GROUP g_stat;