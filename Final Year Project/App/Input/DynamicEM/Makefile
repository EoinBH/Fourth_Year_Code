# note on reasonably up-to-date install do not need the -std=... flag
#CPPFLAGS = -g
CPPFLAGS = -O2 -I../cereal-master/include -std=c++11
#CPPFLAGS = -g -DEM_DEBUG
#CPPFLAGS = -g -DCAT_DEBUG

CMDLINE_OBJ = CmdLine.o CmdLineVersions.o
CORP_OBJ = process_corpus.o years.o sym_table.o GoogleOcc.o Occ.o NGramToken.o SynGramToken.o CohaToken.o senses.o
ARCH_OBJ = process_corpus_frm_archive.o



.PHONY: MakeVersion make_version_generate_symbol_table


senses.o: senses.cpp senses.h StringUtil.h
	g++ $(CPPFLAGS) -c senses.cpp

StringUtil.o: StringUtil.cpp StringUtil.h
	g++ $(CPPFLAGS) -c StringUtil.cpp

sym_table.o: sym_table.cpp
	g++ $(CPPFLAGS) -c sym_table.cpp

Occ.o: Occ.cpp Occ.h senses.h
	g++ $(CPPFLAGS) -c Occ.cpp

GoogleOcc.o: GoogleOcc.cpp GoogleOcc.h
	g++ $(CPPFLAGS) -c GoogleOcc.cpp

NGramToken.o: NGramToken.cpp NGramToken.h
	g++ $(CPPFLAGS) -c NGramToken.cpp

SynGramToken.o: SynGramToken.cpp SynGramToken.h
	g++ $(CPPFLAGS) -c SynGramToken.cpp

CohaToken.o: CohaToken.cpp CohaToken.h
	g++ $(CPPFLAGS) -c CohaToken.cpp

process_corpus.o: process_corpus.cpp process_corpus.h Occ.h GoogleOcc.h SynGramToken.h CohaToken.h StringUtil.h
	g++ $(CPPFLAGS) -c process_corpus.cpp 

prob_tables.o: prob_tables.cpp prob_tables.h sym_table.h Occ.h distancetype.h
	g++ $(CPPFLAGS) -c prob_tables.cpp

probs_calc.o: probs_calc.cpp probs_calc.h prob_tables.h sym_table.h Occ.h
	g++ $(CPPFLAGS) -c probs_calc.cpp

uniftype.o: uniftype.cpp uniftype.h
	g++ -c $(CPPFLAGS)  uniftype.cpp

CmdLine.o: CmdLine.cpp CmdLine.h
	g++ $(CPPFLAGS) -c CmdLine.cpp

CmdLineVersions.o: CmdLineVersions.cpp CmdLineVersions.h CmdLine.h
	g++ $(CPPFLAGS) -c CmdLineVersions.cpp

dynamicEM.o: dynamicEM.cpp Occ.h CmdLine.h CmdLineVersions.h timestamp.h process_corpus_frm_archive.h
	g++ $(CPPFLAGS) -c dynamicEM.cpp

dynamicEM: dynamicEM.o $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o  compute_distance.o probs_calc.o StringUtil.o uniftype.o MakeVersion
	g++ $(CPPFLAGS) dynamicEM.o $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o  compute_distance.o probs_calc.o StringUtil.o uniftype.o Version.cpp -o dynamicEM

inspector: inspector.cpp $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o probs_calc.o StringUtil.o uniftype.o compute_distance.o
	g++ $(CPPFLAGS) inspector.cpp $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o probs_calc.o StringUtil.o uniftype.o compute_distance.o -o inspector

make_coha_counts.o: make_coha_counts.cpp
	g++ $(CPPFLAGS) -c make_coha_counts.cpp


make_coha_counts: make_coha_counts.o $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o probs_calc.o StringUtil.o uniftype.o compute_distance.o
	g++ $(CPPFLAGS) make_coha_counts.o $(CMDLINE_OBJ) $(CORP_OBJ) $(ARCH_OBJ) prob_tables.o probs_calc.o StringUtil.o uniftype.o compute_distance.o -o make_coha_counts



cereal_tester: cereal_tester.cpp $(CMDLINE_OBJ) $(CORP_OBJ) StringUtil.o
	g++ $(CPPFLAGS) cereal_tester.cpp $(CMDLINE_OBJ) $(CORP_OBJ) StringUtil.o -o cereal_tester

corpus_cereal_maker: corpus_cereal_maker.cpp $(CMDLINE_OBJ) $(CORP_OBJ) StringUtil.o Occ_serialize.h CorpPar_serialize.h
	g++ $(CPPFLAGS) corpus_cereal_maker.cpp $(CMDLINE_OBJ) $(CORP_OBJ) StringUtil.o -o corpus_cereal_maker

process_corpus_frm_archive.o: process_corpus_frm_archive.cpp process_corpus.h Occ.h GoogleOcc.h SynGramToken.h CohaToken.h StringUtil.h Occ_serialize.h CorpPar_serialize.h
	g++ $(CPPFLAGS) -c process_corpus_frm_archive.cpp 


MakeVersion:
	git show HEAD | head -n 5 | ./make_version.pl > Version.cpp


clean:
	rm -f *.o;
	rm -f dynamicEM inspector make_coha_counts corpus_cereal_maker;
