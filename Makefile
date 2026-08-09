.PHONY: all app bridge clean
all:
	./scripts/build-all.sh
app:
	./app-native/build.sh
bridge:
	./bridge/build.sh
clean:
	rm -rf app-native/build bridge/build app-native/.local
