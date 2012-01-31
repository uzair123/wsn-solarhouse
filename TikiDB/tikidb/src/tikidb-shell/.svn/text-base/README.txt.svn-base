Open scope 5 with all nodes whose id is greater than 2, use TTL 40

CREATE SCOPE 5 SUBSCOPE OF WORLD as (NODE_ID > 2) USE TTL 40

tikidb AAsBAAUAKAUGDQgJAg==

  0  11   1   0   5   0  40   5   6  13   8   9   2

 -> tikidb_msg_type: (0: statement)
     -> statement_id
         -> statement_type (open scope)
             -> super_scope_id
                 -> scope id
                     -> ttl (2 bytes): 40 seconds
                             -> scope spec len
                                 -> spec
-----------------

Close scope 5

CLOSE SCOPE 5

tikidb ABACBQ==

  0  16   2   5

 -> tikidb_msg_type: (0: statement)
     -> statement_id
         -> statement type (close scope)
             -> scope_id

-----------------
query through scope id 5, 6 columns (AMBIENT_LIGHT, PAR_LIGHT, HUMIDITY, TEMPERATURE, NODE_RSSI, NODE_PARENT), sample every 60 seconds, lifetime 1 day

SELECT AMBIENT_LIGHT, PAR_LIGHT, HUMIDITY, TEMPERATURE, NODE_RSSI, NODE_PARENT 
   FROM SCOPE_5
   SAMPLE PERIOD 60 seconds
   LIFETIME 24 hs

shell command:

tikidb ABUDAAYLKQsqCysLLAs1CzYBBQgAAB4ACQCowAA==

  0  21   3   0   6  11  41  11  42  11  43  11  44  11  53  11  54   1   5   8   0   0  30   0   9   0 168 192 0

 -> tikidb_msg_type: (0: statement)
     -> 21: statement_id
         -> 3: SELECT (instead of OPEN_SCOPE or so)
            -> 0: height
                 -> 6: 6 columns
                                                                      -> 1: num_scopes
                                                                          -> 5: scope_id
                                                                              -> 8: sample period
                                                                                  -> sample period #1
                                                                                      -> sample period #2
                                                                                          -> sample period #3
                                                                                              -> sample period #4
                                                                                                  -> 9: lifetime
                                                                                                      -> lifetime #1
                                                                                                          -> lifetime #2
                                                                                                              -> lifetime #3
                                                                                                                  -> lifetime #4


             