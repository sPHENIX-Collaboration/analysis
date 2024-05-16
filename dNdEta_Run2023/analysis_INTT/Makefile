CFLAGS = -g3 -pthread -m64 -Wno-deprecated -I src/ 
CFLAGS += $(shell root-config --cflags --libs)
CFLAGS += -lEG

all : TrackletAna plotTrkrHit plotTracklets plotRecoVtx plotCluster BeamspotReco INTTVtxZ Corrections GeoAccepCorr

TrackletAna : src/TrackletAna.cxx src/Hit.h src/Tracklet.h
	g++ ${CFLAGS} src/TrackletAna.cxx src/Hit.h src/Tracklet.h -o TrackletAna

plotTrkrHit : src/plotTrkrHit.cxx
	g++ $(CFLAGS) src/plotTrkrHit.cxx -o plotTrkrHit

plotCluster : src/plotCluster.cxx
	g++ $(CFLAGS) src/plotCluster.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotCluster

plotRecoVtx : src/plotRecoVtx.cxx
	g++ $(CFLAGS) src/plotRecoVtx.cxx -o plotRecoVtx

plotTracklets : src/plotTracklets.cxx
	g++ $(CFLAGS) src/plotTracklets.cxx src/Hit.h src/Tracklet.h src/GenHadron.h -o plotTracklets

INTTVtxZ : src/INTTVtxZ.cxx
	g++ $(CFLAGS) src/INTTVtxZ.cxx -o INTTVtxZ

BeamspotReco : src/BeamspotReco.cxx
	g++ $(CFLAGS) src/BeamspotReco.cxx -I$(OFFLINE_MAIN)/include -o BeamspotReco

Corrections : src/Corrections.cxx
	g++ $(CFLAGS) src/Corrections.cxx -o Corrections

GeoAccepCorr : src/GeoAccepCorr.cxx
	g++ $(CFLAGS) src/GeoAccepCorr.cxx -o GeoAccepCorr

clean :
	rm TrackletAna plotTrkrHit plotTracklets plotRecoVtx plotCluster BeamspotReco INTTVtxZ Corrections GeoAccepCorr
