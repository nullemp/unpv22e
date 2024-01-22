#e.Makefile
.PHONY : all

-include test.txt

all :
	@echo "this is $@"
    
test.txt : b.txt
	@echo "creating $@ ..."
	@echo "all : c.txt" > test.txt