CMAKE = cmake
BUILD_TYPE = Debug
TARGET = all

.PHONY:all
all: configure build

.PHONY: configure
configure:
	${CMAKE} -H. -Bbuild -G Ninja

.PHONY: build
build:
	${CMAKE} --build build --config ${BUILD_TYPE} --target ${TARGET} -- -j 10

.PHONY: clean
clean:
	${CMAKE} --build build --config ${BUILD_TYPE} --target clean
