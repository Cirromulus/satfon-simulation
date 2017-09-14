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

all: mview fview

fview:
	@scons $(MAKEJOBS) -C $(FVIEWERDIR)

mview:
	@scons $(MAKEJOBS) -C $(MVIEWERDIR)

example:
	@scons $(MAKEJOBS) -C test/
	
clean:
	@scons -C $(FVIEWERDIR) -c
	@scons -C $(MVIEWERDIR) -c
	@scons -C test/ -c
