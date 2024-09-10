#pragma once

//////////////////////////////////////////////////////////////////////////////////////////
// Important variables ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
const unsigned int kFelix_num = 8;
const unsigned int kLadders_in_felix_num = 14;
const unsigned int kLadders_in_roc_num = 7;
const unsigned int kChip_num = 26;
const unsigned int kCh_num = 128;

const int kFirst_physics_run = 43215; // Data directories in lustre was changed at some point. This is the first run under /sphenix/lustre01/sphnxpro/physics

//////////////////////////////////////////////////////////////////////////////////////////
// Paths /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
const string kIntt_commissioning_evt_dir = "/sphenix/lustre01/sphnxpro/commissioning/INTT/";  
const string kIntt_physics_evt_dir = "/sphenix/lustre01/sphnxpro/physics/INTT/";  
string kIntt_evt_dir = kIntt_physics_evt_dir;

const string kOfficial_Dst_dir = "/sphenix/lustre01/sphnxpro/commissioning/slurp/";
const string kOfficial_physics_Dst_dir = "/sphenix/lustre01/sphnxpro/physics/slurp/streaming/physics/";

const unsigned int kRun_type_num = 7;
const string kRun_types[ kRun_type_num ] = { "physics", "cosmics", "beam", "calib", "pedestal", "junk", "intt" };

const unsigned int kYear = 2024;
const string kIntt_data_dir	= "/sphenix/tg/tg01/commissioning/INTT/data/";
const string kIntt_dst_dir	= kIntt_data_dir + "dst_files/" + to_string( kYear ) + "/";
const string kIntt_root_dir	= kIntt_data_dir + "root_files/" + to_string( kYear ) + "/";
const string kIntt_cdb_dir	= kIntt_data_dir + "CDB_files/" + to_string( kYear ) + "/";
const string kIntt_hot_ch_cdb_dir = kIntt_cdb_dir + "hot_ch_map/";
const string kIntt_bco_diff_cdb_dir = kIntt_cdb_dir + "bco_diff_map/";

/*
const string kIntt_qa_dir	= "/sphenix/tg/tg01/commissioning/INTT/QA/";
const string kIntt_hitmap_dir	= kIntt_qa_dir + "hitmap/" + to_string( kYear ) + "/";
const string kIntt_hotmap_dir	= kIntt_qa_dir + "hotdeadmap/" + to_string( kYear ) + "/";
const string kIntt_qa_cosmics_dir = kIntt_qa_dir + "cosmics/" + to_string( kYear ) + "/";
*/
const string kIntt_qa_dir	= "/sphenix/tg/tg01/commissioning/INTT/QA/";
const string kIntt_hitmap_dir	= kIntt_qa_dir + "hitmap/" + to_string( kYear ) + "/";
const string kIntt_hotmap_dir	= kIntt_qa_dir + "hotdeadmap/";
const string kIntt_qa_hotmap_dir= kIntt_qa_dir + "hotdeadmap/";
const string kIntt_qa_cosmics_dir = kIntt_qa_dir + "cosmics/";
const string kIntt_qa_bco_diff_dir = kIntt_qa_dir + "bco_bcofull_difference/";// plots/2024/

//////////////////////////////////////////////////////////////////////////////////////////
// Misc /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
const int kEvent_num_per_dst = 250;
