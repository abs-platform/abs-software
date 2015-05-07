#####################################################################################################
# SYSCORE MAKEFILE                                                             v1.5.5 (09-abr-2014) #
#####################################################################################################

# Application that you want to compile and link. Must have the same name as C file containing 
# the 'main()' function.
APPLICATION = syscore

# Source files (including the main C file)
SOURCES = shed.c \
          shed_local.c \
          shed_mat.c
          
# Paths to search the sources (separated by a colon ':')
VPATH = 

# Extra Compiler and Linker Flags:
EXTRACFLAGS = -D_GNU_SOURCE -DSYSCORE_VERBOSE -DSYSCORE_DEBUG
EXTRALDFLAGS = 

# Default configuration:
ARM=false#           # Compile for ARM architecture: { true / false }
RT=false#            # Compile with Real-Time libraries: { true / false }
CONF=native#        # Real-time libraries: { posix / native }
CC_HOST=gcc#        # Compiler for your architecture (usually: gcc)
CC_TARGET=arm-linux-gnueabi-gcc# #Cross-compiler for the target device (usually: arm-linux-gnueabi-gcc)

# Name of your toolchain for cross-compile code for ARM machines. 


#####################################################################################################
#####################################################################################################
# Makefile configuration. Do not modify.
MV            := 1.5.5

ifeq ($(ARM),true)
    TOOLCHAIN := $(CC_TARGET)
else
    TOOLCHAIN := $(CC_HOST)
endif

BINDIR        := .
OBJDIR        := obj
OBJS          := $(addprefix $(OBJDIR)/,$(SOURCES:%.c=%.o))
OBJSLF         = $(addprefix '\n------------:',$(OBJS)])
CC_BASE_DIR   := $(subst -gcc,,$(TOOLCHAIN))
CC_BASE_DIR   := $(subst gcc,,$(CC_BASE_DIR))
ifneq (,$(CC_BASE_DIR))
    CC_BASE_DIR := $(CC_BASE_DIR)/
endif

XENO_INCLUDE   = /usr/$(CC_BASE_DIR)include/xenomai
SKYW_INCLUDE   = /usr/$(CC_BASE_DIR)include/skywalker

XENO_LIB       = /usr/$(CC_BASE_DIR)lib/xenomai
SKYW_LIB       = /usr/$(CC_BASE_DIR)lib/skywalker

BASIC_CFLAGS   = -Wall
NATIVE_CFLAGS  = -I$(XENO_INCLUDE) -D_GNU_SOURCE -D_REENTRANT -D__XENO__
POSIX_CFLAGS   = $(NATIVE_CFLAGS) -I$(XENO_INCLUDE)/posix 
BASIC_LDFLAGS  = -L/$(SKYW_LIB) 
NATIVE_LDFLAGS = -lnative -L$(XENO_LIB) -lxenomai -lpthread 
POSIX_LDFLAGS  = -Wl,@$(XENO_LIB)/posix.wrappers -L$(XENO_LIB) -lpthread_rt -lxenomai -lpthread -lrt  

ifneq ($(CONF),quiet)
all: show_config | $(APPLICATION)
else
all: $(APPLICATION) 
endif

ifeq ($(RT),true)
ifneq ($(CONF),posix)
$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@$(TOOLCHAIN) -c $< -o $@ $(BASIC_CFLAGS) $(NATIVE_CFLAGS) $(EXTRACFLAGS) && echo 'done. [RT : Native skin used]'

$(APPLICATION) : $(OBJS) | $(BINDIR) $(OBJDIR)
	@echo -n -e '---------: LINKING : '
	@$(TOOLCHAIN) $(OBJS) -o $@ $(BASIC_LDFLAGS) $(NATIVE_LDFLAGS) $(EXTRALDFLAGS) && echo 'done. [RT : Native skin used]'
else
$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@$(TOOLCHAIN) -c $< -o $@ $(BASIC_CFLAGS) $(POSIX_CFLAGS) $(EXTRACFLAGS) && echo 'done. [RT : POSIX skin used]'

$(APPLICATION) : $(OBJS) | $(BINDIR) $(OBJDIR)
	@echo -n -e '---------: LINKING : '
	@$(TOOLCHAIN) $(OBJS) -o $@ $(BASIC_LDFLAGS) $(POSIX_LDFLAGS) $(EXTRALDFLAGS) && echo 'done. [RT : POSIX skin used]'
endif
else
$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@$(TOOLCHAIN) -c $< -o $@ $(BASIC_CFLAGS) $(EXTRACFLAGS) && echo 'done.'

$(APPLICATION) : $(OBJS) | $(BINDIR) $(OBJDIR)
	@echo -n -e '---------: LINKING : '
	@$(TOOLCHAIN) $(OBJS) -o $@ $(BASIC_LDFLAGS) $(EXTRALDFLAGS) && echo 'done.'
endif
$(OBJS): | $(BINDIR) $(OBJDIR)

$(OBJDIR): 
	@mkdir -p $(OBJDIR)
	
$(BINDIR):
	@mkdir -p $(BINDIR)

show_config:
	@echo '---------: APPLICATION  : $(APPLICATION)'
	@echo '---------: OBJS         : $(SOURCES:%.c=%.o)'
	@echo '---------: XENO. INCLUDE: $(XENO_INCLUDE)'
	@echo '---------: XENO. LIB    : $(XENO_LIB)'
	@echo '---------: SKYW. INCLUDE: $(SKYW_INCLUDE)'
	@echo '---------: SKYW. LIB    : $(SKYW_LIB)'
	@echo '---------: TOOLCHAIN    : $(TOOLCHAIN)'
	@echo '---------: EXTRACFLAGS  : $(EXTRACFLAGS)'
	@echo '---------: EXTRALDFLAGS : $(EXTRALDFLAGS)'
	@echo '---------: ARM ARCH.    : $(ARM)'
	@echo '---------: CONFIG.      : $(CONF)'
	@echo '---------: REAL-TIME    : $(RT)'
	@echo '---------: MAKEFILE VER.: $(MV)'
	@echo -n '---------: ' && date +"%Y/%m/%d   : %T"

clean:
	@echo -n '---------: REMOVING $(BINDIR)/$(APPLICATION)...' && rm $(BINDIR)/$(APPLICATION) -f && echo 'done.'
	@echo -n '---------: REMOVING $(OBJDIR)...' && rm $(OBJDIR) -r -f && echo 'done.'
	
####################################################################################################
# xeno-config OUTPUT:
#
# NATIVE CFLAGS:
#  -I/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__
#
# NATIVE LDFLAGS:
#  -lnative -L/usr/xenomai/lib -lxenomai -lpthread 
#
# POSIX CFLAGS:
#  -I/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__ -I/usr/xenomai/include/posix
#
# POSIX LDFLAGS:
#  -Wl,@/usr/xenomai/lib/posix.wrappers -L/usr/xenomai/lib -lpthread_rt -lxenomai -lpthread -lrt 
####################################################################################################
