CC=gcc
CFLAGS=-Wall -c
LDFLAGS=-I ./include/

SRC_DIR=./src
INC_DIR=./include
BIN_DIR=./bin
DOC_DIR=./docs
GCOV_DIR=./gcov

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
	# generate some data for gcov by calling the generated binary with various options
	$(GCOV_DIR)/$(GEXEC) -h
	$(GCOV_DIR)/$(GEXEC) -i input -o output -v

	find ./ -maxdepth 1 -name *.gcno -exec mv {} $(GCOV_DIR) \;
	find ./ -maxdepth 1 -name *.gcda -exec mv {} $(GCOV_DIR) \;

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

.PHONY: all docs gcov package clean mrproper
