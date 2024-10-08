#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include "Occ.h"

// serialise a single Occ object
template<class Archive> 
void serialize(Archive &ar, Occ &obj) { 
  ar( obj.words ); 
  ar( obj.true_sense );
  ar( obj.inferred_sense );
  ar( obj.year );
  ar( obj.data_prob );
  ar( obj.max_prob );
  ar( obj.diff_to_max );
  ar( obj.rpt_cntr ); 
  ar( obj.size_left );
  ar( obj.size_right ); 
  ar( obj.num_L ); 
  ar( obj.num_R );
}
