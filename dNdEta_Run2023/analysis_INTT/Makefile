CFLAGS = -pthread -m64 -Wno-deprecated -I src/
CFLAGS += $(shell root-config --cflags --libs)

all : TrackletAna plotTracklets plotRecoCluster_INTT INTTVtxZ_test BeamspotReco

TrackletAna : src/TrackletAna.cxx src/Hit.h src/Tracklet.h
	g++ ${CFLAGS} src/TrackletAna.cxx src/Hit.h src/Tracklet.h -o TrackletAna

plotRecoCluster_INTT : src/plotRecoCluster_INTT.cxx
	g++ $(CFLAGS) src/plotRecoCluster_INTT.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotRecoCluster_INTT

plotTracklets : src/plotTracklets.cxx
	g++ $(CFLAGS) src/plotTracklets.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotTracklets

INTTVtxZ_test : src/INTTVtxZ_test.cxx
	g++ $(CFLAGS) src/INTTVtxZ_test.cxx -o INTTVtxZ_test

BeamspotReco : src/BeamspotReco.cxx
	g++ $(CFLAGS) src/BeamspotReco.cxx -I$(OFFLINE_MAIN)/include -o BeamspotReco
