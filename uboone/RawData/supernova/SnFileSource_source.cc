
// Wrapper file that ties my real implimentation to the name.

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "SnFileSourceDriver.h"

namespace snassembler {
  typedef art::Source<SnFileSourceDriver> SnFileSource;
}

DEFINE_ART_INPUT_SOURCE(snassembler::SnFileSource)
