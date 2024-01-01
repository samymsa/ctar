CC=gcc
CFLAGS=-Wall -c
LDFLAGS=-I ./include/ -lz

SRC_DIR=./src
INC_DIR=./include
BIN_DIR=./bin
DOC_DIR=./docs
GCOV_DIR=./gcov
TEST_DIR=./tests

GCOVFLAGS=-O0 --coverage -lgcov -Wall -g

LCOV_REPORT=report.info

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:.c=.o)
EXEC=ctar

GEXEC=$(EXEC).cov

AR_NAME=archive_$(EXEC).tar.gz


all: $(SRC) $(EXEC)
    
%.o:%.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(EXEC): $(OBJ) 
	$(CC) -o $(BIN_DIR)/$@ -Wall $(OBJ) $(LDFLAGS)

$(GEXEC):
	$(CC) $(GCOVFLAGS) -o $(GCOV_DIR)/$@ -Wall $(SRC) $(LDFLAGS)

docs:
	doxygen $(DOC_DIR)/Doxyfile

gcov: $(GEXEC)
	# Run tests to generate gcov files
	mkdir -p $(TEST_DIR)
	$(GCOV_DIR)/$(GEXEC) || true
	$(GCOV_DIR)/$(GEXEC) -h || true
	$(GCOV_DIR)/$(GEXEC) --wrong || true
	$(GCOV_DIR)/$(GEXEC) -c tests/test.tar . || true
	$(GCOV_DIR)/$(GEXEC) -c tests/test.tar -d include/ . || true
	$(GCOV_DIR)/$(GEXEC) -c tests/test.tar -d .github/ workflows || true
	$(GCOV_DIR)/$(GEXEC) -l tests/test.tar || true
	$(GCOV_DIR)/$(GEXEC) -l tests/test.tar -v || true
	$(GCOV_DIR)/$(GEXEC) -e tests/test.tar -d tests/ || true
	$(GCOV_DIR)/$(GEXEC) -e tests/test.tar -d tests/ -v || true

	$(GCOV_DIR)/$(GEXEC) -c tests/test.tar.gz -z -v -d include/ . || true
	$(GCOV_DIR)/$(GEXEC) -c tests/test.tar.gz -z -v src || true
	$(GCOV_DIR)/$(GEXEC) -l tests/test.tar.gz -z || true
	$(GCOV_DIR)/$(GEXEC) -l tests/test.tar.gz -z -v || true
	$(GCOV_DIR)/$(GEXEC) -e tests/test.tar.gz -d tests/ -z || true
	$(GCOV_DIR)/$(GEXEC) -e tests/test.tar.gz -d tests/ -v -z || true

	# Generate the report
	gcov -o $(GCOV_DIR) $(GEXEC)
	lcov -o $(GCOV_DIR)/$(LCOV_REPORT) -c -f -d $(GCOV_DIR)
	genhtml -o $(GCOV_DIR)/report $(GCOV_DIR)/$(LCOV_REPORT)

package: gcov docs all
	rm -rf $(AR_NAME)
	tar cvfz $(AR_NAME) ./*
clean:	
	rm -rf $(OBJ)

mrproper: clean
	rm -rf $(BIN_DIR)/*
	rm -rf $(DOC_DIR)/latex/
	rm -rf $(DOC_DIR)/html/
	rm -rf $(GCOV_DIR)/*
	rm -rf $(TEST_DIR)/*

.PHONY: all docs tests gcov package clean mrproper
