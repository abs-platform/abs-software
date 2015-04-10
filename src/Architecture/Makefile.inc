CC = gcc
CFLAGS = -Wall
SHARED = -shared
CFLAGS_SHARED = -g -fpic
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(LIBRARY_PATH)

export LD_LIBRARY_PATH

# Link library
%.so: %.so.o
	$(CC) $(SHARED) $< -o lib$@

# Link library with DEBUG macro set
%.debug.so: %.debug.so.o
	$(CC) $(SHARED) $< -o lib$@

# Compile object for library
%.so.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_SHARED) -I$(INCLUDE_PATH) -c $< -o $@

# Compile object for library with DEBUG macro set
%.debug.so.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_SHARED) -I$(INCLUDE_PATH) -DDEBUG=1 -c $< -o $@

# Compile object
%.x: %.o
	$(CC) -L$(LIBRARY_PATH) -l$(LOCAL_LIB) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -c $< -o $@

%.rec: %
	$(MAKE) -C $<

%.rec.clean: %
	$(MAKE) -C $< clean
