.PHONY: test

app:
	gcc -g3 main.c -o app -lm

test: ../../build/dstream
	cd ../.. && \
	./build/dstream

../../build/dstream:
	make -C ../..
