.PHONY : all

include test.txt

all :
	@echo "this is $@"
    
test.txt :
	@echo "creating $@ ..."
	@echo "other"
