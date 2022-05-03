TARGET = main
TYPE = exe

SRCS = source/ps1/main.c \
source/ps1/filesystem.c \
source/ps1/graphics.c \
source/ps1/pads.c \
source/ps1/includedfileslist.s \
thirdparty/nugget/common/crt0/crt0.s \



CPPFLAGS += -Ithirdparty/nugget/psyq/include
CPPFLAGS += -Ithirdparty/nugget/common
CPPFLAGS += -Isource
LDFLAGS += -Lthirdparty/nugget/psyq/lib
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgpu
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -Wl,--end-group

include thirdparty/nugget/common.mk
