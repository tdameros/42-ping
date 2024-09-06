#*******************************  VARIABLES  **********************************#

NAME			=	ft_ping

# --------------- FILES --------------- #

include src.mk

# ------------ DIRECTORIES ------------ #

DIR_BUILD		=	.build/
DIR_INCLUDES 	=	\
					$(DIR_INCLUDE) \

# ------------- SHORTCUTS ------------- #

OBJ				=	$(patsubst %.c, $(DIR_BUILD)%.o, $(SRC))
DEP				=	$(patsubst %.c, $(DIR_BUILD)%.d, $(SRC))
SRC				=	$(addprefix $(DIR_SRC),$(LIST_SRC))
INCLUDES		=	$(addprefix -I , $(DIR_INCLUDES))

# ------------ COMPILATION ------------ #

CFLAGS			=	-Wall -Wextra -Werror -O3
DEBUG_CFLAGS	=	-Wall -Wextra -Werror -fsanitize=address -g3
LIBS			=	-lm

DEP_FLAGS		=	-MMD -MP

# -------------  COMMANDS ------------- #

CPPCHECK		=	cppcheck
CLANG_FORMAT	=	clang-format-15
RM				=	rm -rf
MKDIR			=	mkdir -p
OS				=	$(shell uname -s)

#***********************************  RULES  **********************************#

.PHONY: all
all:
				$(MAKE) $(NAME)

# ---------- VARIABLES RULES ---------- #

$(NAME):		$(OBJ)
				$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJ) $(LIBS)

# ---------- COMPILED RULES ----------- #

-include $(DEP)

$(DIR_BUILD)%.o: %.c
				mkdir -p $(shell dirname $@)
				$(CC) $(CFLAGS) $(DEP_FLAGS) $(INCLUDES) -c $< -o $@

.PHONY: run
run:	all
				./$(NAME)

.PHONY: clean
clean:
				$(RM) $(DIR_BUILD)

.PHONY: fclean
fclean: clean
				$(RM) $(NAME)

.PHONY: re
re:				fclean
				$(MAKE) all

.PHONY: debug
debug:	fclean
				$(MAKE) all CFLAGS="$(DEBUG_CFLAGS)"

.PHONY: check-format
check-format:
				$(CLANG_FORMAT) -style=file $(SRC) -n --Werror

.PHONY: format
format:
				$(CLANG_FORMAT) -style=file $(SRC) -i

.PHONY: cppcheck
cppcheck:
				$(CPPCHECK) --quiet --enable=all --inline-suppr --force \
				--error-exitcode=1 \
				--suppress=missingIncludeSystem \
				--std=c99 --language=c $(INCLUDES) $(SRC)

.PHONY: build_docker_image
build_docker_image:
				docker build . -t ft_ping

.PHONY: run_docker_container
run_docker_container:
				docker run --rm -it -v .:/ft_ping/ ft_ping
