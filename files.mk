tinyheap = ../generic/tinyheap/src
FLAGS	+= -DCONFIG_BUILD_TINYHEAP
INC	+= -I${tinyheap}
CPATH	+= ${tinyheap}
CFILES	+= tinyheap.c

