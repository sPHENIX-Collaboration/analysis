CFLAGS = -g3 -pthread -m64 -Wno-deprecated -I src/ 
CFLAGS += $(shell root-config --cflags --libs)

all : TrackletAna plotTracklets plotRecoCluster_INTT BeamspotReco INTTVtxZ

TrackletAna : src/TrackletAna.cxx src/Hit.h src/Tracklet.h
	g++ ${CFLAGS} src/TrackletAna.cxx src/Hit.h src/Tracklet.h -o TrackletAna

plotRecoCluster_INTT : src/plotRecoCluster_INTT.cxx
	g++ $(CFLAGS) src/plotRecoCluster_INTT.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotRecoCluster_INTT

plotTracklets : src/plotTracklets.cxx
	g++ $(CFLAGS) src/plotTracklets.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotTracklets

INTTVtxZ : src/INTTVtxZ.cxx
	g++ $(CFLAGS) src/INTTVtxZ.cxx -o INTTVtxZ

BeamspotReco : src/BeamspotReco.cxx
	g++ $(CFLAGS) src/BeamspotReco.cxx -I$(OFFLINE_MAIN)/include -o BeamspotReco

clean : 
	rm TrackletAna plotTracklets plotRecoCluster_INTT BeamspotReco INTTVtxZ
