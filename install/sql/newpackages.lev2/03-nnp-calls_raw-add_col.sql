ALTER TABLE calls_raw.calls_raw
 DROP COLUMN account_version_orig,
  DROP COLUMN account_version_term,
   DROP COLUMN stats_nnp_package_minute_orig_id,
    DROP COLUMN stats_nnp_package_minute_term_id,
     ADD COLUMN account_version integer,
      ADD COLUMN stats_nnp_package_minute_id integer
      