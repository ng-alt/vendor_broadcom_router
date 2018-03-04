#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <utils.h>
#include <shutils.h>
#include <shared.h>
#include <sys/stat.h>
#include <bcmnvram.h>

#include "httpd.h"
#include "sqlite3.h"

#define LONGSIZE		4194304		// 2^32/1024 (bytes -> Kbytes)
#define DATA_PERIOD		60*60*24*30	// 1 months = 30*24*60*60
#define DAY			60*60*24	// 1 day
#define IFPATH_MAX		64		// path buffer
#define IFNAME_MAX		48		// ifname buffer
#define TL_PATH			"/jffs/tld/"	// traffic limiter database

// traffic_limiter.c
extern int traffic_limiter_main(char *type, char *q_if, char *q_te, char *q_ts);

// traffic_limiter_hook.c
extern int sql_get_table(sqlite3 *db, const char *sql, char ***pazResult, int *pnRow, int *pnColumn);
extern void traffic_limiter_hook(char *ifname, char *start, char *end, char *unit, int *retval, webs_t wp);
extern void traffic_limiter_WanStat(char *buf, char *ifname, char *start, char *end, int unit);
extern void ifname_mapping(char *word, char *ifname);
