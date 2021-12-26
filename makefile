NAME		=LR
TEST		=tests

all: compile run lcov remove genhtml

compile:
	g++ --coverage $(NAME).cpp $(TEST).cpp -o $(NAME)_tests -lgtest -lpthread

run:
	./$(NAME)_tests

lcov:
	mkdir test_coverage
	lcov --directory . --capture -o test_coverage/lcov.info

remove:
	mv test_coverage/lcov.info test_coverage/lcov_orig.info
	lcov --remove test_coverage/lcov_orig.info "/usr/*" "*/tests.cpp" -o test_coverage/lcov.info
	

genhtml: 
	genhtml -o test_coverage/ test_coverage/lcov.info --highlight --legend --title "LR coverage"
