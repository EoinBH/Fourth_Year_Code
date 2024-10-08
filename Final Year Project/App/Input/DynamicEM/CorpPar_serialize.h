#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include "CmdLineVersions.h"

// serialise a single Occ object
template<class Archive> 
void serialize(Archive &ar, CorpPar &obj) {


  ar( obj.target_alternates );
  ar( obj.corpus_type );
  ar( obj.file_list_file );
  ar( obj.whether_csv_suffix );
  ar( obj.grouping_size );
  ar( obj.left );
  ar( obj.right );
  ar( obj.whether_pad );
  ar( obj.lowercase );
  ar( obj.filter_ngram_meta_token );
  ar( obj.syn_token_parts );
  ar( obj.excl_targ_nnp );
  ar( obj.wlp );
  ar( obj.targ_wlp );
  ar( obj.excl_targ_np1 );
  ar( obj.filter_coha_at_token );
  ar( obj.whole_coha_article );
  ar( obj.symbol_table_name );
  ar( obj.closed );

}
