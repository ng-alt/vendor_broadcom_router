#
# Miscellaneous utilities Makefile
#
# Copyright (C) 2009, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile 382640 2013-02-03 04:35:49Z $
#

include $(TOP)/.config

srcbase=$(SRCBASE)

WLU_OBJS := wlu.o wlu_cmd.o wlu_iov.o wlu_linux.o bcmutils.o bcmwifi_channels.o wlu_client_shared.o wlu_pipe.o wlu_pipe_linux.o miniopt.o bcm_app_utils.o wlu_rates_matrix.o wlc_ppr.o
ifneq ($(wildcard $(srcbase)/wl/exe/wlu_common.c),)
WLU_OBJS += wlu_common.o
endif
WLU_OBJS += wlc_clm_data.o

ifeq ($(RWL),1)
WL_OBJS := $(WLU_OBJS)
SERVER_OBJS := wlu_server_linux.o wlu_pipe_linux.o wlu_pipe.o wlu_server_shared.o shellproc_linux.o
APUT_OBJS := wfa_apcmdproc.o wfa_apcmdlib.o wfa_apcs.o wfa_apcmdresp.o wfa_apcmdvalid.o wfa_apcmdtbls.o wfa_apcmdcvt.o wfa_val2str.o wfa_apdiy.o wfa_sock.o wfa_aput_all.o

SOCKET_OBJS := $(SERVER_OBJS:%.o=socket/%.o)
WIFI_OBJS := $(SERVER_OBJS:%.o=wifi/%.o)
WL_OBJS := $(WL_OBJS:%.o=wl_obj/%.o) 
WFA_APUT_OBJS := $(APUT_OBJS:%.o=aput/%.o)

SERVER_SOCKET  := wl_server_socket
SERVER_WIFI  := wl_server_wifi
WFA_APUT_ALL := wfa_aput_all
WL_EXE  := wl
endif

UTILS := epi_ttcp
CFLAGS += -s -I$(srcbase)/include -I$(srcbase)/common/include
CFLAGS += $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(srcbase)/shared/bcmwifi/include)))
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
CFLAGS += -DBCMINTERNAL -DBCMDBG

CFLAGS += -DLINUX -I$(srcbase)/wl/exe

# Use newer 11ac ratespec for wl command line
CFLAGS += -DD11AC_IOTYPES -DPPR_API

#ifdef WLCNT
CFLAGS += -DWLCNT
#endif

ifeq ($(CONFIG_TRAFFIC_MGMT),y)
CFLAGS += -DTRAFFIC_MGMT
endif

vpath %.c $(srcbase)/tools/misc $(WLAN_ComponentSrcDirs)

ifneq ($(wildcard $(srcbase)/et/linux/et.c $(srcbase)/et/linux/et.c),)
UTILS += et
vpath %.c $(srcbase)/et/linux $(srcbase)/et/linux
endif

ifneq ($(wildcard $(srcbase)/wl/exe $(srcbase)/wl/exe),)
CFLAGS += -I$(srcbase)/wl/exe -I$(srcbase)/wl/exe
UTILS += wl
vpath %.c $(foreach d,$(srcbase) $(srcbase),$d/wl/exe $d/wl/clm/src $d/shared)
endif

ifeq ($(RWL),1)
ifeq ($(BUILD_WFA_APUT),1)
ifneq ($(wildcard $(TOP)/aput),)
CFLAGS += -I$(TOP)/aput
UTILS += $(WFA_APUT_ALL)
vpath %.c $(TOP)/aput
endif
endif

ifneq ($(wildcard $(srcbase)/wl/exe),)
UTILS += $(SERVER_SOCKET)
vpath %.c $(srcbase)/wl/exe $(srcbase)/shared
endif

ifneq ($(wildcard $(srcbase)/wl/exe),)
UTILS += $(SERVER_WIFI)
vpath %.c $(srcbase)/wl/exe $(srcbase)/shared
endif
endif

ifeq ($(RTCONFIG_5301X),y)
CFLAGS += -DBCM5301X
endif
UTILS += robocfg

all: $(UTILS)

ifeq ($(wildcard $(srcbase)/wl/exe/*.c),)
wl:
	-cp -f $(srcbase)/wl/exe/prebuilt/wl wl
else
wl: $(WLU_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
endif

ifeq ($(RWL),1)
$(WFA_APUT_ALL): $(WFA_APUT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

aput/%.o: %.c
	@mkdir -p aput
	$(CC) -c $(CFLAGS) -o $@ $^
$(SERVER_SOCKET): $(SOCKET_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ 

socket/%.o: %.c
	@mkdir -p socket
	$(CC) -c $(CFLAGS) -DRWL_SOCKET -o $@ $^

$(SERVER_WIFI): $(WIFI_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

wifi/%.o: %.c
	@mkdir -p wifi
	$(CC) -c $(CFLAGS) -DRWL_WIFI -o $@ $^
endif

ifeq ($(wildcard robocfg.c),)
robocfg:
	-cp -f $(srcbase)/wl/exe/prebuilt/robocfg robocfg
else
robocfg: robocfg.c etc53xx.h
endif

clean:
	rm -f *.o *~ $(UTILS)
	rm -rf socket wifi

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
