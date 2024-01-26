CFLAGS = -pthread -m64 -Wno-deprecated -I src/
CFLAGS += $(shell root-config --cflags --libs)

all : TrackletAna plotTracklets plotRecoCluster_INTT INTTVtxZ_test BeamspotReco INTTVtxZ BeamspotReco_Quadrant

TrackletAna : src/TrackletAna.cxx src/Hit.h src/Tracklet.h
	g++ ${CFLAGS} src/TrackletAna.cxx src/Hit.h src/Tracklet.h -o TrackletAna

plotRecoCluster_INTT : src/plotRecoCluster_INTT.cxx
	g++ $(CFLAGS) src/plotRecoCluster_INTT.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotRecoCluster_INTT

plotTracklets : src/plotTracklets.cxx
	g++ $(CFLAGS) src/plotTracklets.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotTracklets

INTTVtxZ_test : src/INTTVtxZ_test.cxx
	g++ $(CFLAGS) src/INTTVtxZ_test.cxx -o INTTVtxZ_test

INTTVtxZ : src/INTTVtxZ.cxx
	g++ $(CFLAGS) src/INTTVtxZ.cxx -o INTTVtxZ

BeamspotReco : src/BeamspotReco.cxx
	g++ $(CFLAGS) src/BeamspotReco.cxx -I$(OFFLINE_MAIN)/include -o BeamspotReco

BeamspotReco_Quadrant : src/BeamspotReco_Quadrant.cxx
	g++ $(CFLAGS) src/BeamspotReco_Quadrant.cxx -o BeamspotReco_Quadrant
