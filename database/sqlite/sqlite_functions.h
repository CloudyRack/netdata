// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETDATA_SQLITE_FUNCTIONS_H
#define NETDATA_SQLITE_FUNCTIONS_H

#include "../../daemon/common.h"
#include "sqlite3.h"
#include "sqlite_event_loop.h"

// return a node list
struct node_instance_list {
    uuid_t  node_id;
    uuid_t  host_id;
    char *hostname;
    int live;
    int querable;
    int hops;
};


#define SQLITE_INSERT_DELAY (50)        // Insert delay in case of lock

#define SQL_STORE_HOST "insert or replace into host (host_id,hostname,registry_hostname,update_every,os,timezone,tags) values (?1,?2,?3,?4,?5,?6,?7);"

#define SQL_STORE_CHART "insert or replace into chart (chart_id, host_id, type, id, " \
    "name, family, context, title, unit, plugin, module, priority, update_every , chart_type , memory_mode , " \
    "history_entries) values (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,?16);"

#define SQL_FIND_CHART_UUID                                                                                            \
    "select chart_id from chart where host_id = @host and type=@type and id=@id and (name is null or name=@name);"

#define SQL_STORE_ACTIVE_CHART                                                                                         \
    "insert or replace into chart_active (chart_id, date_created) values (@id, strftime('%s'));"

#define SQL_STORE_DIMENSION                                                                                           \
    "INSERT OR REPLACE into dimension (dim_id, chart_id, id, name, multiplier, divisor , algorithm) values (?0001,?0002,?0003,?0004,?0005,?0006,?0007);"

#define SQL_FIND_DIMENSION_UUID "select dim_id from dimension where chart_id=@chart and id=@id and name=@name;"

#define SQL_STORE_ACTIVE_DIMENSION                                                                                     \
    "insert or replace into dimension_active (dim_id, date_created) values (@id, strftime('%s'));"
extern int sql_init_database(void);
extern void sql_close_database(void);

extern int sql_store_host(uuid_t *guid, const char *hostname, const char *registry_hostname, int update_every, const char *os, const char *timezone, const char *tags);
extern int sql_store_chart(
    uuid_t *chart_uuid, uuid_t *host_uuid, const char *type, const char *id, const char *name, const char *family,
    const char *context, const char *title, const char *units, const char *plugin, const char *module, long priority,
    int update_every, int chart_type, int memory_mode, long history_entries);
extern int sql_store_dimension(uuid_t *dim_uuid, uuid_t *chart_uuid, const char *id, const char *name, collected_number multiplier,
                               collected_number divisor, int algorithm);

extern uuid_t *find_dimension_uuid(RRDSET *st, RRDDIM *rd);
extern uuid_t *create_dimension_uuid(RRDSET *st, RRDDIM *rd);
extern void store_active_dimension(uuid_t *dimension_uuid);

extern uuid_t *find_chart_uuid(RRDHOST *host, const char *type, const char *id, const char *name);
extern uuid_t *create_chart_uuid(RRDSET *st, const char *id, const char *name);
extern int update_chart_metadata(uuid_t *chart_uuid, RRDSET *st, const char *id, const char *name);
extern void store_active_chart(uuid_t *dimension_uuid);

extern int find_uuid_type(uuid_t *uuid);

extern void sql_rrdset2json(RRDHOST *host, BUFFER *wb);

extern RRDHOST *sql_create_host_by_uuid(char *guid);
extern void db_execute(const char *cmd);
extern int file_is_migrated(char *path);
extern void add_migrated_file(char *path, uint64_t file_size);
extern void db_unlock(void);
extern void db_lock(void);
extern void delete_dimension_uuid(uuid_t *dimension_uuid);
extern void sql_store_chart_label(uuid_t *chart_uuid, int source_type, char *label, char *value);
extern void sql_build_context_param_list(struct context_param **param_list, RRDHOST *host, char *context, char *chart);
extern void store_claim_id(uuid_t *host_id, uuid_t *claim_id);
extern int update_node_id(uuid_t *host_id, uuid_t *node_id);
extern int get_node_id(uuid_t *host_id, uuid_t *node_id);
extern void invalidate_node_instances(uuid_t *host_id, uuid_t *claim_id);
extern struct node_instance_list *get_node_list(void);
extern void sql_load_node_id(RRDHOST *host);
extern int execute_insert(sqlite3_stmt *res);
extern void compute_chart_hash(RRDSET *st);
extern void sql_chart_from_hash_id(char *hash_str, BUFFER *wb);
extern int alert_hash_and_store_config(
    uuid_t config_hash_id,
    const char *alarm,
    const char *template_key,
    const char *os,
    const char *host,
    const char *on,
    const char *families,
    const char *plugin,
    const char *module,
    const char *lookup,
    const char *calc,
    const char *every,
    const char *green,
    const char *red,
    const char *warn,
    const char *crit,
    const char *exec,
    const char *to,
    const char *units,
    const char *info,
    const char *classification,
    const char *component,
    const char *type,
    const char *delay,
    const char *options,
    const char *repeat,
    const char *host_labels);
extern void sql_select_alert_config(char *hash_str, BUFFER *wb);
extern void sql_create_health_log_table(RRDHOST *host);
extern void sql_health_alarm_log_save(RRDHOST *host, ALARM_ENTRY *ae);
extern void health_alarm_entry_sql2json(BUFFER *wb, uint32_t unique_id, uint32_t alarm_id, RRDHOST *host);
extern int execute_insert(sqlite3_stmt *res);
extern void compute_chart_hash(RRDSET *st);
extern void sql_chart_from_hash_id(char *hash_str, BUFFER *wb);
#endif //NETDATA_SQLITE_FUNCTIONS_H
