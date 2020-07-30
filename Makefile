KRNSRCFILES := $(shell find kernel -type f -name "\*.c")
KRNOBJFILES := $(patsubst %.c,%.o,$(KRNSRCFILES))

.PHONY: build run

build: $(KRNOBJFILES)
	