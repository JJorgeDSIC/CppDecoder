CMAKE = cmake
BUILD_TYPE = Debug
TARGET = all
BUILD_TOOL = Ninja

.PHONY:all
all: configure build

.PHONY: configure
configure:
	${CMAKE} -H. -Bbuild -G ${BUILD_TOOL}

.PHONY: build
build:
	${CMAKE} --build build --config ${BUILD_TYPE} --target ${TARGET} -- -j 10

.PHONY: clean
clean:
	${CMAKE} --build build --config ${BUILD_TYPE} --target clean
