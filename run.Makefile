.ONESHELL:

DATA_DIR := /data
CZI_FILES := $(wildcard $(DATA_DIR)/*.czi)
UNPROCESSED := $(filter-out %_20x.czi, $(CZI_FILES))
TARGETS := $(patsubst %.czi, %_20x.czi, $(UNPROCESSED))

CZIRESIZE := build/cziresize
MINSIZE := 500000000 # minimum file size to be counted as a valid conversion (500MB)


all: $(TARGETS)

%_20x.czi: %.czi
	rm -f $@.tmp
	echo Running $(CZIRESIZE) $^ $@.tmp
	$(CZIRESIZE) $^ $@.tmp
	test -f "$@.tmp" || exit 1
	echo Moving $@.tmp -> $@
	mv $@.tmp $@
	test `stat --format %s $@` -gt $(MINSIZE) && \
	  echo Removing $^ && \
	  rm $^


$(CZIRESIZE):
	cd build
	cmake
	cmake --build .