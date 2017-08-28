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

VIEWERDIR=./flashView/

viewer:
	@scons $(MAKEJOBS) -C $(VIEWERDIR)
	
clean:
	@scons -C $(VIEWERDIR) -c
