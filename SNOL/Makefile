CXX      := g++
CXXFLAGS := -ggdb -std=c++17
CPPFLAGS := -MMD

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

SRCS     := SNOL.cpp
DEPS     := $(SRCS:.cpp=.d)


SNOL: Expr.h Stmt.h SNOL.o
	@$(COMPILE) SNOL.o -o $@

.PHONY: clean
clean:
	rm -f *.d *.o SNOL
