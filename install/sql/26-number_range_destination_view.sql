CREATE MATERIALIZED VIEW nnp.number_range_destination AS
  SELECT DISTINCT
    nrp.number_range_id,
    pd.destination_id
  FROM
    nnp.prefix_destination pd
    LEFT JOIN
    nnp.number_range_prefix nrp
    USING(prefix_id);

CREATE OR REPLACE FUNCTION nnp.number_range_destination_update() RETURNS trigger AS $$
DECLARE
  condition varchar;
BEGIN
  IF TG_OP = 'INSERT' THEN
    IF TG_TABLE_NAME = 'prefix_destination'
    THEN condition := 'pd.destination_id = ' || NEW.destination_id;
    ELSE condition := 'nrp.number_range_id = ' || NEW.number_range_id;
    END IF;
    PERFORM
      'INSERT INTO
         nnp.number_range_destination_view
       SELECT
         nrp.number_range_id,
         pd.destanation_id
       FROM
         nnp.prefix_destination pd
         ON
           pd.destination_id = d.id
       LEFT JOIN
         nnp.number_range_prefix nrp
         USING(prefix_id)
       WHERE ' || condition;
  ELSEIF TG_OP = 'DELETE' THEN
    IF TG_TABLE_NAME = 'prefix_destination'
    THEN condition := 'destination_id = ' || OLD.destination_id;
    ELSE condition := 'number_range_id = ' || OLD.number_range_id;
    END IF;
    PERFORM
      'DELETE FROM
         nnp.number_range_destination_view
       WHERE ' || condition;
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER number_range_destination_view_update
AFTER INSERT OR UPDATE OR DELETE ON nnp.number_range_prefix
FOR EACH ROW EXECUTE PROCEDURE nnp.number_range_destination_update();

CREATE TRIGGER number_range_destination_view_update
AFTER INSERT OR UPDATE OR DELETE ON nnp.prefix_destination
FOR EACH ROW EXECUTE PROCEDURE nnp.number_range_destination_update();

create UNIQUE index CONCURRENTLY on nnp.number_range_destination (number_range_id, destination_id);