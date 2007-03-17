COMMANDS = Command FireCommand HelloCommand ReadyCommand
%: %.java
	@echo "* $@.java"
	@rm errors.$@ 2> /dev/null > /dev/null || true
	@echo "javac $@.java"
	@javac $@.java 2> errors.$@ || echo "Errors occurred. Output can be found in ./errors.$@" && exit 1

server: BattleshipServer $(COMMANDS) TCPServer ServerGame
