CFLAGS = -pthread -m64 -Wno-deprecated -I src/
CFLAGS += $(shell root-config --cflags --libs)

all : Tracklet_RecoVtx

Tracklet_RecoVtx : src/Tracklet_RecoVtx.cxx src/Hit.h src/Vertex.h src/Utilities.h
	g++ ${CFLAGS} src/Tracklet_RecoVtx.cxx src/Hit.h src/Vertex.h src/Utilities.h -o Tracklet_RecoVtx
