.PHONY : all

include test.txt

all :
	@echo "this is $@"
    
test.txt :
	@echo "this is $@"