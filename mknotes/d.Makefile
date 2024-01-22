.PHONY : all

include test.txt

all :
	@echo "this is $@"
    
test.txt : b.txt
	@echo "creating $@"
	@echo "other : ; @echo "this is other" " > test.txt