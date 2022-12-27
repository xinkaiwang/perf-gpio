
.PHONY: all
all:	gyp

config:
	node-gyp configure

gyp: config
	node-gyp build

publish:
	npm publish

clean:
	rm -f build
