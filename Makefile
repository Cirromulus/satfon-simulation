ifneq ($(JOBS),)
  MAKEJOBS=-j$(JOBS)
else
  # Default is 3 parallel jobs. Use `JOBS=` to disable parallel build.
  NPROCS:=3
  OS:=$(shell uname -s)
  ifeq ($(OS),Linux)
    NPROCS:=$(shell nproc)
  endif
  MAKEJOBS=-j$(NPROCS)
endif

FVIEWERDIR=./flashView/
MVIEWERDIR=./mramView/
SCONS=scons-2

all: mview fview example

fview:
	$(SCONS) $(MAKEJOBS) -C $(FVIEWERDIR)

mview:
	$(SCONS) $(MAKEJOBS) -C $(MVIEWERDIR)

example:
	$(SCONS) $(MAKEJOBS) -C test/

clean:
	$(SCONS) -C $(FVIEWERDIR) -c
	$(SCONS) -C $(MVIEWERDIR) -c
	$(SCONS) -C test/ -c
